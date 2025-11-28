#include "SqliteAdapter/sqliteconnection.h"

#include "SqliteAdapter/sqlitetransactiontype.h"

#include <DatabaseAdapter/databaseadapter.hpp>

#include <sstream>
#include <stdexcept>

namespace database_adapter {
namespace sqlite {

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
    for(const auto& prepared_pair : _prepared) {
        sqlite3_finalize(prepared_pair.second);
    }
    _prepared.clear();

    disconnect();
}

bool connection::is_valid()
{
    if(_connection == nullptr)
        return false;

    return IConnection::is_valid();
}

query_result connection::exec(const std::string& query)
{
    using namespace database_adapter;
    sqlite3_stmt* stmt;

    if(_logger != nullptr) {
        _logger->log_sql(query);
    }

    if(sqlite3_prepare_v2(_connection, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::string last_error = "Failed to prepare statement: ";
        last_error.append(sqlite3_errmsg(_connection));

        sqlite3_finalize(stmt);

        if(_logger != nullptr) {
            _logger->log_error(last_error);
        }

        throw sql_exception(std::move(last_error), query);
    }

    int rc = sqlite3_step(stmt);

    query_result result;
    while(rc == SQLITE_ROW) {
        query_result::row row;
        for(int i = 0; i < sqlite3_column_count(stmt); i++) {
            auto* column_name = sqlite3_column_name(stmt, i);
            auto* column_value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));

            row.emplace(column_name, column_value == nullptr ? "" : column_value);
        }
        result.add(row);

        rc = sqlite3_step(stmt);
    }

    if(rc != SQLITE_DONE) {
        std::string last_error = "Failed to execute statement: ";
        last_error.append(sqlite3_errmsg(_connection));

        sqlite3_finalize(stmt);

        if(_logger != nullptr) {
            _logger->log_error(last_error);
        }

        throw sql_exception(std::move(last_error), query);
    }

    sqlite3_finalize(stmt);

    return result;
}

void connection::prepare(const std::string& query, const std::string& name)
{
    if(_prepared.find(name) == _prepared.end())
        return;

    sqlite3_stmt* stmt;

    if(_logger != nullptr) {
        _logger->log_sql("Prepare query " + name + " sql: " + query);
    }

    if(sqlite3_prepare_v2(_connection, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::string last_error = "Failed to prepare statement: ";
        last_error.append(sqlite3_errmsg(_connection));

        if(_logger != nullptr) {
            _logger->log_error(last_error);
        }

        sqlite3_finalize(stmt);

        throw sql_exception(std::move(last_error), query);
    }

    _prepared.insert({ name, stmt });
}

query_result connection::exec_prepared(const std::vector<std::string>& params, const std::string& name)
{
    const auto stmt_it = _prepared.find(name);
    if(stmt_it == _prepared.end()) {
        throw sql_exception("Doesn't have prepared statment");
    }

    auto* stmt = stmt_it->second;

    sqlite3_clear_bindings(stmt);
    sqlite3_reset(stmt);

    const auto size = sqlite3_bind_parameter_count(stmt);

    if(params.size() > size) {
        throw std::invalid_argument("binding values more that binding parameters");
    }

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

    for(int i = 0; i < size; i++) {
        if(params[i] == NULL_VALUE) {
            sqlite3_bind_null(stmt, i);
            continue;
        }

        // i + 1, так как в sqlite индекс параметров начинается с 1, а не с 0
        sqlite3_bind_text(stmt, i + 1, params[i].c_str(), params[i].size(), SQLITE_STATIC);
    }

    int rc = sqlite3_step(stmt);

    query_result result;
    while(rc == SQLITE_ROW) {
        query_result::row row;
        for(int i = 0; i < sqlite3_column_count(stmt); i++) {
            auto* column_name = sqlite3_column_name(stmt, i);
            auto* column_value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));

            row.emplace(column_name, column_value == nullptr ? "" : column_value);
        }
        result.add(row);

        rc = sqlite3_step(stmt);
    }

    if(rc != SQLITE_DONE) {
        std::string last_error = "Failed to execute statement: ";
        last_error.append(sqlite3_errmsg(_connection));

        if(_logger != nullptr) {
            _logger->log_error(last_error);
        }

        throw sql_exception(std::move(last_error));
    }

    return result;
}

bool connection::open_transaction(int type)
{
    const auto sql = [&type]() {
        switch(static_cast<transaction_type>(type)) {
            case transaction_type::DEFERRED:
                return "BEGIN DEFERRED;";
            case transaction_type::IMMEDIATE:
                return "BEGIN IMMEDIATE;";
            case transaction_type::EXCLUSIVE:
                return "BEGIN EXCLUSIVE;";
            default:
                return "BEGIN;";
        }
    }();

    try {
        exec(sql);
        _has_transaction = true;
        return true;
    } catch(sql_exception&) {
        return false;
    }
}

void connection::connect(const database_connection_settings& settings)
{
    if(_logger != nullptr) {
        _logger->log_sql("Connect to database by path: " + settings.url);
    }

    if(sqlite3_open_v2(settings.url.c_str(), &_connection, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, nullptr) != SQLITE_OK) {
        std::string last_error = "Can't open database path: " + settings.url + "; ";
        last_error.append(sqlite3_errmsg(_connection));

        sqlite3_close(_connection);
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

    if(_logger != nullptr) {
        _logger->log_sql("Disconnect from database");
    }

    sqlite3_close(_connection);
    _connection = nullptr;
}

} // namespace sqlite
} // namespace database_adapter