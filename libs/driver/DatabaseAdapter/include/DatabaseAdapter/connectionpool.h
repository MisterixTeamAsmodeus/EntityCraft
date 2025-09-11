#pragma once

#include "DatabaseAdapter/model/databasesettings.h"
#include "iconnection.h"

#include <chrono>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace database_adapter {

class IConnectionPool
{
public:
    IConnectionPool() = default;

    explicit IConnectionPool(database_connection_settings settings,
        const size_t start_pool_size,
        const size_t max_pool_size,
        const std::chrono::seconds wait_time = std::chrono::seconds(10))
        : _settings(std::move(settings))
        , _start_pool_size(start_pool_size)
        , _max_pool_size(max_pool_size)
        , _wait_time(wait_time)
    {
    }

    explicit IConnectionPool(database_connection_settings settings,
        const size_t start_pool_size = 5,
        const std::chrono::seconds wait_time = std::chrono::seconds(10))
        : IConnectionPool(std::move(settings), start_pool_size, start_pool_size, wait_time)
    {
    }

    IConnectionPool(const IConnectionPool& other) = delete;
    IConnectionPool(IConnectionPool&& other) noexcept = delete;
    IConnectionPool& operator=(const IConnectionPool& other) = delete;
    IConnectionPool& operator=(IConnectionPool&& other) noexcept = delete;

    virtual ~IConnectionPool() = default;

    void set_max_pool_size(const size_t max_pool_size)
    {
        _max_pool_size = max_pool_size;
    }

    void set_settings(const database_connection_settings& settings)
    {
        _settings = settings;

        init_start_conncetions();
    }

    void set_wait_time(const std::chrono::seconds& wait_time)
    {
        _wait_time = wait_time;
    }

    std::shared_ptr<IConnection> open_connection()
    {
        std::lock_guard<std::mutex> lock_guard(_lock);

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

private:
    void init_start_conncetions()
    {
        _connections.clear();

        _connections.reserve(_start_pool_size);
        for(auto i = 0; i < _start_pool_size; i++) {
            _connections.push_back(create_connection(_settings));
        }
    }

protected:
    virtual std::shared_ptr<IConnection> create_connection(const database_connection_settings& settings) = 0;

private:
    std::mutex _lock;

    database_connection_settings _settings {};

    size_t _start_pool_size = 2;
    size_t _max_pool_size = 10;
    std::chrono::seconds _wait_time = std::chrono::seconds(2);

    std::vector<std::shared_ptr<IConnection>> _connections {};
};

} // namespace database_adapter