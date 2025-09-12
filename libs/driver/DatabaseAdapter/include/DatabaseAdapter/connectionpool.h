#pragma once

#include "DatabaseAdapter/model/databasesettings.h"
#include "iconnection.h"

#include <chrono>
#include <memory>
#include <mutex>
#include <vector>

namespace database_adapter {

class IConnectionPool
{
public:
    IConnectionPool() = default;

    explicit IConnectionPool(database_connection_settings settings,
        size_t start_pool_size,
        size_t max_pool_size,
        std::chrono::seconds wait_time = std::chrono::seconds(10));

    explicit IConnectionPool(database_connection_settings settings,
        size_t start_pool_size = 5,
        std::chrono::seconds wait_time = std::chrono::seconds(10));

    IConnectionPool(const IConnectionPool& other) = delete;
    IConnectionPool(IConnectionPool&& other) noexcept = delete;
    IConnectionPool& operator=(const IConnectionPool& other) = delete;
    IConnectionPool& operator=(IConnectionPool&& other) noexcept = delete;

    virtual ~IConnectionPool() = default;

    void set_max_pool_size(size_t max_pool_size);

    void set_settings(const database_connection_settings& settings);

    void set_wait_time(const std::chrono::seconds& wait_time);

    std::shared_ptr<IConnection> open_connection();

private:
    void init_start_conncetions();

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