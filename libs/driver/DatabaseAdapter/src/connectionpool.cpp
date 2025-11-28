#include "DatabaseAdapter/connectionpool.h"

#include <algorithm>
#include <thread>

namespace database_adapter {
IConnectionPool::IConnectionPool(database_connection_settings settings, const size_t start_pool_size, const size_t max_pool_size, const std::chrono::seconds wait_time)
    : _settings(std::move(settings))
    , _start_pool_size(start_pool_size)
    , _max_pool_size(max_pool_size)
    , _wait_time(wait_time)
{
}

IConnectionPool::IConnectionPool(database_connection_settings settings, const size_t start_pool_size, const std::chrono::seconds wait_time)
    : IConnectionPool(std::move(settings), start_pool_size, start_pool_size, wait_time)
{
}

void IConnectionPool::set_max_pool_size(const size_t max_pool_size)
{
    _max_pool_size = max_pool_size;
}

void IConnectionPool::set_settings(const database_connection_settings& settings)
{
    _settings = settings;

    init_start_conncetions();
}

void IConnectionPool::set_wait_time(const std::chrono::seconds& wait_time)
{
    _wait_time = wait_time;
}

std::shared_ptr<IConnection> IConnectionPool::open_connection()
{
    std::lock_guard<std::mutex> lock_guard(_lock);

    _connections.erase(std::remove_if(_connections.begin(), _connections.end(), [](const auto& connection) {
        if(connection.use_count() != 1) {
            return false;
        }

        if(!connection->is_valid()) {
            return true;
        }

        if(connection->is_transaction()) {
            connection->rollback();
        }

        return false;
    }),
        _connections.end());

    if(_connections.empty()) {
        init_start_conncetions();
    }

    for(const auto& connection : _connections) {
        if(connection.use_count() == 1) {
            return connection;
        }
    }

    if(_connections.size() < _max_pool_size) {
        auto conn = create_connection(_settings);
        _connections.push_back(conn);
        return conn;
    }

    const auto start = std::chrono::system_clock::now();

    while(std::chrono::system_clock::now() - start <= _wait_time) {
        for(auto& connection : _connections) {
            if(connection.use_count() == 1) {
                return connection;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return nullptr;
}

void IConnectionPool::init_start_conncetions()
{
    _connections.clear();

    _connections.reserve(_start_pool_size);
    for(auto i = 0; i < _start_pool_size; i++) {
        _connections.push_back(create_connection(_settings));
    }
}
} // namespace database_adapter