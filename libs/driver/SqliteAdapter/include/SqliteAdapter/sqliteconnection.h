#pragma once
#define SQLITE_API // Гарантирует статическую линковку для sqlite3

#include "sqliteadapter_global.h"

#include <DatabaseAdapter/iconnection.h>
#include <DatabaseAdapter/ilogger.h>
#include <sqlite3.h>

#include <memory>

class SQLITE_EXPORT database_adapter::IConnection;
class SQLITE_EXPORT database_adapter::ILogger;

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

    bool open_transaction(int type) override;

private:
    void connect(const settings& settings);
    void disconnect();

private:
    static std::shared_ptr<ILogger> _logger;

private:
    sqlite3* _connection = nullptr;

    std::unordered_map<std::string, sqlite3_stmt*> _prepared;
};
} // namespace sqlite
} // namespace database_adapter