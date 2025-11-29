#pragma once

#include "sqliteadapter_global.hpp"
#include "sqliteconnection.hpp"

#include <DatabaseAdapter/connectionpool.hpp>

namespace database_adapter {
namespace sqlite {

class SQLITE_EXPORT connection_pool final : public IConnectionPool
{
public:
    static database_connection_settings connection_settings;

    static size_t start_pool_size;
    static size_t max_pool_size;
    static std::chrono::seconds wait_time;
    static std::chrono::seconds idle_timeout;

    static std::shared_ptr<connection_pool> instance();

public:
    explicit connection_pool(database_connection_settings settings,
        size_t start_pool_size = 2,
        size_t max_pool_size = 10,
        std::chrono::seconds wait_time = std::chrono::seconds(2),
        std::chrono::seconds idle_timeout = std::chrono::seconds(300));

protected:
    std::shared_ptr<IConnection> create_connection(const database_connection_settings& settings) override;
};

} // namespace sqlite
} // namespace database_adapter