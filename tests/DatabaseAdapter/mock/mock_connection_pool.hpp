#pragma once

#include <DatabaseAdapter/connectionpool.hpp>

#include <memory>

/**
 * @brief Mock-реализация IConnectionPool для тестирования
 */
class MockConnectionPool : public database_adapter::IConnectionPool
{
public:
    explicit MockConnectionPool(database_adapter::database_connection_settings settings,
        size_t start_pool_size = 2,
        size_t max_pool_size = 10,
        std::chrono::seconds wait_time = std::chrono::seconds(2),
        std::chrono::seconds idle_timeout = std::chrono::seconds(300))
        : IConnectionPool(settings, start_pool_size, max_pool_size, wait_time, idle_timeout)
    {
    }

    explicit MockConnectionPool(database_adapter::database_connection_settings settings,
        size_t start_pool_size = 2,
        std::chrono::seconds wait_time = std::chrono::seconds(2),
        std::chrono::seconds idle_timeout = std::chrono::seconds(300))
        : IConnectionPool(settings, start_pool_size, wait_time, idle_timeout)
    {
    }

protected:
    class connection final : public database_adapter::IConnection
    {
    public:
        explicit connection(const database_adapter::database_connection_settings& settings)
            : IConnection(settings)
        {
        }

        database_adapter::query_result exec(const std::string& query) override
        {
            return {};
        }

        void prepare(const std::string& query, const std::string& name) override
        {
        }

        database_adapter::query_result exec_prepared(const std::vector<std::string>& params, const std::string& name) override
        {
            return {};
        }

        bool open_transaction(database_adapter::transaction_isolation_level level) override
        {
            _has_transaction = true;
            return true;
        }
    };

    /**
     * @brief Создает новое соединение для пула
     */
    std::shared_ptr<database_adapter::IConnection> create_connection(
        const database_adapter::database_connection_settings& settings) override
    {
        return std::make_shared<connection>(settings);
    }
};
