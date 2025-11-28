#pragma once

#include <DatabaseAdapter/iconnection.hpp>
#include <DatabaseAdapter/model/queryresult.hpp>
#include <gmock/gmock.h>

#include <string>
#include <vector>

/// @brief Mock-реализация IConnection для тестирования с использованием GoogleTest Mock
class MockConnection final : public database_adapter::IConnection
{
public:
    explicit MockConnection(const database_adapter::database_connection_settings& settings)
        : IConnection(settings)
    {
    }

    // MOCK_METHOD для виртуальных методов
    MOCK_METHOD(bool, is_valid, (), (override));
    MOCK_METHOD(database_adapter::query_result, exec, (const std::string& query), (override));
    MOCK_METHOD(void, prepare, (const std::string& query, const std::string& name), (override));
    MOCK_METHOD(database_adapter::query_result, exec_prepared, (const std::vector<std::string>& params, const std::string& name), (override));
    MOCK_METHOD(bool, open_transaction, (database_adapter::transaction_isolation_level type), (override));
};
