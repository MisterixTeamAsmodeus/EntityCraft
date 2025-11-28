#include "DatabaseAdapter/transaction_guard.hpp"
#include "mock/databaseadapter.hpp"
#include "mock/mock_connection.hpp"

#include <DatabaseAdapter/exception/opendatabaseexception.hpp>
#include <DatabaseAdapter/exception/sqlexception.hpp>
#include <DatabaseAdapter/iconnection.hpp>
#include <DatabaseAdapter/transaction_isolation.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

/**
 * @brief Тест конструктора с соединением
 */
TEST_F(DatabaseAdapter, TransactionGuard_ConstructorWithConnection)
{
    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));

    database_adapter::transaction_guard guard(_mock_connection);
    EXPECT_FALSE(guard.is_finished());
    EXPECT_EQ(guard.get_connection(), _mock_connection);
}

/**
 * @brief Тест конструктора с уровнем изоляции
 */
TEST_F(DatabaseAdapter, TransactionGuard_ConstructorWithIsolationLevel)
{
    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::READ_COMMITTED))
        .WillOnce(testing::Return(true));

    database_adapter::transaction_guard guard(_mock_connection, database_adapter::transaction_isolation_level::READ_COMMITTED);
    EXPECT_FALSE(guard.is_finished());
}

/**
 * @brief Тест конструктора с nullptr
 */
TEST_F(DatabaseAdapter, TransactionGuard_ConstructorWithNullptr)
{
    EXPECT_THROW({ database_adapter::transaction_guard guard(nullptr); }, database_adapter::open_database_exception);
}

/**
 * @brief Тест конструктора при неудачном открытии транзакции
 */
TEST_F(DatabaseAdapter, TransactionGuard_ConstructorWithFailedTransaction)
{
    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(false));

    EXPECT_THROW({ database_adapter::transaction_guard guard(_mock_connection); }, database_adapter::sql_exception);
}

/**
 * @brief Тест commit
 */
TEST_F(DatabaseAdapter, TransactionGuard_Commit)
{

    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(*_mock_connection, exec("COMMIT;"))
        .WillOnce(testing::Return(database_adapter::query_result()));

    database_adapter::transaction_guard guard(_mock_connection);
    EXPECT_FALSE(guard.is_finished());

    guard.commit();
    EXPECT_TRUE(guard.is_finished());
}

/**
 * @brief Тест rollback
 */
TEST_F(DatabaseAdapter, TransactionGuard_Rollback)
{

    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(*_mock_connection, exec("ROLLBACK;"))
        .WillOnce(testing::Return(database_adapter::query_result()));

    database_adapter::transaction_guard guard(_mock_connection);
    EXPECT_FALSE(guard.is_finished());

    guard.rollback();
    EXPECT_TRUE(guard.is_finished());
}

/**
 * @brief Тест повторного commit
 */
TEST_F(DatabaseAdapter, TransactionGuard_DoubleCommit)
{

    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(*_mock_connection, exec("COMMIT;"))
        .WillOnce(testing::Return(database_adapter::query_result()));

    database_adapter::transaction_guard guard(_mock_connection);
    guard.commit();

    EXPECT_THROW({ guard.commit(); }, database_adapter::sql_exception);
}

/**
 * @brief Тест повторного rollback
 */
TEST_F(DatabaseAdapter, TransactionGuard_DoubleRollback)
{

    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(*_mock_connection, exec("ROLLBACK;"))
        .WillOnce(testing::Return(database_adapter::query_result()));

    database_adapter::transaction_guard guard(_mock_connection);
    guard.rollback();

    EXPECT_THROW({ guard.rollback(); }, database_adapter::sql_exception);
}

/**
 * @brief Тест commit после rollback
 */
TEST_F(DatabaseAdapter, TransactionGuard_CommitAfterRollback)
{

    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(*_mock_connection, exec("ROLLBACK;"))
        .WillOnce(testing::Return(database_adapter::query_result()));

    database_adapter::transaction_guard guard(_mock_connection);
    guard.rollback();

    EXPECT_THROW({ guard.commit(); }, database_adapter::sql_exception);
}

/**
 * @brief Тест rollback после commit
 */
TEST_F(DatabaseAdapter, TransactionGuard_RollbackAfterCommit)
{

    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(*_mock_connection, exec("COMMIT;"))
        .WillOnce(testing::Return(database_adapter::query_result()));

    database_adapter::transaction_guard guard(_mock_connection);
    guard.commit();

    EXPECT_THROW({ guard.rollback(); }, database_adapter::sql_exception);
}

/**
 * @brief Тест автоматического rollback в деструкторе
 */
TEST_F(DatabaseAdapter, TransactionGuard_AutomaticRollbackInDestructor)
{

    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(*_mock_connection, exec("ROLLBACK;"))
        .WillOnce(testing::Return(database_adapter::query_result()));

    {
        database_adapter::transaction_guard guard(_mock_connection);
        EXPECT_FALSE(guard.is_finished());
        // Деструктор должен вызвать rollback автоматически
    }
}

/**
 * @brief Тест отсутствия rollback после commit в деструкторе
 */
TEST_F(DatabaseAdapter, TransactionGuard_NoRollbackAfterCommitInDestructor)
{

    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(*_mock_connection, exec("COMMIT;"))
        .WillOnce(testing::Return(database_adapter::query_result()));

    {
        database_adapter::transaction_guard guard(_mock_connection);
        guard.commit();
        // Деструктор не должен вызывать rollback после commit
    }
}

/**
 * @brief Тест отсутствия rollback после rollback в деструкторе
 */
TEST_F(DatabaseAdapter, TransactionGuard_NoRollbackAfterRollbackInDestructor)
{

    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(*_mock_connection, exec("ROLLBACK;"))
        .WillOnce(testing::Return(database_adapter::query_result()));

    {
        database_adapter::transaction_guard guard(_mock_connection);
        guard.rollback();
        // Деструктор не должен вызывать rollback после rollback
    }
}

/**
 * @brief Тест получения соединения
 */
TEST_F(DatabaseAdapter, TransactionGuard_GetConnection)
{
    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));

    database_adapter::transaction_guard guard(_mock_connection);
    auto connection = guard.get_connection();

    EXPECT_EQ(connection, _mock_connection);
}

/**
 * @brief Тест работы с операциями внутри транзакции
 */
TEST_F(DatabaseAdapter, TransactionGuard_OperationsInsideTransaction)
{
    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(*_mock_connection, exec("INSERT INTO users VALUES (1)"))
        .WillOnce(testing::Return(database_adapter::query_result()));
    EXPECT_CALL(*_mock_connection, exec("INSERT INTO users VALUES (2)"))
        .WillOnce(testing::Return(database_adapter::query_result()));
    EXPECT_CALL(*_mock_connection, exec("COMMIT;"))
        .WillOnce(testing::Return(database_adapter::query_result()));

    database_adapter::transaction_guard guard(_mock_connection);
    auto conn = guard.get_connection();
    conn->exec("INSERT INTO users VALUES (1)");
    conn->exec("INSERT INTO users VALUES (2)");
    guard.commit();
}