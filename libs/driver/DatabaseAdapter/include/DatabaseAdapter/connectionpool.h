#pragma once

#include "DatabaseAdapter/model/databasesettings.h"

#include <chrono>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace database_adapter {

template<typename ConnectionType>
class connection_pool
{
public:
    connection_pool() = default;

    explicit connection_pool(models::database_settings settings,
        const size_t start_pool_size,
        const size_t max_pool_size,
        const std::chrono::seconds wait_time = std::chrono::seconds(10))
        : _settings(std::move(settings))
        , _start_pool_size(start_pool_size)
        , _max_pool_size(max_pool_size)
        , _wait_time(wait_time)
    {
        init_start_conncetions();
    }

    explicit connection_pool(models::database_settings settings,
        const size_t start_pool_size = 5,
        const std::chrono::seconds wait_time = std::chrono::seconds(10))
        : connection_pool(settings, start_pool_size, start_pool_size, wait_time)
    {
    }

    connection_pool(const connection_pool& other) = delete;
    connection_pool(connection_pool&& other) noexcept = delete;
    connection_pool& operator=(const connection_pool& other) = delete;
    connection_pool& operator=(connection_pool&& other) noexcept = delete;

    ~connection_pool() = default;

    void set_max_pool_size(const size_t max_pool_size)
    {
        _max_pool_size = max_pool_size;
    }

    void set_settings(const models::database_settings& settings)
    {
        _settings = settings;

        init_start_conncetions();
    }

    void set_wait_time(const std::chrono::seconds& wait_time)
    {
        _wait_time = wait_time;
    }

    std::shared_ptr<ConnectionType> open_connection()
    {
        std::lock_guard<std::mutex> lock_guard(_lock);

        for(const auto& connection : _connections) {
            if(connection.use_count() == 1) {
                return connection;
            }
        }

        if(_connections.size() < _max_pool_size) {
            auto conn = std::make_shared<ConnectionType>(_settings);
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

private:
    void init_start_conncetions()
    {
        std::lock_guard<std::mutex> lock_guard(_lock);

        _connections.clear();

        _connections.reserve(_start_pool_size);
        for(auto i = 0; i < _start_pool_size; i++) {
            _connections.push_back(std::make_shared<ConnectionType>(_settings));
        }
    }

private:
    std::mutex _lock;

    models::database_settings _settings;

    size_t _start_pool_size;
    size_t _max_pool_size;
    std::chrono::seconds _wait_time;

    std::vector<std::shared_ptr<ConnectionType>> _connections;
};

} // namespace database_adapter