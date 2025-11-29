#include "SqliteAdapter/sqliteconnection.hpp"
#include "SqliteAdapter/sqliteerrorcode.hpp"

#include <DatabaseAdapter/databaseadapter.hpp>

#include <cctype>
#include <sstream>
#include <stdexcept>
#include <mutex>

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
    std::lock_guard<std::mutex> lock(_prepared_mutex);
    for(const auto& prepared_pair : _prepared) {
        if(prepared_pair.second != nullptr) {
            const int rc = sqlite3_finalize(prepared_pair.second);
            const sqlite_error_code error_code = to_sqlite_error_code(rc);
            if(error_code != sqlite_error_code::OK && _logger != nullptr) {
                _logger->log_error("Error finalizing prepared statement: " + std::string(sqlite3_errmsg(_connection)));
            }
        }
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
    
    validate_query(query);
    
    sqlite3_stmt* stmt = nullptr;

    if(_logger != nullptr) {
        _logger->log_sql(query);
    }

    const int prepare_rc = sqlite3_prepare_v2(_connection, query.c_str(), -1, &stmt, nullptr);
    const sqlite_error_code prepare_error_code = to_sqlite_error_code(prepare_rc);
    if(prepare_error_code != sqlite_error_code::OK) {
        std::string last_error = "Failed to prepare statement: ";
        last_error.append(sqlite3_errmsg(_connection));
        const int error_code = sqlite3_extended_errcode(_connection);

        if(stmt != nullptr) {
            sqlite3_finalize(stmt);
        }

        if(_logger != nullptr) {
            _logger->log_error(last_error);
        }

        throw sql_exception(std::move(last_error), query, error_code);
    }

    int rc = sqlite3_step(stmt);
    sqlite_error_code step_error_code = to_sqlite_error_code(rc);

    query_result result;
    while(step_error_code == sqlite_error_code::ROW) {
        query_result::row row;
        const int column_count = sqlite3_column_count(stmt);
        for(int i = 0; i < column_count; i++) {
            const char* column_name = sqlite3_column_name(stmt, i);
            const std::string column_value = extract_column_value(stmt, i);
            row.emplace(column_name, column_value);
        }
        result.add(row);

        rc = sqlite3_step(stmt);
        step_error_code = to_sqlite_error_code(rc);
    }

    if(step_error_code != sqlite_error_code::DONE && step_error_code != sqlite_error_code::ROW) {
        std::string last_error = "Failed to execute statement: ";
        last_error.append(sqlite3_errmsg(_connection));
        const int error_code = sqlite3_extended_errcode(_connection);

        sqlite3_finalize(stmt);

        if(_logger != nullptr) {
            _logger->log_error(last_error);
        }

        throw sql_exception(std::move(last_error), query, error_code);
    }

    sqlite3_finalize(stmt);

    return result;
}

void connection::prepare(const std::string& query, const std::string& name)
{
    validate_query(query);
    
    std::lock_guard<std::mutex> lock(_prepared_mutex);
    
    // Если запрос уже существует, освобождаем старый
    const auto existing_it = _prepared.find(name);
    if(existing_it != _prepared.end()) {
        if(existing_it->second != nullptr) {
            const int rc = sqlite3_finalize(existing_it->second);
            const sqlite_error_code error_code = to_sqlite_error_code(rc);
            if(error_code != sqlite_error_code::OK && _logger != nullptr) {
                _logger->log_error("Error finalizing existing prepared statement: " + std::string(sqlite3_errmsg(_connection)));
            }
        }
        _prepared.erase(existing_it);
    }

    sqlite3_stmt* stmt = nullptr;

    if(_logger != nullptr) {
        _logger->log_sql("Prepare query " + name + " sql: " + query);
    }

    const int prepare_rc = sqlite3_prepare_v2(_connection, query.c_str(), -1, &stmt, nullptr);
    const sqlite_error_code prepare_error_code = to_sqlite_error_code(prepare_rc);
    if(prepare_error_code != sqlite_error_code::OK) {
        std::string last_error = "Failed to prepare statement: ";
        last_error.append(sqlite3_errmsg(_connection));
        const int error_code = sqlite3_extended_errcode(_connection);

        if(stmt != nullptr) {
            sqlite3_finalize(stmt);
        }

        if(_logger != nullptr) {
            _logger->log_error(last_error);
        }

        throw sql_exception(std::move(last_error), query, error_code);
    }

    _prepared.insert({ name, stmt });
}

query_result connection::exec_prepared(const std::vector<std::string>& params, const std::string& name)
{
    std::lock_guard<std::mutex> lock(_prepared_mutex);
    
    const auto stmt_it = _prepared.find(name);
    if(stmt_it == _prepared.end()) {
        throw sql_exception("Doesn't have prepared statement: " + name);
    }

    auto* stmt = stmt_it->second;
    if(stmt == nullptr) {
        throw sql_exception("Prepared statement is null: " + name);
    }

    sqlite3_clear_bindings(stmt);
    const int reset_rc = sqlite3_reset(stmt);
    const sqlite_error_code reset_error_code = to_sqlite_error_code(reset_rc);
    if(reset_error_code != sqlite_error_code::OK && _logger != nullptr) {
        _logger->log_error("Error resetting prepared statement: " + std::string(sqlite3_errmsg(_connection)));
    }

    const int size = sqlite3_bind_parameter_count(stmt);

    if(static_cast<int>(params.size()) > size) {
        throw std::invalid_argument("binding values more than binding parameters");
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

    for(int i = 0; i < static_cast<int>(params.size()); i++) {
        const int bind_index = i + 1; // В sqlite индекс параметров начинается с 1, а не с 0
        bind_parameter(stmt, bind_index, params[i]);
    }

    int rc = sqlite3_step(stmt);
    sqlite_error_code step_error_code = to_sqlite_error_code(rc);

    query_result result;
    while(step_error_code == sqlite_error_code::ROW) {
        query_result::row row;
        const int column_count = sqlite3_column_count(stmt);
        for(int i = 0; i < column_count; i++) {
            const char* column_name = sqlite3_column_name(stmt, i);
            const std::string column_value = extract_column_value(stmt, i);
            row.emplace(column_name, column_value);
        }
        result.add(row);

        rc = sqlite3_step(stmt);
        step_error_code = to_sqlite_error_code(rc);
    }

    if(step_error_code != sqlite_error_code::DONE && step_error_code != sqlite_error_code::ROW) {
        std::string last_error = "Failed to execute prepared statement: ";
        last_error.append(sqlite3_errmsg(_connection));
        const int error_code = sqlite3_extended_errcode(_connection);

        if(_logger != nullptr) {
            _logger->log_error(last_error);
        }

        throw sql_exception(std::move(last_error), "", error_code);
    }

    return result;
}

bool connection::open_transaction(transaction_isolation_level level)
{
    if(_has_transaction) {
        throw sql_exception("Транзакция уже открыта", "BEGIN");
    }

    using namespace database_adapter;
    
    std::string sql;
    
    // SQLite поддерживает только DEFERRED, IMMEDIATE и EXCLUSIVE
    // Маппинг уровней изоляции на типы транзакций SQLite
    switch(level) {
        case transaction_isolation_level::READ_UNCOMMITTED:
        case transaction_isolation_level::READ_COMMITTED:
            // SQLite не поддерживает READ UNCOMMITTED и READ COMMITTED напрямую
            // Используем DEFERRED как наиболее близкий аналог
            sql = "BEGIN DEFERRED;";
            break;
        case transaction_isolation_level::REPEATABLE_READ:
            // Используем IMMEDIATE для REPEATABLE READ
            sql = "BEGIN IMMEDIATE;";
            break;
        case transaction_isolation_level::SERIALIZABLE:
            // SERIALIZABLE соответствует EXCLUSIVE в SQLite
            sql = "BEGIN EXCLUSIVE;";
            break;
        default:
            sql = "BEGIN;";
            break;
    }

    try {
        exec(sql);
        std::lock_guard<std::mutex> lock(_mutex);
        _has_transaction = true;
        return true;
    } catch(const sql_exception&) {
        return false;
    }
}

void connection::connect(const database_connection_settings& settings)
{
    if(_logger != nullptr) {
        _logger->log_sql("Connect to database by path: " + settings.url);
    }

    const int open_rc = sqlite3_open_v2(settings.url.c_str(), &_connection, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, nullptr);
    const sqlite_error_code open_error_code = to_sqlite_error_code(open_rc);
    if(open_error_code != sqlite_error_code::OK) {
        std::string last_error = "Can't open database path: " + settings.url + "; ";
        last_error.append(sqlite3_errmsg(_connection));
        const int error_code = (_connection != nullptr) ? sqlite3_extended_errcode(_connection) : open_rc;

        sqlite3_close(_connection);
        _connection = nullptr;

        if(_logger != nullptr) {
            _logger->log_error(last_error);
        }

        throw open_database_exception(std::move(last_error), error_code);
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

void connection::bind_parameter(sqlite3_stmt* stmt, int bind_index, const std::string& param) const
{
    if(stmt == nullptr) {
        throw sql_exception("Statement is null");
    }

    // Обработка NULL значений
    if(param == NULL_VALUE) {
        const int bind_rc = sqlite3_bind_null(stmt, bind_index);
        const sqlite_error_code bind_error_code = to_sqlite_error_code(bind_rc);
        if(bind_error_code != sqlite_error_code::OK) {
            std::string error_msg = "Error binding NULL parameter at index " + std::to_string(bind_index) + ": " + std::string(sqlite3_errmsg(_connection));
            const int error_code = sqlite3_extended_errcode(_connection);
            if(_logger != nullptr) {
                _logger->log_error(error_msg);
            }
            throw sql_exception(std::move(error_msg), "", error_code);
        }
        return;
    }

    // Попытка определить тип параметра по содержимому
    // SQLite автоматически преобразует типы, но использование правильного типа улучшает производительность
    
    // Проверка на целое число (может быть со знаком)
    if(!param.empty()) {
        bool is_integer = true;
        bool is_float = false;
        size_t start_pos = 0;
        
        // Проверка на знак
        if(param[0] == '-' || param[0] == '+') {
            start_pos = 1;
        }
        
        // Проверка на наличие точки или экспоненты (признак вещественного числа)
        if(param.find('.') != std::string::npos || 
           param.find('e') != std::string::npos || 
           param.find('E') != std::string::npos) {
            is_float = true;
            is_integer = false;
        }
        
        // Проверка всех символов
        for(size_t i = start_pos; i < param.size(); i++) {
            if(!std::isdigit(static_cast<unsigned char>(param[i]))) {
                if(param[i] == '.' && is_float) {
                    continue; // Точка допустима для вещественных чисел
                }
                if((param[i] == 'e' || param[i] == 'E') && is_float) {
                    continue; // Экспонента допустима
                }
                if((param[i] == '-' || param[i] == '+') && i > start_pos && (param[i-1] == 'e' || param[i-1] == 'E')) {
                    continue; // Знак после экспоненты допустим
                }
                is_integer = false;
                is_float = false;
                break;
            }
        }
        
        // Привязка как INTEGER
        if(is_integer && !param.empty()) {
            try {
                const sqlite3_int64 int_value = std::stoll(param);
                const int bind_rc = sqlite3_bind_int64(stmt, bind_index, int_value);
                const sqlite_error_code bind_error_code = to_sqlite_error_code(bind_rc);
                if(bind_error_code != sqlite_error_code::OK) {
                    // Если не удалось привязать как INTEGER, пробуем как TEXT
                    // (SQLite автоматически преобразует)
                    const int text_bind_rc = sqlite3_bind_text(stmt, bind_index, param.c_str(), static_cast<int>(param.size()), SQLITE_TRANSIENT);
                    const sqlite_error_code text_bind_error_code = to_sqlite_error_code(text_bind_rc);
                    if(text_bind_error_code != sqlite_error_code::OK) {
                        std::string error_msg = "Error binding parameter at index " + std::to_string(bind_index) + ": " + std::string(sqlite3_errmsg(_connection));
                        const int error_code = sqlite3_extended_errcode(_connection);
                        if(_logger != nullptr) {
                            _logger->log_error(error_msg);
                        }
                        throw sql_exception(std::move(error_msg), "", error_code);
                    }
                }
                return;
            } catch(const std::exception&) {
                // Если преобразование не удалось, используем TEXT
            }
        }
        
        // Привязка как REAL
        if(is_float && !param.empty()) {
            try {
                const double float_value = std::stod(param);
                const int bind_rc = sqlite3_bind_double(stmt, bind_index, float_value);
                const sqlite_error_code bind_error_code = to_sqlite_error_code(bind_rc);
                if(bind_error_code != sqlite_error_code::OK) {
                    // Если не удалось привязать как REAL, пробуем как TEXT
                    const int text_bind_rc = sqlite3_bind_text(stmt, bind_index, param.c_str(), static_cast<int>(param.size()), SQLITE_TRANSIENT);
                    const sqlite_error_code text_bind_error_code = to_sqlite_error_code(text_bind_rc);
                    if(text_bind_error_code != sqlite_error_code::OK) {
                        std::string error_msg = "Error binding parameter at index " + std::to_string(bind_index) + ": " + std::string(sqlite3_errmsg(_connection));
                        const int error_code = sqlite3_extended_errcode(_connection);
                        if(_logger != nullptr) {
                            _logger->log_error(error_msg);
                        }
                        throw sql_exception(std::move(error_msg), "", error_code);
                    }
                }
                return;
            } catch(const std::exception&) {
                // Если преобразование не удалось, используем TEXT
            }
        }
    }
    
    // По умолчанию привязываем как TEXT
    // SQLite автоматически преобразует типы при необходимости
    // При использовании prepared statements экранирование не требуется - SQLite обрабатывает это безопасно
    const int bind_rc = sqlite3_bind_text(stmt, bind_index, param.c_str(), static_cast<int>(param.size()), SQLITE_TRANSIENT);
    const sqlite_error_code bind_error_code = to_sqlite_error_code(bind_rc);
    if(bind_error_code != sqlite_error_code::OK) {
        std::string error_msg = "Error binding text parameter at index " + std::to_string(bind_index) + ": " + std::string(sqlite3_errmsg(_connection));
        const int error_code = sqlite3_extended_errcode(_connection);
        if(_logger != nullptr) {
            _logger->log_error(error_msg);
        }
        throw sql_exception(std::move(error_msg), "", error_code);
    }
}

std::string connection::extract_column_value(sqlite3_stmt* stmt, int column_index) const
{
    if(stmt == nullptr) {
        return "";
    }

    const int column_type = sqlite3_column_type(stmt, column_index);
    
    switch(column_type) {
        case SQLITE_NULL:
            return NULL_VALUE;
            
        case SQLITE_INTEGER: {
            const sqlite3_int64 int_value = sqlite3_column_int64(stmt, column_index);
            return std::to_string(int_value);
        }
        
        case SQLITE_FLOAT: {
            const double float_value = sqlite3_column_double(stmt, column_index);
            return std::to_string(float_value);
        }
        
        case SQLITE_TEXT: {
            const unsigned char* text_value = sqlite3_column_text(stmt, column_index);
            if(text_value == nullptr) {
                return NULL_VALUE;
            }
            return reinterpret_cast<const char*>(text_value);
        }
        
        case SQLITE_BLOB: {
            // Для BLOB данных возвращаем сырые байты
            const void* blob_data = sqlite3_column_blob(stmt, column_index);
            const int blob_size = sqlite3_column_bytes(stmt, column_index);
            
            if(blob_data == nullptr || blob_size == 0) {
                return "";
            }
            
            // Преобразуем байты в строку
            return std::string(static_cast<const char*>(blob_data), static_cast<size_t>(blob_size));
        }
        
        default:
            return "";
    }
}

} // namespace sqlite
} // namespace database_adapter