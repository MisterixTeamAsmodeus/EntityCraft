#pragma once

#include "postgreadapter_global.h"

#include <DatabaseAdapter/iconnection.h>
#include <DatabaseAdapter/ilogger.h>
#include <libpq-fe.h>

#include <memory>

namespace database_adapter {
class POSTGRE_EXPORT IConnection;
class POSTGRE_EXPORT ILogger;
}

namespace database_adapter {
namespace postgre {
/**
 * Define для настроек подключения к PostgreSQL
 */
using settings = database_connection_settings;

class POSTGRE_EXPORT connection final : public IConnection
{
public:
    static void set_logger(std::shared_ptr<ILogger>&& logger);

public:
    explicit connection(const settings& settings, bool needCreateDatabaseIfNotExist = false, int retryCount = 4, int retryDeltaSeconds = 2);
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
    PGconn* _connection = nullptr;
};

} // namespace postgre
} // namespace database_adapter