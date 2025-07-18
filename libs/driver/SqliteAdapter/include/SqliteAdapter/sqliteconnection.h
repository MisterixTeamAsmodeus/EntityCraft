#pragma once

#include <DatabaseAdapter/iconnection.h>
#include <sqlite3.h>

namespace database_adapter {

/**
 * Define для настроек подключения к Sqlite
 * @param url Путь до файла базы данных
 */
using sqlite_settings = models::database_settings;

class sqlite_connection final : public IConnection
{
public:
    explicit sqlite_connection(const sqlite_settings& settings);
    ~sqlite_connection() override;

    bool is_valid() override;
    models::query_result exec(const std::string& query) override;

    void prepare(const std::string& query) override;
    models::query_result exec_prepared(const std::vector<std::string>& params) override;

private:
    void connect(const sqlite_settings& settings);
    void disconnect() const;

private:
    sqlite3* _connection = nullptr;

    sqlite3_stmt* prepared = nullptr;
};

} // namespace database_adapter