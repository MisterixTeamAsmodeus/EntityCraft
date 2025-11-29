#pragma once
#define SQLITE_API // Гарантирует статическую линковку для sqlite3

#include "sqliteadapter_global.hpp"

#include <DatabaseAdapter/iconnection.hpp>
#include <DatabaseAdapter/ilogger.hpp>
#include <DatabaseAdapter/transaction_isolation.hpp>
#include <sqlite3.h>

#include <memory>
#include <mutex>
#include <unordered_map>

namespace database_adapter {
class SQLITE_EXPORT IConnection;
class SQLITE_EXPORT ILogger;
}

namespace database_adapter {
namespace sqlite {
/**
 * Define для настроек подключения к Sqlite
 * @param url Путь до файла базы данных
 */
using settings = database_connection_settings;

class SQLITE_EXPORT connection final : public IConnection
{
public:
    static void set_logger(std::shared_ptr<ILogger>&& logger);

public:
    explicit connection(const settings& settings);
    ~connection() override;

    bool is_valid() override;
    query_result exec(const std::string& query) override;

    void prepare(const std::string& query, const std::string& name) override;
    query_result exec_prepared(const std::vector<std::string>& params, const std::string& name) override;

    bool open_transaction(transaction_isolation_level level) override;

private:
    void connect(const settings& settings);
    void disconnect();

private:
    static std::shared_ptr<ILogger> _logger;

private:
    /**
     * @brief Извлекает значение столбца из результата запроса с учетом типа данных SQLite
     * @param stmt Подготовленное выражение SQLite
     * @param column_index Индекс столбца
     * @return Строковое представление значения столбца
     */
    std::string extract_column_value(sqlite3_stmt* stmt, int column_index) const;

    /**
     * @brief Привязывает параметр к подготовленному запросу с автоматическим определением типа
     * @param stmt Подготовленное выражение SQLite
     * @param bind_index Индекс параметра (начинается с 1)
     * @param param Значение параметра в виде строки
     * @throws sql_exception В случае ошибки привязки параметра
     */
    void bind_parameter(sqlite3_stmt* stmt, int bind_index, const std::string& param) const;

private:
    sqlite3* _connection = nullptr;

    /// @brief Мьютекс для защиты операций с подготовленными запросами
    mutable std::mutex _prepared_mutex;
    std::unordered_map<std::string, sqlite3_stmt*> _prepared;
};
} // namespace sqlite
} // namespace database_adapter
