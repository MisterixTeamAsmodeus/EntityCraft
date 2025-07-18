#pragma once

#include "sqliteadapter_global.h"

#include <DatabaseAdapter/iconnection.h>
#include <sqlite3.h>

class SQLITE_EXPORT database_adapter::IConnection;

namespace database_adapter {
namespace sqlite {
/**
 * Define для настроек подключения к Sqlite
 * @param url Путь до файла базы данных
 */
using settings = models::database_settings;

class SQLITE_EXPORT connection final : public IConnection
{
public:
    explicit connection(const settings& settings);
    ~connection() override;

    bool is_valid() override;
    models::query_result exec(const std::string& query) override;

    void prepare(const std::string& query, const std::string& name = "") override;
    models::query_result exec_prepared(const std::vector<std::string>& params, const std::string& name = "") override;

private:
    void connect(const settings& settings);
    void disconnect();

private:
    sqlite3* _connection = nullptr;

    sqlite3_stmt* prepared = nullptr;
};
} // namespace sqlite
} // namespace database_adapter