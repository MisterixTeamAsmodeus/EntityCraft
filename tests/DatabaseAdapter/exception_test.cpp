#include "mock/databaseadapter.hpp"

#include <DatabaseAdapter/exception/opendatabaseexception.hpp>
#include <DatabaseAdapter/exception/sqlexception.hpp>
#include <gtest/gtest.h>

#include <exception>
#include <string>

/**
 * @brief Тест конструктора с сообщением
 */
TEST_F(DatabaseAdapter, Exception_Sql_ConstructorWithMessage)
{
    database_adapter::sql_exception ex("Ошибка выполнения запроса");
    EXPECT_STREQ(ex.what(), "Ошибка выполнения запроса");
    EXPECT_EQ(ex.last_query(), "");
    EXPECT_EQ(ex.error_code(), 0);
}

/**
 * @brief Тест конструктора с сообщением и запросом
 */
TEST_F(DatabaseAdapter, Exception_Sql_ConstructorWithMessageAndQuery)
{
    database_adapter::sql_exception ex("Ошибка выполнения запроса", "SELECT * FROM users");
    std::string what_str = ex.what();
    EXPECT_TRUE(what_str.find("Ошибка выполнения запроса") != std::string::npos);
    EXPECT_TRUE(what_str.find("SELECT * FROM users") != std::string::npos);
    EXPECT_EQ(ex.last_query(), "SELECT * FROM users");
    EXPECT_EQ(ex.error_code(), 0);
}

/**
 * @brief Тест конструктора с сообщением, запросом и кодом ошибки
 */
TEST_F(DatabaseAdapter, Exception_Sql_ConstructorWithMessageQueryAndErrorCode)
{
    database_adapter::sql_exception ex("Ошибка выполнения запроса", "SELECT * FROM users", 42);
    std::string what_str = ex.what();
    EXPECT_TRUE(what_str.find("Ошибка выполнения запроса") != std::string::npos);
    EXPECT_TRUE(what_str.find("SELECT * FROM users") != std::string::npos);
    EXPECT_TRUE(what_str.find("42") != std::string::npos);
    EXPECT_EQ(ex.last_query(), "SELECT * FROM users");
    EXPECT_EQ(ex.error_code(), 42);
}

/**
 * @brief Тест конструктора только с кодом ошибки
 */
TEST_F(DatabaseAdapter, Exception_Sql_ConstructorWithErrorCode)
{
    database_adapter::sql_exception ex("Ошибка", "", 100);
    std::string what_str = ex.what();
    EXPECT_TRUE(what_str.find("Ошибка") != std::string::npos);
    EXPECT_TRUE(what_str.find("100") != std::string::npos);
    EXPECT_EQ(ex.error_code(), 100);
}

/**
 * @brief Тест наследования от std::exception
 */
TEST_F(DatabaseAdapter, Exception_Sql_InheritsFromStdException)
{
    database_adapter::sql_exception ex("Тест");
    std::exception& base = ex;
    EXPECT_STREQ(base.what(), "Тест");
}

/**
 * @brief Тест копирования
 */
TEST_F(DatabaseAdapter, Exception_Sql_CopyConstructor)
{
    database_adapter::sql_exception ex1("Ошибка", "SELECT 1", 5);
    database_adapter::sql_exception ex2(ex1);

    EXPECT_STREQ(ex1.what(), ex2.what());
    EXPECT_EQ(ex1.last_query(), ex2.last_query());
    EXPECT_EQ(ex1.error_code(), ex2.error_code());
}

/**
 * @brief Тест перемещения
 */
TEST_F(DatabaseAdapter, Exception_Sql_MoveConstructor)
{
    database_adapter::sql_exception ex1("Ошибка", "SELECT 1", 5);
    database_adapter::sql_exception ex2(std::move(ex1));

    EXPECT_EQ(ex2.last_query(), "SELECT 1");
    EXPECT_EQ(ex2.error_code(), 5);
}

/**
 * @brief Тест присваивания
 */
TEST_F(DatabaseAdapter, Exception_Sql_AssignmentOperator)
{
    database_adapter::sql_exception ex1("Ошибка1", "SELECT 1", 1);
    database_adapter::sql_exception ex2("Ошибка2", "SELECT 2", 2);

    ex2 = ex1;
    EXPECT_STREQ(ex1.what(), ex2.what());
    EXPECT_EQ(ex1.last_query(), ex2.last_query());
    EXPECT_EQ(ex1.error_code(), ex2.error_code());
}

/**
 * @brief Тест конструктора с сообщением
 */
TEST_F(DatabaseAdapter, Exception_Open_ConstructorWithMessage)
{
    database_adapter::open_database_exception ex("Ошибка подключения к базе данных");
    EXPECT_STREQ(ex.what(), "Ошибка подключения к базе данных");
    EXPECT_EQ(ex.error_code(), 0);
}

/**
 * @brief Тест конструктора с сообщением и кодом ошибки
 */
TEST_F(DatabaseAdapter, Exception_Open_ConstructorWithMessageAndErrorCode)
{
    database_adapter::open_database_exception ex("Ошибка подключения", 404);
    std::string what_str = ex.what();
    EXPECT_TRUE(what_str.find("Ошибка подключения") != std::string::npos);
    EXPECT_TRUE(what_str.find("404") != std::string::npos);
    EXPECT_EQ(ex.error_code(), 404);
}

/**
 * @brief Тест наследования от std::exception
 */
TEST_F(DatabaseAdapter, Exception_Open_InheritsFromStdException)
{
    database_adapter::open_database_exception ex("Тест");
    std::exception& base = ex;
    EXPECT_STREQ(base.what(), "Тест");
}

/**
 * @brief Тест копирования
 */
TEST_F(DatabaseAdapter, Exception_Open_CopyConstructor)
{
    database_adapter::open_database_exception ex1("Ошибка", 500);
    database_adapter::open_database_exception ex2(ex1);

    EXPECT_STREQ(ex1.what(), ex2.what());
    EXPECT_EQ(ex1.error_code(), ex2.error_code());
}

/**
 * @brief Тест перемещения
 */
TEST_F(DatabaseAdapter, Exception_Open_MoveConstructor)
{
    database_adapter::open_database_exception ex1("Ошибка", 500);
    database_adapter::open_database_exception ex2(std::move(ex1));

    EXPECT_EQ(ex2.error_code(), 500);
}

/**
 * @brief Тест присваивания
 */
TEST_F(DatabaseAdapter, Exception_Open_AssignmentOperator)
{
    database_adapter::open_database_exception ex1("Ошибка1", 100);
    database_adapter::open_database_exception ex2("Ошибка2", 200);

    ex2 = ex1;
    EXPECT_STREQ(ex1.what(), ex2.what());
    EXPECT_EQ(ex1.error_code(), ex2.error_code());
}

/**
 * @brief Тест различных кодов ошибок
 */
TEST_F(DatabaseAdapter, Exception_Open_DifferentErrorCodes)
{
    database_adapter::open_database_exception ex1("Ошибка", 0);
    database_adapter::open_database_exception ex2("Ошибка", -1);
    database_adapter::open_database_exception ex3("Ошибка", 1000);

    EXPECT_EQ(ex1.error_code(), 0);
    EXPECT_EQ(ex2.error_code(), -1);
    EXPECT_EQ(ex3.error_code(), 1000);
}
