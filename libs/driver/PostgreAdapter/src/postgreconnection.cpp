#include "PostgreAdapter/postgreconnection.hpp"
#include "PostgreAdapter/postgreerrorcode.hpp"

#include "DatabaseAdapter/databaseadapter.hpp"
#include "DatabaseAdapter/exception/opendatabaseexception.hpp"
#include "DatabaseAdapter/exception/sqlexception.hpp"
#include "DatabaseAdapter/transaction_isolation.hpp"

#include <sstream>
#include <thread>
#include <vector>

namespace database_adapter {
namespace postgre {

std::shared_ptr<ILogger> connection::_logger = nullptr;

void connection::set_logger(std::shared_ptr<ILogger>&& logger)
{
    _logger = std::move(logger);
}

connection::connection(const settings& settings, const bool needCreateDatabaseIfNotExist, const int retryCount, const int retryDeltaSeconds)
    : IConnection(settings)
{
    for(int i = 0; i < retryCount; i++) {
        try {
            connect(settings);
            if(is_valid()) {
                return;
            }
        } catch(const open_database_exception& e) {
            if(_logger != nullptr) {
                _logger->log_error("Connection attempt " + std::to_string(i + 1) + " failed: " + e.what());
            }
        } catch(const sql_exception& e) {
            if(_logger != nullptr) {
                _logger->log_error("Connection attempt " + std::to_string(i + 1) + " failed: " + e.what());
            }
        }

        if(i < retryCount - 1) {
            std::this_thread::sleep_for(std::chrono::seconds(retryDeltaSeconds));
        }
    }

    if(!needCreateDatabaseIfNotExist) {
        std::string last_error = "Failed to connect to database after " + std::to_string(retryCount) + " attempts";
        if(_logger != nullptr) {
            _logger->log_error(last_error);
        }
        throw open_database_exception(std::move(last_error));
    }

    auto temp_database_settings = settings;
    temp_database_settings.database_name = "postgres";
    connect(temp_database_settings);

    exec("CREATE DATABASE \"" + settings.database_name + "\"");

    disconnect();
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

    return IConnection::is_valid();
}

query_result connection::exec(const std::string& query)
{
    validate_query(query);

    if(_logger != nullptr) {
        _logger->log_sql(query);
    }

    auto* query_result = PQexec(_connection, query.c_str());
    const auto status = to_postgre_exec_status(PQresultStatus(query_result));
    
    if(!is_success(status)) {
        std::string last_error = "Failed to execute statement: ";
        last_error.append(PQerrorMessage(_connection));
        
        const char* error_code_str = PQresultErrorField(query_result, PG_DIAG_SQLSTATE);
        int error_code = 0;
        if(error_code_str != nullptr) {
            // SQLSTATE - это 5-символьная строка, конвертируем в число
            try {
                error_code = std::stoi(std::string(error_code_str));
            } catch(const std::exception&) {
                // Если не удалось преобразовать, оставляем 0
            }
        }

        PQclear(query_result);

        if(_logger != nullptr) {
            _logger->log_error(last_error);
        }

        throw sql_exception(std::move(last_error), query, error_code);
    }

    const auto rows = PQntuples(query_result);
    const auto cols = PQnfields(query_result);

    database_adapter::query_result result;
    for(int i = 0; i < rows; i++) {
        query_result::row row;
        for(int j = 0; j < cols; j++) {
            auto* column_value = PQgetvalue(query_result, i, j);
            row.emplace(PQfname(query_result, j), column_value == nullptr ? "" : column_value);
        }
        result.add(row);
    }

    PQclear(query_result);

    return result;
}

void connection::prepare(const std::string& query, const std::string& name)
{
    validate_query(query);

    std::lock_guard<std::mutex> lock(_prepared_mutex);

    if(_logger != nullptr) {
        _logger->log_sql("Prepare query " + name + " sql: " + query);
    }

    if(_prepared_statements.find(name) != _prepared_statements.end()) {
        throw sql_exception("Prepared statement already exists: " + name, query);
    }

    auto* query_result = PQprepare(_connection, name.c_str(), query.c_str(), 0, nullptr);
    const auto status = to_postgre_exec_status(PQresultStatus(query_result));

    if(status != postgre_exec_status::COMMAND_OK) {
        std::string last_error = "Failed to prepare statement: ";
        last_error.append(PQerrorMessage(_connection));
        
        const char* error_code_str = PQresultErrorField(query_result, PG_DIAG_SQLSTATE);
        int error_code = 0;
        if(error_code_str != nullptr) {
            // SQLSTATE - это 5-символьная строка, конвертируем в число
            try {
                error_code = std::stoi(std::string(error_code_str));
            } catch(const std::exception&) {
                // Если не удалось преобразовать, оставляем 0
            }
        }

        PQclear(query_result);

        if(_logger != nullptr) {
            _logger->log_error(last_error);
        }

        throw sql_exception(std::move(last_error), query, error_code);
    }

    _prepared_statements.insert(name);
    PQclear(query_result);
}

query_result connection::exec_prepared(const std::vector<std::string>& params, const std::string& name)
{
    std::lock_guard<std::mutex> lock(_prepared_mutex);

    // Проверяем, был ли запрос подготовлен
    if(_prepared_statements.find(name) == _prepared_statements.end()) {
        std::string last_error = "Prepared statement '" + name + "' was not prepared. Call prepare() first.";
        if(_logger != nullptr) {
            _logger->log_error(last_error);
        }
        throw sql_exception(std::move(last_error), "");
    }

    // Используем std::vector для безопасного управления памятью
    std::vector<std::string> param_storage;
    std::vector<const char*> param_ptrs;
    
    param_storage.reserve(params.size());
    param_ptrs.reserve(params.size());

    for(const auto& param : params) {
        if(param == NULL_VALUE) {
            param_ptrs.push_back(nullptr);
        } else {
            param_storage.push_back(param);
            param_ptrs.push_back(param_storage.back().c_str());
        }
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

    auto* query_result = PQexecPrepared(_connection, name.c_str(), static_cast<int>(params.size()), 
                                        param_ptrs.data(), nullptr, nullptr, 0);
    const auto status = to_postgre_exec_status(PQresultStatus(query_result));
    
    if(!is_success(status)) {
        std::string last_error = "Failed to execute prepared statement: ";
        last_error.append(PQerrorMessage(_connection));
        
        const char* error_code_str = PQresultErrorField(query_result, PG_DIAG_SQLSTATE);
        int error_code = 0;
        if(error_code_str != nullptr) {
            // SQLSTATE - это 5-символьная строка, конвертируем в число
            try {
                error_code = std::stoi(std::string(error_code_str));
            } catch(const std::exception&) {
                // Если не удалось преобразовать, оставляем 0
            }
        }

        PQclear(query_result);

        if(_logger != nullptr) {
            _logger->log_error(last_error);
        }

        throw sql_exception(std::move(last_error), "", error_code);
    }

    const auto rows = PQntuples(query_result);
    const auto cols = PQnfields(query_result);

    database_adapter::query_result result;
    for(int i = 0; i < rows; i++) {
        query_result::row row;
        for(int j = 0; j < cols; j++) {
            auto* column_value = PQgetvalue(query_result, i, j);
            row.emplace(PQfname(query_result, j), column_value == nullptr ? "" : column_value);
        }
        result.add(row);
    }

    PQclear(query_result);

    return result;
}

bool connection::open_transaction(transaction_isolation_level level)
{
    const auto sql = [&level]() -> std::string {
        switch(level) {
            case transaction_isolation_level::READ_UNCOMMITTED:
                return "BEGIN TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;";
            case transaction_isolation_level::READ_COMMITTED:
                return "BEGIN TRANSACTION ISOLATION LEVEL READ COMMITTED;";
            case transaction_isolation_level::REPEATABLE_READ:
                return "BEGIN TRANSACTION ISOLATION LEVEL REPEATABLE READ;";
            case transaction_isolation_level::SERIALIZABLE:
                return "BEGIN TRANSACTION ISOLATION LEVEL SERIALIZABLE;";
            default:
                return "BEGIN;";
        }
    }();

    try {
        exec(sql);
        std::lock_guard<std::mutex> lock(_mutex);
        _has_transaction = true;
        return true;
    } catch(sql_exception&) {
        return false;
    }
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

    const auto conn_status = to_postgre_conn_status(PQstatus(_connection));
    if(!is_connection_ok(conn_status)) {
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

    cleanup_prepared_statements();

    if(_logger != nullptr) {
        _logger->log_sql("Disconnect from database");
    }

    PQfinish(_connection);
    _connection = nullptr;
}

void connection::cleanup_prepared_statements()
{
    std::lock_guard<std::mutex> lock(_prepared_mutex);
    
    if(_prepared_statements.empty() || !is_valid() || _connection == nullptr) {
        return;
    }

    try {
        // Очищаем все подготовленные запросы одной командой
        exec("DEALLOCATE ALL");
        _prepared_statements.clear();
    } catch(const std::exception& e) {
        if(_logger != nullptr) {
            _logger->log_error("Error cleaning up prepared statements: " + std::string(e.what()));
        }
    }
}

} // namespace postgre
} // namespace database_adapter