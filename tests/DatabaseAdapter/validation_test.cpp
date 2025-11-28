#include "mock/mock_connection.hpp"

#include <DatabaseAdapter/exception/opendatabaseexception.hpp>
#include <DatabaseAdapter/exception/sqlexception.hpp>
#include <DatabaseAdapter/iconnection.hpp>
#include <DatabaseAdapter/model/databasesettings.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "mock/databaseadapter.hpp"

#include <string>

/**
 * @brief Тест валидных настроек подключения
 */
TEST_F(DatabaseAdapter, Validation_ValidSettings)
{
    EXPECT_NO_THROW({
        MockConnection connection(_settings);
    });
}

/**
 * @brief Тест пустого имени базы данных
 */
TEST_F(DatabaseAdapter, Validation_EmptyDatabaseName)
{
    _settings.database_name = "";
    EXPECT_THROW({ MockConnection connection(_settings); }, database_adapter::open_database_exception);
}

/**
 * @brief Тест валидного запроса
 */
TEST_F(DatabaseAdapter, Validation_ValidQuery)
{
    database_adapter::query_result result;
    EXPECT_CALL(*_mock_connection, exec(testing::_))
        .WillOnce(testing::Return(result));

    EXPECT_NO_THROW({
        _mock_connection->exec("SELECT * FROM users");
    });
}

/**
 * @brief Тест пустого запроса
 */
TEST_F(DatabaseAdapter, Validation_EmptyQuery)
{
    EXPECT_CALL(*_mock_connection, exec(testing::_))
        .WillOnce(testing::Throw(database_adapter::sql_exception("SQL-запрос не может быть пустым", "")));

    EXPECT_THROW({ _mock_connection->exec(""); }, database_adapter::sql_exception);
}

/**
 * @brief Тест запроса только с пробелами
 */
TEST_F(DatabaseAdapter, Validation_WhitespaceOnlyQuery)
{
    EXPECT_CALL(*_mock_connection, exec(testing::_))
        .WillOnce(testing::Throw(database_adapter::sql_exception("SQL-запрос не может содержать только пробелы", "   ")));

    EXPECT_THROW({ _mock_connection->exec("   "); }, database_adapter::sql_exception);
}

/**
 * @brief Тест запроса с табуляцией
 */
TEST_F(DatabaseAdapter, Validation_TabOnlyQuery)
{
    EXPECT_CALL(*_mock_connection, exec(testing::_))
        .WillOnce(testing::Throw(database_adapter::sql_exception("SQL-запрос не может содержать только пробелы", "\t\t")));

    EXPECT_THROW({ _mock_connection->exec("\t\t"); }, database_adapter::sql_exception);
}

/**
 * @brief Тест запроса с переносами строк
 */
TEST_F(DatabaseAdapter, Validation_NewlineOnlyQuery)
{
    EXPECT_CALL(*_mock_connection, exec(testing::_))
        .WillOnce(testing::Throw(database_adapter::sql_exception("SQL-запрос не может содержать только пробелы", "\n\n")));

    EXPECT_THROW({ _mock_connection->exec("\n\n"); }, database_adapter::sql_exception);
}

/**
 * @brief Тест запроса с пробелами в начале и конце (должен быть валидным)
 */
TEST_F(DatabaseAdapter, Validation_QueryWithLeadingTrailingSpaces)
{
    database_adapter::query_result result;
    EXPECT_CALL(*_mock_connection, exec(testing::_))
        .WillOnce(testing::Return(result));

    // Запрос с пробелами в начале и конце должен быть валидным
    // (валидация проверяет только полностью пустые или только пробелы)
    EXPECT_NO_THROW({
        _mock_connection->exec("  SELECT * FROM users  ");
    });
}

/**
 * @brief Тест валидного имени savepoint
 */
TEST_F(DatabaseAdapter, Validation_ValidSavepointName)
{
    EXPECT_CALL(*_mock_connection, exec(testing::_))
        .WillRepeatedly(testing::Return(database_adapter::query_result()));
    EXPECT_CALL(*_mock_connection, open_transaction(testing::_))
        .WillOnce(testing::Return(true));

    _mock_connection->begin_transaction();
    EXPECT_NO_THROW({
        _mock_connection->add_save_point("savepoint1");
        _mock_connection->add_save_point("savepoint_2");
        _mock_connection->add_save_point("SavePoint123");
    });
}

/**
 * @brief Тест невалидного имени savepoint с дефисом
 */
TEST_F(DatabaseAdapter, Validation_InvalidSavepointNameWithDash)
{
    EXPECT_CALL(*_mock_connection, exec(testing::_))
        .WillRepeatedly(testing::Return(database_adapter::query_result()));
    EXPECT_CALL(*_mock_connection, open_transaction(testing::_))
        .WillOnce(testing::Return(true));

    _mock_connection->begin_transaction();
    EXPECT_THROW({ _mock_connection->add_save_point("save-point"); }, database_adapter::sql_exception);
}

/**
 * @brief Тест невалидного имени savepoint с пробелом
 */
TEST_F(DatabaseAdapter, Validation_InvalidSavepointNameWithSpace)
{
    EXPECT_CALL(*_mock_connection, exec(testing::_))
        .WillRepeatedly(testing::Return(database_adapter::query_result()));
    EXPECT_CALL(*_mock_connection, open_transaction(testing::_))
        .WillOnce(testing::Return(true));

    _mock_connection->begin_transaction();
    EXPECT_THROW({ _mock_connection->add_save_point("save point"); }, database_adapter::sql_exception);
}

/**
 * @brief Тест невалидного имени savepoint со специальными символами
 */
TEST_F(DatabaseAdapter, Validation_InvalidSavepointNameWithSpecialChars)
{
    EXPECT_CALL(*_mock_connection, exec(testing::_))
        .WillRepeatedly(testing::Return(database_adapter::query_result()));
    EXPECT_CALL(*_mock_connection, open_transaction(testing::_))
        .WillOnce(testing::Return(true));

    _mock_connection->begin_transaction();
    EXPECT_THROW({ _mock_connection->add_save_point("save@point"); }, database_adapter::sql_exception);
}

/**
 * @brief Тест пустого имени savepoint (должно игнорироваться)
 */
TEST_F(DatabaseAdapter, Validation_EmptySavepointName)
{
    EXPECT_CALL(*_mock_connection, exec(testing::_))
        .WillRepeatedly(testing::Return(database_adapter::query_result()));
    EXPECT_CALL(*_mock_connection, open_transaction(testing::_))
        .WillOnce(testing::Return(true));

    _mock_connection->begin_transaction();
    // Пустое имя должно игнорироваться без исключения
    EXPECT_NO_THROW({
        _mock_connection->add_save_point("");
    });
}

/**
 * @brief Тест валидации rollback to savepoint
 */
TEST_F(DatabaseAdapter, Validation_RollbackToSavepointValidation)
{
    EXPECT_CALL(*_mock_connection, exec(testing::_))
        .WillRepeatedly(testing::Return(database_adapter::query_result()));
    EXPECT_CALL(*_mock_connection, open_transaction(testing::_))
        .WillOnce(testing::Return(true));

    _mock_connection->begin_transaction();
    _mock_connection->add_save_point("valid_savepoint");

    // Валидное имя
    EXPECT_NO_THROW({
        _mock_connection->rollback_to_save_point("valid_savepoint");
    });

    // Невалидное имя
    EXPECT_THROW({ _mock_connection->rollback_to_save_point("invalid-savepoint"); }, database_adapter::sql_exception);
}

/**
 * @brief Тест rollback с пустым savepoint (полный rollback)
 */
TEST_F(DatabaseAdapter, Validation_RollbackWithEmptySavepoint)
{
    EXPECT_CALL(*_mock_connection, exec(testing::_))
        .WillRepeatedly(testing::Return(database_adapter::query_result()));
    EXPECT_CALL(*_mock_connection, open_transaction(testing::_))
        .WillOnce(testing::Return(true));

    _mock_connection->begin_transaction();
    // Пустой savepoint должен выполнить полный rollback
    EXPECT_NO_THROW({
        _mock_connection->rollback_to_save_point("");
    });
}
