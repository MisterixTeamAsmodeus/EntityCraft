#include "PostgreAdapter/postgreconnection.h"

#include "DatabaseAdapter/exception/opendatabaseexception.h"
#include "DatabaseAdapter/exception/sqlexception.h"
#include "DatabaseAdapter/idatabasedriver.h"
#include "DatabaseAdapter/model/databasesettings.h"

#include <iostream>
#include <sstream>

namespace database_adapter {
namespace postgre {

std::shared_ptr<ILogger> connection::_logger = nullptr;

void connection::set_logger(std::shared_ptr<ILogger>&& logger)
{
    _logger = std::move(logger);
}

connection::connection(const settings& settings)
    : IConnection(settings)
{
    connect(settings);
}

connection::~connection()
{
    disconnect();
}

bool connection::is_valid()
{
    if(_connection == nullptr)
        return false;

    try {
        exec("select 1");
        return true;
    } catch(const sql_exception&) {
        return false;
    }
}

query_result connection::exec(const std::string& query)
{
    if(_logger != nullptr) {
        _logger->log_sql(query);
    }

    auto* query_result = PQexec(_connection, query.c_str());
    if(PQresultStatus(query_result) != PGRES_TUPLES_OK && PQresultStatus(query_result) != PGRES_COMMAND_OK) {
        PQclear(query_result);

        std::string last_error = "Failed to execute statement: ";
        last_error.append(PQerrorMessage(_connection));

        if(_logger != nullptr) {
            _logger->log_error(last_error);
        }

        throw sql_exception(std::move(last_error), query);
    }

    const auto rows = PQntuples(query_result);
    const auto cols = PQnfields(query_result);

    query_result result;
    for(int i = 0; i < rows; i++) {
        query_result::result_row row;
        for(int j = 0; j < cols; j++) {
            auto* column_value = PQgetvalue(query_result, i, j);
            row.emplace(PQfname(query_result, j), column_value == nullptr ? "" : column_value);
        }
        result.add_row(row);
    }

    PQclear(query_result);

    return result;
}

void connection::prepare(const std::string& query, const std::string& name)
{
    if(_logger != nullptr) {
        _logger->log_sql("Prepare query " + name + " sql: " + query);
    }
    auto* query_result = PQprepare(_connection, name.c_str(), query.c_str(), 0, nullptr);

    if(PQresultStatus(query_result) != PGRES_COMMAND_OK) {
        PQclear(query_result);

        std::string last_error = "Failed to prepare statement: ";
        last_error.append(PQerrorMessage(_connection));

        if(_logger != nullptr) {
            _logger->log_error(last_error);
        }

        throw sql_exception(std::move(last_error), query);
    }
}

query_result connection::exec_prepared(const std::vector<std::string>& params, const std::string& name)
{
    auto transform = [](const std::vector<std::string>& params) {
        auto** transform_params = new char*[params.size()];

        auto null = NULL_VALUE;
        for(int i = 0; i < params.size(); i++) {
            if(params[i] == null) {
                transform_params[i] = nullptr;
            } else {
                auto* target_param = new char[params[i].size()];
                for(int j = 0; j < params[i].size(); j++) {
                    target_param[j] = params[i].at(j);
                }
                transform_params[i] = target_param;
            }
        }

        return transform_params;
    };

    if(_logger != nullptr) {
        _logger->log_sql([&name, &params]() {
            std::stringstream stream;

            stream << "Execute prepare query " << name << " with params: [ ";
            for(const auto& param : params) {
                stream << param << " ";
            }
            stream << "]";

            return stream.str();
        }());
    }

    auto* query_result = PQexecPrepared(_connection, name.c_str(), params.size(), transform(params), nullptr, nullptr, 0);
    if(PQresultStatus(query_result) != PGRES_TUPLES_OK && PQresultStatus(query_result) != PGRES_COMMAND_OK) {
        PQclear(query_result);

        std::string last_error = "Failed to execute prepared statement: ";
        last_error.append(PQerrorMessage(_connection));

        if(_logger != nullptr) {
            _logger->log_error(last_error);
        }

        throw sql_exception(std::move(last_error));
    }

    const auto rows = PQntuples(query_result);
    const auto cols = PQnfields(query_result);

    query_result result;
    for(int i = 0; i < rows; i++) {
        query_result::result_row row;
        for(int j = 0; j < cols; j++) {
            auto* column_value = PQgetvalue(query_result, i, j);
            row.emplace(PQfname(query_result, j), column_value == nullptr ? "" : column_value);
        }
        result.add_row(row);
    }

    PQclear(query_result);

    return result;
}

void connection::connect(const settings& settings)
{
    const auto connection_info = [&settings]() {
        std::stringstream connection_info;
        connection_info << "dbname=" << settings.database_name << " "
                        << "user=" << settings.login << " "
                        << "password=" << settings.password << " "
                        << "host=" << settings.url << " "
                        << "port=" << settings.port;
        return connection_info.str();
    }();

    _connection = PQconnectdb(connection_info.c_str());

    if(_logger != nullptr) {
        _logger->log_sql("Connect to database with param: " + connection_info);
    }

    if(PQstatus(_connection) != CONNECTION_OK) {
        std::string last_error = "Can't open database. settings: " + connection_info + "; error: ";
        last_error.append(PQerrorMessage(_connection));

        PQfinish(_connection);
        _connection = nullptr;

        if(_logger != nullptr) {
            _logger->log_error(last_error);
        }

        throw open_database_exception(std::move(last_error));
    }
}

void connection::disconnect()
{
    if(!is_valid())
        return;

    PQfinish(_connection);
    _connection = nullptr;
}

} // namespace postgre
} // namespace database_adapter