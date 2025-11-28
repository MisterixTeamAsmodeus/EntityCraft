#include "DatabaseAdapter/connectionpool.hpp"

#include <algorithm>
#include <thread>

namespace database_adapter {
IConnectionPool::IConnectionPool(database_connection_settings settings, const size_t start_pool_size, const size_t max_pool_size, const std::chrono::seconds wait_time, const std::chrono::seconds idle_timeout)
    : _settings(std::move(settings))
    , _start_pool_size(start_pool_size)
    , _max_pool_size(max_pool_size)
    , _wait_time(wait_time)
    , _idle_timeout(idle_timeout)
{
}

IConnectionPool::IConnectionPool(database_connection_settings settings, const size_t start_pool_size, const std::chrono::seconds wait_time, const std::chrono::seconds idle_timeout)
    : IConnectionPool(std::move(settings), start_pool_size, start_pool_size, wait_time, idle_timeout)
{
}

void IConnectionPool::set_max_pool_size(const size_t max_pool_size)
{
    _max_pool_size = max_pool_size;
}

void IConnectionPool::set_settings(const database_connection_settings& settings)
{
    std::lock_guard<std::mutex> lock(_lock);
    _settings = settings;
    init_start_connections();
    _connection_available.notify_all();
}

void IConnectionPool::set_wait_time(const std::chrono::seconds& wait_time)
{
    _wait_time = wait_time;
}

void IConnectionPool::set_idle_timeout(const std::chrono::seconds& idle_timeout)
{
    _idle_timeout = idle_timeout;
}

std::shared_ptr<IConnection> IConnectionPool::open_connection()
{
    std::unique_lock<std::mutex> lock(_lock);

    // Удаляем невалидные соединения и откатываем транзакции
    _connections.erase(std::remove_if(_connections.begin(), _connections.end(), [this](auto& entry) {
        auto conn = entry.connection();
        if(conn.use_count() != 2) {
            return false;
        }

        // Проверяем время простоя
        const auto idle_duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - entry.last_used());
        if(idle_duration >= _idle_timeout) {
            // Закрываем соединение, если оно не использовалось слишком долго
            return true;
        }

        if(!conn->is_valid()) {
            return true;
        }

        if(conn->is_transaction()) {
            conn->rollback();
        }

        return false;
    }),
        _connections.end());

    if(_connections.empty()) {
        init_start_connections();
    }

    const auto deadline = std::chrono::system_clock::now() + _wait_time;

    while(true) {
        for(auto& entry : _connections) {
            auto conn = entry.connection();
            if(conn.use_count() == 2) {
                // Обновляем время последнего использования при выдаче соединения
                entry.update_last_used();
                return conn;
            }
        }

        if(_connections.size() < _max_pool_size) {
            auto conn = create_connection(_settings);
            _connections.emplace_back(conn);
            return conn;
        }

        if(_connection_available.wait_until(lock, deadline) == std::cv_status::timeout) {
            break;
        }
    }

    return nullptr;
}

void IConnectionPool::init_start_connections()
{
    _connections.clear();

    _connections.reserve(_start_pool_size);
    for(auto i = 0; i < _start_pool_size; i++) {
        _connections.emplace_back(create_connection(_settings));
    }
}

} // namespace database_adapter