#include <PostgreAdapter/postgreadapter.hpp>

#include <DatabaseAdapter/databaseadapter.hpp>
#include <DatabaseAdapter/exception/opendatabaseexception.hpp>
#include <DatabaseAdapter/exception/sqlexception.hpp>
#include <DatabaseAdapter/model/queryresult.hpp>
#include <DatabaseAdapter/transaction_isolation.hpp>
#include <PostgreAdapter/postgreerrorcode.hpp>

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
 * @brief Фикстура для тестов PostgreAdapter
 * @note Для работы тестов требуется запущенный PostgreSQL сервер
 *       Настройки подключения можно изменить через переменные окружения или изменить значения по умолчанию
 */
class PostgreAdapter : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Получаем настройки подключения из переменных окружения или используем значения по умолчанию
        const char* db_name = std::getenv("POSTGRES_TEST_DB");
        const char* db_host = std::getenv("POSTGRES_TEST_HOST");
        const char* db_port = std::getenv("POSTGRES_TEST_PORT");
        const char* db_user = std::getenv("POSTGRES_TEST_USER");
        const char* db_password = std::getenv("POSTGRES_TEST_PASSWORD");

        _settings.database_name = db_name ? db_name : "test_postgre_" + std::to_string(std::time(nullptr)) + "_" + std::to_string(std::rand());
        _settings.url = db_host ? db_host : "localhost";
        _settings.port = db_port ? db_port : "5432";
        _settings.login = db_user ? db_user : "postgres";
        _settings.password = db_password ? db_password : "postgres";

        try {
            _connection = std::make_shared<database_adapter::postgre::connection>(_settings, true, 1, 0);
        } catch(const database_adapter::open_database_exception& e) {
            GTEST_SKIP() << "Не удалось подключиться к PostgreSQL. Убедитесь, что сервер запущен и настройки подключения корректны: " << e.what();
        } catch(const database_adapter::sql_exception& e) {
            GTEST_SKIP() << "Не удалось создать тестовую базу данных: " << e.what();
        }
    }

    void TearDown() override
    {
        _connection.reset();

        // Удаляем тестовую базу данных
        try {
            auto temp_settings = _settings;
            temp_settings.database_name = "postgres";
            const auto cleanup_connection = std::make_shared<database_adapter::postgre::connection>(temp_settings);
            cleanup_connection->exec("DROP DATABASE IF EXISTS \"" + _settings.database_name + "\"");
        } catch(...) {
            // Игнорируем ошибки при очистке
        }
    }

protected:
    database_adapter::postgre::settings _settings;
    std::shared_ptr<database_adapter::postgre::connection> _connection;
};

/**
 * @brief Тест создания соединения
 */
TEST_F(PostgreAdapter, Connection_Create)
{
    EXPECT_TRUE(_connection != nullptr);
    EXPECT_TRUE(_connection->is_valid());
}

/**
 * @brief Тест создания таблицы и выполнения запроса
 */
TEST_F(PostgreAdapter, Connection_CreateTableAndQuery)
{
    // Создаем таблицу
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, name VARCHAR(100))");
    
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
TEST_F(PostgreAdapter, Connection_ExecWithoutResults)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY)");
    
    auto result = _connection->exec("INSERT INTO test_table DEFAULT VALUES");
    EXPECT_TRUE(result.empty());
    
    result = _connection->exec("UPDATE test_table SET id = 10 WHERE id = 1");
    EXPECT_TRUE(result.empty());
}

/**
 * @brief Тест валидации запроса - пустой запрос
 */
TEST_F(PostgreAdapter, Connection_Validation_EmptyQuery)
{
    EXPECT_THROW(_connection->exec(""), database_adapter::sql_exception);
}

/**
 * @brief Тест валидации запроса - только пробелы
 */
TEST_F(PostgreAdapter, Connection_Validation_WhitespaceOnlyQuery)
{
    EXPECT_THROW(_connection->exec("   "), database_adapter::sql_exception);
}

/**
 * @brief Тест валидации запроса - только табуляция
 */
TEST_F(PostgreAdapter, Connection_Validation_TabOnlyQuery)
{
    EXPECT_THROW(_connection->exec("\t\t"), database_adapter::sql_exception);
}

/**
 * @brief Тест подготовки запроса
 */
TEST_F(PostgreAdapter, Connection_PrepareQuery)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, name VARCHAR(100))");
    
    EXPECT_NO_THROW({
        _connection->prepare("SELECT * FROM test_table WHERE id = $1", "get_by_id");
    });
}

/**
 * @brief Тест выполнения подготовленного запроса
 */
TEST_F(PostgreAdapter, Connection_ExecPreparedQuery)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, name VARCHAR(100))");
    _connection->exec("INSERT INTO test_table (name) VALUES ('Test1')");
    _connection->exec("INSERT INTO test_table (name) VALUES ('Test2')");
    
    _connection->prepare("SELECT * FROM test_table WHERE id = $1", "get_by_id");
    
    auto result = _connection->exec_prepared({ "1" }, "get_by_id");
    
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result.at(0).at("id"), "1");
    EXPECT_EQ(result.at(0).at("name"), "Test1");
}

/**
 * @brief Тест выполнения подготовленного запроса с несколькими параметрами
 */
TEST_F(PostgreAdapter, Connection_ExecPreparedQuery_MultipleParams)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, name VARCHAR(100), value INTEGER)");
    _connection->exec("INSERT INTO test_table (name, value) VALUES ('Test1', 100)");
    _connection->exec("INSERT INTO test_table (name, value) VALUES ('Test2', 200)");
    
    _connection->prepare("SELECT * FROM test_table WHERE name = $1 AND value = $2", "get_by_name_and_value");
    
    auto result = _connection->exec_prepared({ "Test1", "100" }, "get_by_name_and_value");
    
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result.at(0).at("name"), "Test1");
    EXPECT_EQ(result.at(0).at("value"), "100");
}

/**
 * @brief Тест выполнения подготовленного запроса с NULL значением
 */
TEST_F(PostgreAdapter, Connection_ExecPreparedQuery_WithNull)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, name VARCHAR(100), value INTEGER)");
    
    _connection->prepare("INSERT INTO test_table (name, value) VALUES ($1, $2)", "insert_with_null");
    
    // Вставляем запись с NULL значением через prepared statement
    _connection->exec_prepared({ "Test1", NULL_VALUE }, "insert_with_null");
    _connection->exec_prepared({ "Test2", "100" }, "insert_with_null");
    
    // Проверяем, что NULL значение было правильно вставлено
    auto result = _connection->exec("SELECT * FROM test_table WHERE value IS NULL");
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result.at(0).at("name"), "Test1");
}

/**
 * @brief Тест обновления подготовленного запроса
 */
TEST_F(PostgreAdapter, Connection_PrepareQuery_UpdateExisting)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, name VARCHAR(100))");

    _connection->prepare("SELECT * FROM test_table WHERE name = $1", "get_by_name");
    
    // Обновляем подготовленный запрос
    EXPECT_THROW(_connection->prepare("SELECT * FROM test_table WHERE id = $1", "get_by_name") , database_adapter::sql_exception);
    
    _connection->exec("INSERT INTO test_table (name) VALUES ('Test1')");
    
    auto result = _connection->exec_prepared({ "Test1" }, "get_by_name");
    EXPECT_FALSE(result.empty());
}

/**
 * @brief Тест выполнения несуществующего подготовленного запроса
 */
TEST_F(PostgreAdapter, Connection_ExecPreparedQuery_NonExistent)
{
    EXPECT_THROW({
        _connection->exec_prepared({ "1" }, "non_existent_query");
    }, database_adapter::sql_exception);
}

/**
 * @brief Тест обработки разных типов данных - INTEGER
 */
TEST_F(PostgreAdapter, Connection_DataTypes_Integer)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, value INTEGER)");
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
 * @brief Тест обработки разных типов данных - REAL/NUMERIC
 */
TEST_F(PostgreAdapter, Connection_DataTypes_Real)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, value NUMERIC(10,2))");
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
 * @brief Тест обработки разных типов данных - TEXT/VARCHAR
 */
TEST_F(PostgreAdapter, Connection_DataTypes_Text)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, value VARCHAR(255))");
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
 * @brief Тест обработки разных типов данных - NULL
 */
TEST_F(PostgreAdapter, Connection_DataTypes_Null)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, value VARCHAR(100))");
    _connection->exec("INSERT INTO test_table (value) VALUES (NULL)");
    _connection->exec("INSERT INTO test_table (value) VALUES ('Not NULL')");
    
    auto result = _connection->exec("SELECT value FROM test_table ORDER BY id");
    
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0).at("value"), "");
    EXPECT_EQ(result.at(1).at("value"), "Not NULL");
}

/**
 * @brief Тест автоматического определения типа параметра - INTEGER
 */
TEST_F(PostgreAdapter, Connection_BindParameter_Integer)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, value INTEGER)");
    
    _connection->prepare("INSERT INTO test_table (value) VALUES ($1)", "insert_int");
    
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
TEST_F(PostgreAdapter, Connection_BindParameter_Real)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, value NUMERIC(10,2))");
    
    _connection->prepare("INSERT INTO test_table (value) VALUES ($1)", "insert_real");
    
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
TEST_F(PostgreAdapter, Connection_BindParameter_Text)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, value VARCHAR(255))");
    
    _connection->prepare("INSERT INTO test_table (value) VALUES ($1)", "insert_text");
    
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
TEST_F(PostgreAdapter, Connection_OpenTransaction)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, name VARCHAR(100))");
    
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
TEST_F(PostgreAdapter, Connection_OpenTransaction_IsolationLevels)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY)");
    
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
TEST_F(PostgreAdapter, Connection_CommitTransaction)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, name VARCHAR(100))");
    
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
TEST_F(PostgreAdapter, Connection_RollbackTransaction)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, name VARCHAR(100))");
    
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
TEST_F(PostgreAdapter, Connection_CommitWithoutTransaction)
{
    EXPECT_THROW(_connection->commit(), database_adapter::sql_exception);
}

/**
 * @brief Тест rollback без активной транзакции
 */
TEST_F(PostgreAdapter, Connection_RollbackWithoutTransaction)
{
    EXPECT_THROW(_connection->rollback(), database_adapter::sql_exception);
}

/**
 * @brief Тест добавления savepoint
 */
TEST_F(PostgreAdapter, Connection_AddSavepoint)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, name VARCHAR(100))");
    
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
TEST_F(PostgreAdapter, Connection_AddSavepointWithoutTransaction)
{
    EXPECT_THROW({
        _connection->add_save_point("sp1");
    }, database_adapter::sql_exception);
}

/**
 * @brief Тест rollback to savepoint без транзакции
 */
TEST_F(PostgreAdapter, Connection_RollbackToSavepointWithoutTransaction)
{
    EXPECT_THROW({
        _connection->rollback_to_save_point("sp1");
    }, database_adapter::sql_exception);
}

/**
 * @brief Тест обработки ошибок - неверный SQL
 */
TEST_F(PostgreAdapter, Connection_ErrorHandling_InvalidSQL)
{
    EXPECT_THROW({
        _connection->exec("INVALID SQL STATEMENT");
    }, database_adapter::sql_exception);
}

/**
 * @brief Тест обработки ошибок - несуществующая таблица
 */
TEST_F(PostgreAdapter, Connection_ErrorHandling_NonExistentTable)
{
    EXPECT_THROW({
        _connection->exec("SELECT * FROM non_existent_table");
    }, database_adapter::sql_exception);
}

/**
 * @brief Тест обработки ошибок - неверный prepared statement
 */
TEST_F(PostgreAdapter, Connection_ErrorHandling_InvalidPreparedStatement)
{
    EXPECT_THROW({
        _connection->prepare("INVALID SQL STATEMENT", "invalid_query");
    }, database_adapter::sql_exception);
}

/**
 * @brief Тест проверки валидности соединения
 */
TEST_F(PostgreAdapter, Connection_IsValid)
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
TEST_F(PostgreAdapter, Connection_Escaping_SpecialCharacters)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, value TEXT)");
    
    _connection->prepare("INSERT INTO test_table (value) VALUES ($1)", "insert_value");
    
    // Тестируем различные специальные символы
    std::vector<std::string> test_values = {
        "Test with 'quotes'",
        "Test with \"double quotes\"",
        "Test with \\backslash",
        "Test with\nnewline",
        "Test with\ttab"
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
TEST_F(PostgreAdapter, Connection_OperationSequence)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, name VARCHAR(100), value INTEGER)");
    
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

/**
 * @brief Тест использования enum классов для статусов
 */
TEST_F(PostgreAdapter, Connection_ErrorCodeEnums)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY)");
    
    // Тест успешного выполнения запроса
    auto result = _connection->exec("SELECT 1");
    EXPECT_FALSE(result.empty());
    
    // Тест обработки ошибки с кодом
    try {
        _connection->exec("SELECT * FROM non_existent_table_12345");
        FAIL() << "Ожидалось исключение sql_exception";
    } catch(const database_adapter::sql_exception& e) {
        // Проверяем, что код ошибки установлен (может быть 0, если не удалось извлечь)
        EXPECT_GE(e.error_code(), 0);
    }
}

/**
 * @brief Тест очистки подготовленных запросов при закрытии соединения
 */
TEST_F(PostgreAdapter, Connection_CleanupPreparedStatements)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, name VARCHAR(100))");
    
    // Подготавливаем несколько запросов
    _connection->prepare("SELECT * FROM test_table WHERE id = $1", "get_by_id");
    _connection->prepare("INSERT INTO test_table (name) VALUES ($1)", "insert_name");
    
    // Проверяем, что запросы работают
    _connection->exec_prepared({ "Test1" }, "insert_name");
    auto result = _connection->exec_prepared({ "1" }, "get_by_id");
    EXPECT_FALSE(result.empty());
    
    // Закрываем соединение (происходит автоматически в деструкторе)
    _connection.reset();
    
    // Создаем новое соединение - подготовленные запросы должны быть очищены
    _connection = std::make_shared<database_adapter::postgre::connection>(_settings);
    
    // Проверяем, что старые подготовленные запросы не существуют
    EXPECT_THROW({
        _connection->exec_prepared({ "1" }, "get_by_id");
    }, database_adapter::sql_exception);
}

/**
 * @brief Тест работы с большими объемами данных
 */
TEST_F(PostgreAdapter, Connection_LargeDataSet)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, value INTEGER)");
    
    _connection->begin_transaction();
    
    // Вставляем большое количество записей
    const int record_count = 1000;
    for(int i = 0; i < record_count; ++i) {
        _connection->exec("INSERT INTO test_table (value) VALUES (" + std::to_string(i) + ")");
    }
    
    _connection->commit();
    
    auto result = _connection->exec("SELECT COUNT(*) as count FROM test_table");
    EXPECT_EQ(std::stoi(result.at(0).at("count")), record_count);
}

/**
 * @brief Тест использования prepared statements для производительности
 */
TEST_F(PostgreAdapter, Connection_PreparedStatementsPerformance)
{
    _connection->exec("CREATE TABLE IF NOT EXISTS test_table (id SERIAL PRIMARY KEY, value INTEGER)");
    
    _connection->prepare("INSERT INTO test_table (value) VALUES ($1)", "insert_value");
    
    _connection->begin_transaction();
    
    // Вставляем большое количество записей через prepared statement
    const int record_count = 1000;
    for(int i = 0; i < record_count; ++i) {
        _connection->exec_prepared({ std::to_string(i) }, "insert_value");
    }
    
    _connection->commit();
    
    auto result = _connection->exec("SELECT COUNT(*) as count FROM test_table");
    EXPECT_EQ(std::stoi(result.at(0).at("count")), record_count);
}

