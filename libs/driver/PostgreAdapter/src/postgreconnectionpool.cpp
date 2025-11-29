#include "PostgreAdapter/postgreconnectionpool.hpp"

namespace database_adapter {
namespace postgre {

database_connection_settings connection_pool::connection_settings {};

size_t connection_pool::start_pool_size = 2;
size_t connection_pool::max_pool_size = 10;
std::chrono::seconds connection_pool::wait_time = std::chrono::seconds(2);

std::shared_ptr<connection_pool> connection_pool::instance()
{
    static auto pool = std::make_shared<connection_pool>(connection_settings, start_pool_size, max_pool_size, wait_time);
    return pool;
}

connection_pool::connection_pool(database_connection_settings settings, const size_t start_pool_size, const size_t max_pool_size, std::chrono::seconds wait_time)
    : IConnectionPool(std::move(settings), start_pool_size, max_pool_size, wait_time)
{
}

std::shared_ptr<IConnection> connection_pool::create_connection(const database_connection_settings& settings)
{
    return std::make_shared<connection>(settings);
}

} // namespace postgre
} // namespace database_adapter