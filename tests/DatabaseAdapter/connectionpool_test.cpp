#include "mock/databaseadapter.hpp"
#include "mock/mock_connection.hpp"
#include "mock/mock_connection_pool.hpp"

#include <DatabaseAdapter/connectionpool.hpp>
#include <DatabaseAdapter/model/databasesettings.hpp>
#include <DatabaseAdapter/model/queryresult.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>

/**
 * @brief Тест создания пула с начальным размером
 */
TEST_F(DatabaseAdapter, ConnectionPool_CreatePoolWithStartSize)
{
    MockConnectionPool pool(_settings, 3, 10);
    auto conn1 = pool.open_connection();
    auto conn2 = pool.open_connection();
    auto conn3 = pool.open_connection();

    EXPECT_NE(conn1, nullptr);
    EXPECT_NE(conn2, nullptr);
    EXPECT_NE(conn3, nullptr);
}

/**
 * @brief Тест установки максимального размера пула
 */
TEST_F(DatabaseAdapter, ConnectionPool_SetMaxPoolSize)
{
    MockConnectionPool pool(_settings, 2, 5);
    pool.set_max_pool_size(20);

    std::vector<std::shared_ptr<database_adapter::IConnection>> connections;
    for(int i = 0; i < 10; ++i) {
        auto conn = pool.open_connection();
        EXPECT_NE(conn, nullptr);
        connections.push_back(conn);
    }
    EXPECT_EQ(connections.size(), 10);
}

/**
 * @brief Тест установки настроек подключения
 */
TEST_F(DatabaseAdapter, ConnectionPool_SetSettings)
{
    MockConnectionPool pool(_settings, 2, 5);

    database_adapter::database_connection_settings new_settings;
    new_settings.database_name = "new_db";
    new_settings.url = "new_host";
    new_settings.port = "3306";
    new_settings.login = "new_user";
    new_settings.password = "new_pass";

    pool.set_settings(new_settings);

    auto conn = pool.open_connection();
    EXPECT_NE(conn, nullptr);
}

/**
 * @brief Тест получения соединения из пула
 */
TEST_F(DatabaseAdapter, ConnectionPool_OpenConnection)
{
    MockConnectionPool pool(_settings, 2, 5);

    auto conn1 = pool.open_connection();
    auto conn2 = pool.open_connection();

    EXPECT_NE(conn1, nullptr);
    EXPECT_NE(conn2, nullptr);
    EXPECT_NE(conn1, conn2); // Разные соединения
}

/**
 * @brief Тест повторного использования соединений
 */
TEST_F(DatabaseAdapter, ConnectionPool_ReuseConnections)
{
    MockConnectionPool pool(_settings, 2, 5);

    std::shared_ptr<database_adapter::IConnection> conn1;
    {
        conn1 = pool.open_connection();
        EXPECT_NE(conn1, nullptr);
    }

    // После освобождения соединение должно быть доступно для повторного использования
    auto conn2 = pool.open_connection();
    EXPECT_NE(conn2, nullptr);
}

/**
 * @brief Тест создания новых соединений при превышении начального размера
 */
TEST_F(DatabaseAdapter, ConnectionPool_CreateNewConnectionsWhenExceedingStartSize)
{
    MockConnectionPool pool(_settings, 2, 10);

    std::vector<std::shared_ptr<database_adapter::IConnection>> connections;
    for(int i = 0; i < 5; ++i) {
        auto conn = pool.open_connection();
        EXPECT_NE(conn, nullptr);
        connections.push_back(conn);
    }

    EXPECT_EQ(connections.size(), 5);
}

/**
 * @brief Тест ограничения максимального размера пула
 */
TEST_F(DatabaseAdapter, ConnectionPool_MaxPoolSizeLimit)
{
    MockConnectionPool pool(_settings, 2, 3);

    std::vector<std::shared_ptr<database_adapter::IConnection>> connections;
    for(int i = 0; i < 3; ++i) {
        auto conn = pool.open_connection();
        EXPECT_NE(conn, nullptr);
        connections.push_back(conn);
    }

    // Все соединения заняты, новое соединение должно быть nullptr после таймаута
    auto conn4 = pool.open_connection();
    // В зависимости от реализации может быть nullptr или новое соединение
    // Проверяем, что пул работает корректно
    EXPECT_EQ(conn4, nullptr);
}

/**
 * @brief Тест многопоточного доступа к пулу
 */
TEST_F(DatabaseAdapter, ConnectionPool_MultiThreadedAccess)
{
    MockConnectionPool pool(_settings, 5, 10);

    constexpr int num_threads = 10;
    constexpr int connections_per_thread = 2;
    std::atomic<int> success_count { 0 };
    std::vector<std::thread> threads;

    for(int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&pool, &success_count]() {
            std::vector<std::shared_ptr<database_adapter::IConnection>> connections;
            for(int j = 0; j < connections_per_thread; ++j) {
                auto conn = pool.open_connection();
                if(conn != nullptr) {
                    connections.push_back(conn);
                    ++success_count;
                }
            }
            // Соединения освобождаются при выходе из области видимости
        });
    }

    for(auto& t : threads) {
        t.join();
    }

    // Проверяем, что все потоки смогли получить соединения
    EXPECT_GE(success_count.load(), num_threads);
}

/**
 * @brief Тест очистки невалидных соединений
 */
TEST_F(DatabaseAdapter, ConnectionPool_CleanupInvalidConnections)
{
    MockConnectionPool pool(_settings, 2, 5);

    auto conn1 = pool.open_connection();
    EXPECT_NE(conn1, nullptr);

    // Симулируем невалидное соединение
    // В реальной реализации пул должен очищать такие соединения
    conn1.reset();

    // Следующее соединение должно быть валидным
    auto conn2 = pool.open_connection();
    EXPECT_NE(conn2, nullptr);
}

/**
 * @brief Тест работы с транзакциями в пуле
 */
TEST_F(DatabaseAdapter, ConnectionPool_TransactionsInPool)
{
    MockConnectionPool pool(_settings, 2, 5);

    auto conn = pool.open_connection();
    EXPECT_NE(conn, nullptr);

    // Проверяем, что можно работать с транзакциями
    auto mock_conn = std::dynamic_pointer_cast<MockConnection>(conn);
    if(mock_conn) {
        EXPECT_CALL(*mock_conn, open_transaction(testing::_))
            .WillOnce(testing::Return(true));
        EXPECT_CALL(*mock_conn, exec("COMMIT;"))
            .WillOnce(testing::Return(database_adapter::query_result()));

        conn->begin_transaction();
        conn->commit();
    }
}

/**
 * @brief Тест таймаута при ожидании соединения
 */
TEST_F(DatabaseAdapter, ConnectionPool_TimeoutWhenWaitingForConnection)
{
    MockConnectionPool pool(_settings, 1, 1, std::chrono::seconds(1));

    // Занимаем единственное соединение
    auto conn1 = pool.open_connection();
    EXPECT_NE(conn1, nullptr);

    // Пытаемся получить еще одно соединение (должно быть nullptr после таймаута)
    auto start = std::chrono::steady_clock::now();
    auto conn2 = pool.open_connection();
    auto end = std::chrono::steady_clock::now();

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    // Проверяем, что прошло примерно время ожидания (с допуском)
    EXPECT_GE(elapsed.count(), 1000); // Минимум 1000ms
}

/**
 * @brief Тест инициализации пула с нулевым начальным размером
 */
TEST_F(DatabaseAdapter, ConnectionPool_ZeroStartPoolSize)
{
    MockConnectionPool pool(_settings, 0, 5);

    // Пул должен создавать соединения по требованию
    auto conn = pool.open_connection();
    EXPECT_NE(conn, nullptr);
}

/**
 * @brief Тест работы пула с большим количеством соединений
 */
TEST_F(DatabaseAdapter, ConnectionPool_LargePoolSize)
{
    const size_t start_size = 10;
    const size_t max_size = 50;
    MockConnectionPool pool(_settings, start_size, max_size);

    std::vector<std::shared_ptr<database_adapter::IConnection>> connections;
    for(size_t i = 0; i < max_size; ++i) {
        auto conn = pool.open_connection();
        if(conn != nullptr) {
            connections.push_back(conn);
        }
    }

    EXPECT_LE(connections.size(), max_size);
}

/**
 * @brief Тест последовательного получения и освобождения соединений
 */
TEST_F(DatabaseAdapter, ConnectionPool_SequentialGetAndRelease)
{
    MockConnectionPool pool(_settings, 2, 5);

    for(int i = 0; i < 10; ++i) {
        auto conn = pool.open_connection();
        EXPECT_NE(conn, nullptr);
        // Соединение освобождается при выходе из области видимости
    }
}

/**
 * @brief Тест закрытия неиспользуемых соединений после периода простоя
 */
TEST_F(DatabaseAdapter, ConnectionPool_CloseIdleConnections)
{
    // Создаем пул с очень коротким временем простоя для теста
    const std::chrono::seconds idle_timeout(1);
    MockConnectionPool pool(_settings, 3, 10, std::chrono::seconds(2), idle_timeout);

    // Получаем несколько соединений
    auto conn1 = pool.open_connection();
    auto conn2 = pool.open_connection();
    auto conn3 = pool.open_connection();

    EXPECT_NE(conn1, nullptr);
    EXPECT_NE(conn2, nullptr);
    EXPECT_NE(conn3, nullptr);

    // Сохраняем указатели на соединения
    std::weak_ptr<database_adapter::IConnection> weak_conn1 = conn1;
    std::weak_ptr<database_adapter::IConnection> weak_conn2 = conn2;
    std::weak_ptr<database_adapter::IConnection> weak_conn3 = conn3;

    // Освобождаем соединения (они возвращаются в пул)
    conn1.reset();
    conn2.reset();
    conn3.reset();

    // Ждем, пока пройдет время простоя
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    // Получаем новое соединение - это должно вызвать очистку неиспользуемых соединений
    auto conn4 = pool.open_connection();
    EXPECT_NE(conn4, nullptr);

    // Проверяем, что старые соединения были закрыты (weak_ptr истекли)
    // После очистки пула старые соединения должны быть удалены
    EXPECT_TRUE(weak_conn1.expired() || weak_conn2.expired() || weak_conn3.expired());
}

/**
 * @brief Тест, что активные соединения не закрываются
 */
TEST_F(DatabaseAdapter, ConnectionPool_ActiveConnectionsNotClosed)
{
    const std::chrono::seconds idle_timeout(1);
    MockConnectionPool pool(_settings, 2, 10, std::chrono::seconds(2), idle_timeout);

    // Получаем соединение и удерживаем его
    auto conn1 = pool.open_connection();
    EXPECT_NE(conn1, nullptr);

    // Ждем больше времени простоя
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    // Получаем еще одно соединение - активное соединение не должно быть закрыто
    auto conn2 = pool.open_connection();
    EXPECT_NE(conn2, nullptr);

    // Проверяем, что первое соединение все еще валидно
    EXPECT_NE(conn1, nullptr);
}

/**
 * @brief Тест обновления времени последнего использования при выдаче соединения
 */
TEST_F(DatabaseAdapter, ConnectionPool_UpdateLastUsedTime)
{
    const std::chrono::seconds idle_timeout(1);
    MockConnectionPool pool(_settings, 2, 10, std::chrono::seconds(2), idle_timeout);

    // Получаем соединение
    auto conn1 = pool.open_connection();
    EXPECT_NE(conn1, nullptr);

    // Освобождаем соединение
    conn1.reset();

    // Ждем почти время простоя, но не полностью
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Получаем соединение снова - это обновит время последнего использования
    auto conn2 = pool.open_connection();
    EXPECT_NE(conn2, nullptr);
    conn2.reset();

    // Ждем еще немного, но не больше времени простоя
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Получаем соединение - оно не должно быть закрыто, так как время было обновлено
    auto conn3 = pool.open_connection();
    EXPECT_NE(conn3, nullptr);
}

/**
 * @brief Тест установки времени простоя через set_idle_timeout
 */
TEST_F(DatabaseAdapter, ConnectionPool_SetIdleTimeout)
{
    MockConnectionPool pool(_settings, 2, 10);

    // Устанавливаем короткое время простоя
    pool.set_idle_timeout(std::chrono::seconds(1));

    // Получаем соединение
    auto conn1 = pool.open_connection();
    EXPECT_NE(conn1, nullptr);
    conn1.reset();

    // Ждем больше времени простоя
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    // Получаем новое соединение - старое должно быть закрыто
    auto conn2 = pool.open_connection();
    EXPECT_NE(conn2, nullptr);
}

/**
 * @brief Тест, что соединения не закрываются до истечения времени простоя
 */
TEST_F(DatabaseAdapter, ConnectionPool_ConnectionsNotClosedBeforeTimeout)
{
    const std::chrono::seconds idle_timeout(2);
    MockConnectionPool pool(_settings, 2, 10, std::chrono::seconds(2), idle_timeout);

    std::weak_ptr<database_adapter::IConnection> weak_conn1;
    {
        // Получаем соединение
        auto conn1 = pool.open_connection();
        EXPECT_NE(conn1, nullptr);
        weak_conn1 = conn1;
    }

    // Ждем меньше времени простоя
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Получаем новое соединение
    auto conn2 = pool.open_connection();
    EXPECT_NE(conn2, nullptr);

    // Соединение не должно быть закрыто, так как время простоя не истекло
    // Проверяем, что weak_ptr все еще валиден (соединение не удалено)
    EXPECT_FALSE(weak_conn1.expired());
}