#include "mock/mock_connection.hpp"

#include "mock/databaseadapter.hpp"
#include <DatabaseAdapter/exception/sqlexception.hpp>
#include <DatabaseAdapter/transaction_isolation.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>

/**
 * @brief Тест выполнения запроса
 */
TEST_F(DatabaseAdapter, Connection_ExecQuery)
{
    database_adapter::query_result result;
    result.add({ { "id", "1" }, { "name", "Test" } });

    EXPECT_CALL(*_mock_connection, exec("SELECT * FROM users"))
        .WillOnce(testing::Return(result));

    auto query_result = _mock_connection->exec("SELECT * FROM users");
    EXPECT_FALSE(query_result.empty());
    EXPECT_EQ(query_result.size(), 1);
}

/**
 * @brief Тест подготовки запроса
 */
TEST_F(DatabaseAdapter, Connection_PrepareQuery)
{
    EXPECT_CALL(*_mock_connection, prepare("SELECT * FROM users WHERE id = $1", "get_user"))
        .Times(1);

    _mock_connection->prepare("SELECT * FROM users WHERE id = $1", "get_user");
}

/**
 * @brief Тест выполнения подготовленного запроса
 */
TEST_F(DatabaseAdapter, Connection_ExecPreparedQuery)
{
    database_adapter::query_result result;
    result.add({ { "id", "1" }, { "name", "Test" } });

    EXPECT_CALL(*_mock_connection, exec_prepared(std::vector<std::string> { "1" }, "get_user"))
        .WillOnce(testing::Return(result));

    auto query_result = _mock_connection->exec_prepared({ "1" }, "get_user");
    EXPECT_FALSE(query_result.empty());
}

/**
 * @brief Тест открытия транзакции
 */
TEST_F(DatabaseAdapter, Connection_BeginTransaction)
{
    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));

    bool result = _mock_connection->begin_transaction();
    EXPECT_TRUE(result);
    EXPECT_TRUE(_mock_connection->is_transaction());
}

/**
 * @brief Тест открытия транзакции с уровнем изоляции
 */
TEST_F(DatabaseAdapter, Connection_OpenTransactionWithIsolationLevel)
{
    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::READ_COMMITTED))
        .WillOnce(testing::Return(true));

    bool result = _mock_connection->open_transaction(database_adapter::transaction_isolation_level::READ_COMMITTED);
    EXPECT_TRUE(result);
}

/**
 * @brief Тест неудачного открытия транзакции
 */
TEST_F(DatabaseAdapter, Connection_FailedBeginTransaction)
{
    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(false));

    bool result = _mock_connection->begin_transaction();
    EXPECT_FALSE(result);
    EXPECT_FALSE(_mock_connection->is_transaction());
}

/**
 * @brief Тест commit транзакции
 */
TEST_F(DatabaseAdapter, Connection_CommitTransaction)
{
    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(*_mock_connection, exec("COMMIT;"))
        .WillOnce(testing::Return(database_adapter::query_result()));

    _mock_connection->begin_transaction();
    EXPECT_TRUE(_mock_connection->is_transaction());

    _mock_connection->commit();
    EXPECT_FALSE(_mock_connection->is_transaction());
}

/**
 * @brief Тест commit без активной транзакции
 */
TEST_F(DatabaseAdapter, Connection_CommitWithoutTransaction)
{
    EXPECT_THROW(_mock_connection->commit(), database_adapter::sql_exception);
}

/**
 * @brief Тест rollback транзакции
 */
TEST_F(DatabaseAdapter, Connection_RollbackTransaction)
{
    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(*_mock_connection, exec("ROLLBACK;"))
        .WillOnce(testing::Return(database_adapter::query_result()));

    _mock_connection->begin_transaction();
    EXPECT_TRUE(_mock_connection->is_transaction());

    _mock_connection->rollback();
    EXPECT_FALSE(_mock_connection->is_transaction());
}

/**
 * @brief Тест rollback без активной транзакции
 */
TEST_F(DatabaseAdapter, Connection_RollbackWithoutTransaction)
{
    EXPECT_THROW(_mock_connection->rollback(), database_adapter::sql_exception);
}

/**
 * @brief Тест добавления savepoint
 */
TEST_F(DatabaseAdapter, Connection_AddSavepoint)
{
    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(*_mock_connection, exec("SAVEPOINT sp1"))
        .WillOnce(testing::Return(database_adapter::query_result()));

    _mock_connection->begin_transaction();
    _mock_connection->add_save_point("sp1");
}

/**
 * @brief Тест добавления savepoint без транзакции
 */
TEST_F(DatabaseAdapter, Connection_AddSavepointWithoutTransaction)
{
    EXPECT_THROW({ _mock_connection->add_save_point("sp1"); }, database_adapter::sql_exception);
}

/**
 * @brief Тест rollback to savepoint
 */
TEST_F(DatabaseAdapter, Connection_RollbackToSavepoint)
{
    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(*_mock_connection, exec("SAVEPOINT sp1"))
        .WillOnce(testing::Return(database_adapter::query_result()));
    EXPECT_CALL(*_mock_connection, exec("ROLLBACK TO sp1"))
        .WillOnce(testing::Return(database_adapter::query_result()));

    _mock_connection->begin_transaction();
    _mock_connection->add_save_point("sp1");
    _mock_connection->rollback_to_save_point("sp1");
}

/**
 * @brief Тест rollback to savepoint без транзакции
 */
TEST_F(DatabaseAdapter, Connection_RollbackToSavepointWithoutTransaction)
{
    EXPECT_THROW({ _mock_connection->rollback_to_save_point("sp1"); }, database_adapter::sql_exception);
}

/**
 * @brief Тест проверки наличия транзакции
 */
TEST_F(DatabaseAdapter, Connection_IsTransaction)
{
    EXPECT_FALSE(_mock_connection->is_transaction());

    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));

    _mock_connection->begin_transaction();
    EXPECT_TRUE(_mock_connection->is_transaction());
}

/**
 * @brief Тест потокобезопасности is_transaction
 */
TEST_F(DatabaseAdapter, Connection_IsTransactionThreadSafe)
{
    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));

    _mock_connection->begin_transaction();

    // Проверка из разных потоков
    std::vector<std::thread> threads;
    std::atomic<int> success_count { 0 };

    for(int i = 0; i < 10; ++i) {
        threads.emplace_back([this, &success_count]() {
            if(_mock_connection->is_transaction()) {
                ++success_count;
            }
        });
    }

    for(auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(success_count.load(), 10);
}

/**
 * @brief Тест последовательности операций с транзакцией
 */
TEST_F(DatabaseAdapter, Connection_TransactionSequence)
{
    EXPECT_CALL(*_mock_connection, open_transaction(database_adapter::transaction_isolation_level::DEFAULT))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(*_mock_connection, exec("INSERT INTO users VALUES (1)"))
        .WillOnce(testing::Return(database_adapter::query_result()));
    EXPECT_CALL(*_mock_connection, exec("SAVEPOINT sp1"))
        .WillOnce(testing::Return(database_adapter::query_result()));
    EXPECT_CALL(*_mock_connection, exec("INSERT INTO users VALUES (2)"))
        .WillOnce(testing::Return(database_adapter::query_result()));
    EXPECT_CALL(*_mock_connection, exec("ROLLBACK TO sp1"))
        .WillOnce(testing::Return(database_adapter::query_result()));
    EXPECT_CALL(*_mock_connection, exec("COMMIT;"))
        .WillOnce(testing::Return(database_adapter::query_result()));

    _mock_connection->begin_transaction();
    _mock_connection->exec("INSERT INTO users VALUES (1)");
    _mock_connection->add_save_point("sp1");
    _mock_connection->exec("INSERT INTO users VALUES (2)");
    _mock_connection->rollback_to_save_point("sp1");
    _mock_connection->commit();
}
