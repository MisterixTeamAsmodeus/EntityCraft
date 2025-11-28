#pragma once

#include "DatabaseAdapter/model/databasesettings.hpp"
#include "gtest/gtest.h"
#include "mock_connection.hpp"

class DatabaseAdapter : public ::testing::Test
{
protected:
    void SetUp() override
    {
        _settings.database_name = "test_db";
        _settings.url = "localhost";
        _settings.port = "5432";
        _settings.login = "user";
        _settings.password = "password";

        _mock_connection = std::make_shared<MockConnection>(_settings);

        // Подготовка тестовых данных
        _row1["id"] = "1";
        _row1["name"] = "Test1";
        _row1["value"] = "100";

        _row2["id"] = "2";
        _row2["name"] = "Test2";
        _row2["value"] = "200";

        _row3["id"] = "3";
        _row3["name"] = "Test3";
        _row3["value"] = "300";
    }

    database_adapter::database_connection_settings _settings;
    std::shared_ptr<MockConnection> _mock_connection;

    database_adapter::query_result::row _row1;
    database_adapter::query_result::row _row2;
    database_adapter::query_result::row _row3;
};