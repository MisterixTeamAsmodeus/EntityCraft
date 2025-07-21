#include "PostgreAdapter/postgreconnectionpool.h"

namespace database_adapter {
namespace postgre {

settings connection_pool::connection_settings = {};

size_t connection_pool::start_pool_size = 10;
size_t connection_pool::max_pool_size = 10;
std::chrono::seconds connection_pool::wait_time = std::chrono::seconds(10);

std::shared_ptr<connection_pool> connection_pool::instance()
{
    static auto pool = std::make_shared<connection_pool>(connection_settings, start_pool_size, max_pool_size, wait_time);
    return pool;
}

connection_pool::connection_pool(models::database_settings settings, const size_t start_pool_size, const size_t max_pool_size, std::chrono::seconds wait_time)
    : database_adapter::connection_pool<connection>(std::move(settings), start_pool_size, max_pool_size, wait_time)
{
}

connection_pool::connection_pool(models::database_settings settings, const size_t start_pool_size, std::chrono::seconds wait_time)
    : database_adapter::connection_pool<connection>(std::move(settings), start_pool_size, wait_time)
{
}
} // namespace postgre
} // namespace database_adapter