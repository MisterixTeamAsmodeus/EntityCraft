#pragma once

#include "postgreadapter_global.h"

#include <DatabaseAdapter/iconnection.h>
#include <libpq-fe.h>

class POSTGRE_EXPORT database_adapter::IConnection;

namespace database_adapter {
namespace postgre {
/**
 * Define для настроек подключения к PostgreSQL
 */
using settings = models::database_settings;

class POSTGRE_EXPORT connection final : public IConnection
{
public:
    explicit connection(const settings& settings);
    ~connection() override;

    bool is_valid() override;
    models::query_result exec(const std::string& query) override;

    void prepare(const std::string& query, const std::string& name) override;
    models::query_result exec_prepared(const std::vector<std::string>& params, const std::string& name) override;

private:
    void connect(const settings& settings);
    void disconnect();

private:
    bool has_prepared = false;
    PGconn* _connection = nullptr;
};

} // namespace postgre
} // namespace database_adapter