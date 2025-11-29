#include <SqliteAdapter/sqliteadapter.hpp>

#include <DatabaseAdapter/databaseadapter.hpp>
#include <DatabaseAdapter/exception/opendatabaseexception.hpp>
#include <DatabaseAdapter/exception/sqlexception.hpp>
#include <DatabaseAdapter/model/queryresult.hpp>
#include <DatabaseAdapter/transaction_isolation.hpp>

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include <atomic>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <memory>
#include <string>
#include <thread>
#include <vector>

/**
 * @brief Фикстура для тестов SqliteAdapter
 */
class SqliteAdapter : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Создаем уникальное имя файла базы данных для каждого теста
        _db_path = "test_sqlite_" + std::to_string(std::time(nullptr)) + "_" + std::to_string(std::rand()) + ".db";
        
        _settings.url = _db_path;
        
        _connection = std::make_shared<database_adapter::sqlite::connection>(_settings);
    }

    void TearDown() override
    {
        _connection.reset();
        
        std::remove(_db_path.c_str());
    }

protected:
    std::string _db_path;
    database_adapter::sqlite::settings _settings;
    std::shared_ptr<database_adapter::sqlite::connection> _connection;
};

/**
 * @brief Тест создания соединения
 */
TEST_F(SqliteAdapter, Connection_Create)
{
    EXPECT_TRUE(_connection != nullptr);
    EXPECT_TRUE(_connection->is_valid());
}

/**
 * @brief Тест создания таблицы и выполнения запроса
 */
TEST_F(SqliteAdapter, Connection_CreateTableAndQuery)
{
    // Создаем таблицу
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, name TEXT)");
    
    // Вставляем данные
    _connection->exec("INSERT INTO test_table (name) VALUES ('Test1')");
    _connection->exec("INSERT INTO test_table (name) VALUES ('Test2')");
    
    // Выполняем запрос
    auto result = _connection->exec("SELECT id, name FROM test_table ORDER BY id");
    
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 2);
    
    const auto& first_row = result.at(0);
    EXPECT_EQ(first_row.at("id"), "1");
    EXPECT_EQ(first_row.at("name"), "Test1");
    
    const auto& second_row = result.at(1);
    EXPECT_EQ(second_row.at("id"), "2");
    EXPECT_EQ(second_row.at("name"), "Test2");
}

/**
 * @brief Тест выполнения запроса без результатов
 */
TEST_F(SqliteAdapter, Connection_ExecWithoutResults)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY)");
    
    auto result = _connection->exec("INSERT INTO test_table DEFAULT VALUES");
    EXPECT_TRUE(result.empty());
    
    result = _connection->exec("UPDATE test_table SET id = 10 WHERE id = 1");
    EXPECT_TRUE(result.empty());
}

/**
 * @brief Тест валидации запроса - пустой запрос
 */
TEST_F(SqliteAdapter, Connection_Validation_EmptyQuery)
{
    EXPECT_THROW(_connection->exec(""), database_adapter::sql_exception);
}

/**
 * @brief Тест валидации запроса - только пробелы
 */
TEST_F(SqliteAdapter, Connection_Validation_WhitespaceOnlyQuery)
{
    EXPECT_THROW(_connection->exec("   "), database_adapter::sql_exception);
}

/**
 * @brief Тест валидации запроса - только табуляция
 */
TEST_F(SqliteAdapter, Connection_Validation_TabOnlyQuery)
{
    EXPECT_THROW(_connection->exec("\t\t"), database_adapter::sql_exception);
}

/**
 * @brief Тест подготовки запроса
 */
TEST_F(SqliteAdapter, Connection_PrepareQuery)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, name TEXT)");
    
    EXPECT_NO_THROW({
        _connection->prepare("SELECT * FROM test_table WHERE id = ?", "get_by_id");
    });
}

/**
 * @brief Тест выполнения подготовленного запроса
 */
TEST_F(SqliteAdapter, Connection_ExecPreparedQuery)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, name TEXT)");
    _connection->exec("INSERT INTO test_table (name) VALUES ('Test1')");
    _connection->exec("INSERT INTO test_table (name) VALUES ('Test2')");
    
    _connection->prepare("SELECT * FROM test_table WHERE id = ?", "get_by_id");
    
    auto result = _connection->exec_prepared({ "1" }, "get_by_id");
    
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result.at(0).at("id"), "1");
    EXPECT_EQ(result.at(0).at("name"), "Test1");
}

/**
 * @brief Тест выполнения подготовленного запроса с несколькими параметрами
 */
TEST_F(SqliteAdapter, Connection_ExecPreparedQuery_MultipleParams)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, name TEXT, value INTEGER)");
    _connection->exec("INSERT INTO test_table (name, value) VALUES ('Test1', 100)");
    _connection->exec("INSERT INTO test_table (name, value) VALUES ('Test2', 200)");
    
    _connection->prepare("SELECT * FROM test_table WHERE name = ? AND value = ?", "get_by_name_and_value");
    
    auto result = _connection->exec_prepared({ "Test1", "100" }, "get_by_name_and_value");
    
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result.at(0).at("name"), "Test1");
    EXPECT_EQ(result.at(0).at("value"), "100");
}

/**
 * @brief Тест выполнения подготовленного запроса с NULL значением
 */
TEST_F(SqliteAdapter, Connection_ExecPreparedQuery_WithNull)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, name TEXT, value INTEGER)");
    _connection->exec("INSERT INTO test_table (name, value) VALUES ('Test1', NULL)");
    
    _connection->prepare("SELECT * FROM test_table WHERE value IS ?", "get_by_null_value");
    
    auto result = _connection->exec_prepared({ NULL_VALUE }, "get_by_null_value");
    
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result.at(0).at("name"), "Test1");
}

/**
 * @brief Тест обновления подготовленного запроса
 */
TEST_F(SqliteAdapter, Connection_PrepareQuery_UpdateExisting)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, name TEXT)");
    
    _connection->prepare("SELECT * FROM test_table WHERE id = ?", "get_by_id");
    
    // Обновляем подготовленный запрос
    EXPECT_NO_THROW({
        _connection->prepare("SELECT * FROM test_table WHERE name = ?", "get_by_id");
    });
    
    _connection->exec("INSERT INTO test_table (name) VALUES ('Test1')");
    
    auto result = _connection->exec_prepared({ "Test1" }, "get_by_id");
    EXPECT_FALSE(result.empty());
}

/**
 * @brief Тест выполнения несуществующего подготовленного запроса
 */
TEST_F(SqliteAdapter, Connection_ExecPreparedQuery_NonExistent)
{
    EXPECT_THROW({
        _connection->exec_prepared({ "1" }, "non_existent_query");
    }, database_adapter::sql_exception);
}

/**
 * @brief Тест выполнения подготовленного запроса с неправильным количеством параметров
 */
TEST_F(SqliteAdapter, Connection_ExecPreparedQuery_WrongParamCount)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, name TEXT)");
    _connection->prepare("SELECT * FROM test_table WHERE id = ?", "get_by_id");
    
    // Больше параметров, чем нужно
    EXPECT_THROW({
        _connection->exec_prepared({ "1", "2" }, "get_by_id");
    }, std::invalid_argument);
}

/**
 * @brief Тест обработки разных типов данных - INTEGER
 */
TEST_F(SqliteAdapter, Connection_DataTypes_Integer)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, value INTEGER)");
    _connection->exec("INSERT INTO test_table (value) VALUES (42)");
    _connection->exec("INSERT INTO test_table (value) VALUES (-100)");
    _connection->exec("INSERT INTO test_table (value) VALUES (0)");
    
    auto result = _connection->exec("SELECT value FROM test_table ORDER BY id");
    
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result.at(0).at("value"), "42");
    EXPECT_EQ(result.at(1).at("value"), "-100");
    EXPECT_EQ(result.at(2).at("value"), "0");
}

/**
 * @brief Тест обработки разных типов данных - REAL
 */
TEST_F(SqliteAdapter, Connection_DataTypes_Real)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, value REAL)");
    _connection->exec("INSERT INTO test_table (value) VALUES (3.14)");
    _connection->exec("INSERT INTO test_table (value) VALUES (-2.5)");
    _connection->exec("INSERT INTO test_table (value) VALUES (0.0)");
    
    auto result = _connection->exec("SELECT value FROM test_table ORDER BY id");
    
    EXPECT_EQ(result.size(), 3);
    // Проверяем, что значения преобразованы в строки
    EXPECT_NE(result.at(0).at("value").find("3.14"), std::string::npos);
    EXPECT_NE(result.at(1).at("value").find("-2.5"), std::string::npos);
}

/**
 * @brief Тест обработки разных типов данных - TEXT
 */
TEST_F(SqliteAdapter, Connection_DataTypes_Text)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, value TEXT)");
    _connection->exec("INSERT INTO test_table (value) VALUES ('Hello World')");
    _connection->exec("INSERT INTO test_table (value) VALUES ('Test with ''quotes''')");
    _connection->exec("INSERT INTO test_table (value) VALUES ('')");
    
    auto result = _connection->exec("SELECT value FROM test_table ORDER BY id");
    
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result.at(0).at("value"), "Hello World");
    EXPECT_EQ(result.at(1).at("value"), "Test with 'quotes'");
    EXPECT_EQ(result.at(2).at("value"), "");
}

/**
 * @brief Тест обработки разных типов данных - BLOB
 */
TEST_F(SqliteAdapter, Connection_DataTypes_Blob)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, value BLOB)");
    
    // Создаем тестовые байты (без нулевых символов для простоты)
    std::string blob_data = "Hello World Test";
    
    // Используем подготовленный запрос для вставки BLOB
    _connection->prepare("INSERT INTO test_table (value) VALUES (?)", "insert_blob");
    // SQLite автоматически преобразует строку в BLOB при необходимости
    _connection->exec_prepared({ blob_data }, "insert_blob");
    
    auto result = _connection->exec("SELECT value FROM test_table WHERE id = 1");
    
    EXPECT_FALSE(result.empty());
    // Проверяем, что BLOB данные сохранены и прочитаны
    const std::string& retrieved_blob = result.at(0).at("value");
    EXPECT_EQ(retrieved_blob.size(), blob_data.size());
    EXPECT_EQ(retrieved_blob, blob_data);
}

/**
 * @brief Тест обработки разных типов данных - NULL
 */
TEST_F(SqliteAdapter, Connection_DataTypes_Null)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, value TEXT)");
    _connection->exec("INSERT INTO test_table (value) VALUES (NULL)");
    _connection->exec("INSERT INTO test_table (value) VALUES ('Not NULL')");
    
    auto result = _connection->exec("SELECT value FROM test_table ORDER BY id");
    
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0).at("value"), NULL_VALUE);
    EXPECT_EQ(result.at(1).at("value"), "Not NULL");
}

/**
 * @brief Тест автоматического определения типа параметра - INTEGER
 */
TEST_F(SqliteAdapter, Connection_BindParameter_Integer)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, value INTEGER)");
    
    _connection->prepare("INSERT INTO test_table (value) VALUES (?)", "insert_int");
    
    // Передаем строку, которая выглядит как целое число
    _connection->exec_prepared({ "42" }, "insert_int");
    _connection->exec_prepared({ "-100" }, "insert_int");
    
    auto result = _connection->exec("SELECT value FROM test_table ORDER BY id");
    
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0).at("value"), "42");
    EXPECT_EQ(result.at(1).at("value"), "-100");
}

/**
 * @brief Тест автоматического определения типа параметра - REAL
 */
TEST_F(SqliteAdapter, Connection_BindParameter_Real)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, value REAL)");
    
    _connection->prepare("INSERT INTO test_table (value) VALUES (?)", "insert_real");
    
    // Передаем строку, которая выглядит как вещественное число
    _connection->exec_prepared({ "3.14" }, "insert_real");
    _connection->exec_prepared({ "-2.5" }, "insert_real");
    
    auto result = _connection->exec("SELECT value FROM test_table ORDER BY id");
    
    EXPECT_EQ(result.size(), 2);
    // Проверяем, что значения сохранены
    EXPECT_NE(result.at(0).at("value").find("3.14"), std::string::npos);
    EXPECT_NE(result.at(1).at("value").find("-2.5"), std::string::npos);
}

/**
 * @brief Тест автоматического определения типа параметра - TEXT
 */
TEST_F(SqliteAdapter, Connection_BindParameter_Text)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, value TEXT)");
    
    _connection->prepare("INSERT INTO test_table (value) VALUES (?)", "insert_text");
    
    // Передаем строку, которая не является числом
    _connection->exec_prepared({ "Hello World" }, "insert_text");
    _connection->exec_prepared({ "Test with 'quotes'" }, "insert_text");
    
    auto result = _connection->exec("SELECT value FROM test_table ORDER BY id");
    
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0).at("value"), "Hello World");
    EXPECT_EQ(result.at(1).at("value"), "Test with 'quotes'");
}

/**
 * @brief Тест открытия транзакции
 */
TEST_F(SqliteAdapter, Connection_OpenTransaction)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, name TEXT)");
    
    bool result = _connection->open_transaction(database_adapter::transaction_isolation_level::DEFAULT);
    EXPECT_TRUE(result);
    EXPECT_TRUE(_connection->is_transaction());
    
    _connection->exec("INSERT INTO test_table (name) VALUES ('Test1')");
    _connection->commit();
    
    EXPECT_FALSE(_connection->is_transaction());
    
    auto query_result = _connection->exec("SELECT name FROM test_table");
    EXPECT_FALSE(query_result.empty());
    EXPECT_EQ(query_result.at(0).at("name"), "Test1");
}

/**
 * @brief Тест открытия транзакции с разными уровнями изоляции
 */
TEST_F(SqliteAdapter, Connection_OpenTransaction_IsolationLevels)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY)");
    
    // Тестируем разные уровни изоляции
    EXPECT_TRUE(_connection->open_transaction(database_adapter::transaction_isolation_level::READ_UNCOMMITTED));
    _connection->rollback();
    
    EXPECT_TRUE(_connection->open_transaction(database_adapter::transaction_isolation_level::READ_COMMITTED));
    _connection->rollback();
    
    EXPECT_TRUE(_connection->open_transaction(database_adapter::transaction_isolation_level::REPEATABLE_READ));
    _connection->rollback();
    
    EXPECT_TRUE(_connection->open_transaction(database_adapter::transaction_isolation_level::SERIALIZABLE));
    _connection->rollback();
}

/**
 * @brief Тест commit транзакции
 */
TEST_F(SqliteAdapter, Connection_CommitTransaction)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, name TEXT)");
    
    _connection->begin_transaction();
    _connection->exec("INSERT INTO test_table (name) VALUES ('Test1')");
    _connection->exec("INSERT INTO test_table (name) VALUES ('Test2')");
    _connection->commit();
    
    EXPECT_FALSE(_connection->is_transaction());
    
    auto result = _connection->exec("SELECT COUNT(*) as count FROM test_table");
    EXPECT_EQ(result.at(0).at("count"), "2");
}

/**
 * @brief Тест rollback транзакции
 */
TEST_F(SqliteAdapter, Connection_RollbackTransaction)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, name TEXT)");
    
    _connection->begin_transaction();
    _connection->exec("INSERT INTO test_table (name) VALUES ('Test1')");
    _connection->exec("INSERT INTO test_table (name) VALUES ('Test2')");
    _connection->rollback();
    
    EXPECT_FALSE(_connection->is_transaction());
    
    auto result = _connection->exec("SELECT COUNT(*) as count FROM test_table");
    EXPECT_EQ(result.at(0).at("count"), "0");
}

/**
 * @brief Тест commit без активной транзакции
 */
TEST_F(SqliteAdapter, Connection_CommitWithoutTransaction)
{
    EXPECT_THROW(_connection->commit(), database_adapter::sql_exception);
}

/**
 * @brief Тест rollback без активной транзакции
 */
TEST_F(SqliteAdapter, Connection_RollbackWithoutTransaction)
{
    EXPECT_THROW(_connection->rollback(), database_adapter::sql_exception);
}

/**
 * @brief Тест добавления savepoint
 */
TEST_F(SqliteAdapter, Connection_AddSavepoint)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, name TEXT)");
    
    _connection->begin_transaction();
    _connection->exec("INSERT INTO test_table (name) VALUES ('Test1')");
    
    EXPECT_NO_THROW({
        _connection->add_save_point("sp1");
    });
    
    _connection->exec("INSERT INTO test_table (name) VALUES ('Test2')");
    _connection->rollback_to_save_point("sp1");
    
    _connection->commit();
    
    auto result = _connection->exec("SELECT COUNT(*) as count FROM test_table");
    EXPECT_EQ(result.at(0).at("count"), "1");
    EXPECT_EQ(_connection->exec("SELECT name FROM test_table").at(0).at("name"), "Test1");
}

/**
 * @brief Тест добавления savepoint без транзакции
 */
TEST_F(SqliteAdapter, Connection_AddSavepointWithoutTransaction)
{
    EXPECT_THROW({
        _connection->add_save_point("sp1");
    }, database_adapter::sql_exception);
}

/**
 * @brief Тест rollback to savepoint без транзакции
 */
TEST_F(SqliteAdapter, Connection_RollbackToSavepointWithoutTransaction)
{
    EXPECT_THROW({
        _connection->rollback_to_save_point("sp1");
    }, database_adapter::sql_exception);
}

/**
 * @brief Тест обработки ошибок - неверный SQL
 */
TEST_F(SqliteAdapter, Connection_ErrorHandling_InvalidSQL)
{
    EXPECT_THROW({
        _connection->exec("INVALID SQL STATEMENT");
    }, database_adapter::sql_exception);
}

/**
 * @brief Тест обработки ошибок - несуществующая таблица
 */
TEST_F(SqliteAdapter, Connection_ErrorHandling_NonExistentTable)
{
    EXPECT_THROW({
        _connection->exec("SELECT * FROM non_existent_table");
    }, database_adapter::sql_exception);
}

/**
 * @brief Тест обработки ошибок - неверный prepared statement
 */
TEST_F(SqliteAdapter, Connection_ErrorHandling_InvalidPreparedStatement)
{
    EXPECT_THROW({
        _connection->prepare("INVALID SQL STATEMENT", "invalid_query");
    }, database_adapter::sql_exception);
}

/**
 * @brief Тест потокобезопасности - выполнение запросов из разных потоков
 */
TEST_F(SqliteAdapter, Connection_ThreadSafety_ConcurrentQueries)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, name TEXT)");
    
    std::vector<std::thread> threads;
    std::atomic<int> success_count { 0 };
    const int thread_count = 10;
    
    for(int i = 0; i < thread_count; ++i) {
        threads.emplace_back([this, i, &success_count]() {
            try {
                std::string query = "INSERT INTO test_table (name) VALUES ('Thread" + std::to_string(i) + "')";
                _connection->exec(query);
                ++success_count;
            } catch(const std::exception&) {
                // Игнорируем ошибки для этого теста
            }
        });
    }
    
    for(auto& t : threads) {
        t.join();
    }
    
    // Проверяем, что все вставки выполнены
    auto result = _connection->exec("SELECT COUNT(*) as count FROM test_table");
    EXPECT_EQ(std::stoi(result.at(0).at("count")), thread_count);
}

/**
 * @brief Тест потокобезопасности - подготовленные запросы
 */
TEST_F(SqliteAdapter, Connection_ThreadSafety_PreparedQueries)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, name TEXT)");
    _connection->prepare("INSERT INTO test_table (name) VALUES (?)", "insert_name");
    
    std::vector<std::thread> threads;
    std::atomic<int> success_count { 0 };
    const int thread_count = 10;
    
    for(int i = 0; i < thread_count; ++i) {
        threads.emplace_back([this, i, &success_count]() {
            try {
                _connection->exec_prepared({ "Thread" + std::to_string(i) }, "insert_name");
                ++success_count;
            } catch(...) {
                // Игнорируем ошибки для этого теста
            }
        });
    }
    
    for(auto& t : threads) {
        t.join();
    }
    
    // Проверяем, что все вставки выполнены
    auto result = _connection->exec("SELECT COUNT(*) as count FROM test_table");
    EXPECT_EQ(std::stoi(result.at(0).at("count")), thread_count);
}

/**
 * @brief Тест проверки валидности соединения
 */
TEST_F(SqliteAdapter, Connection_IsValid)
{
    EXPECT_TRUE(_connection->is_valid());
    
    // Выполняем простой запрос
    auto result = _connection->exec("SELECT 1 as value");
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.at(0).at("value"), "1");
}

/**
 * @brief Тест экранирования специальных символов в prepared statements
 */
TEST_F(SqliteAdapter, Connection_Escaping_SpecialCharacters)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, value TEXT)");
    
    _connection->prepare("INSERT INTO test_table (value) VALUES (?)", "insert_value");
    
    // Тестируем различные специальные символы
    std::vector<std::string> test_values = {
        "Test with 'quotes'",
        "Test with \"double quotes\"",
        "Test with \\backslash",
        "Test with\nnewline",
        "Test with\ttab",
        "Test with NULL\0character"
    };
    
    for(const auto& value : test_values) {
        EXPECT_NO_THROW({
            _connection->exec_prepared({ value }, "insert_value");
        });
    }
    
    auto result = _connection->exec("SELECT COUNT(*) as count FROM test_table");
    EXPECT_EQ(std::stoi(result.at(0).at("count")), static_cast<int>(test_values.size()));
}

/**
 * @brief Тест последовательности операций
 */
TEST_F(SqliteAdapter, Connection_OperationSequence)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, name TEXT, value INTEGER)");
    
    _connection->begin_transaction();
    
    _connection->exec("INSERT INTO test_table (name, value) VALUES ('Test1', 100)");
    _connection->exec("INSERT INTO test_table (name, value) VALUES ('Test2', 200)");
    
    _connection->add_save_point("sp1");
    
    _connection->exec("INSERT INTO test_table (name, value) VALUES ('Test3', 300)");
    
    _connection->rollback_to_save_point("sp1");
    
    _connection->commit();
    
    auto result = _connection->exec("SELECT COUNT(*) as count FROM test_table");
    EXPECT_EQ(result.at(0).at("count"), "2");
}

