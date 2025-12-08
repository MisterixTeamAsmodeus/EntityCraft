#include "querycraft.hpp"

/**
 * @brief Тест кавычек идентификаторов
 */
TEST_F(QueryCraft, SqliteDialect_QuoteIdentifier)
{
    std::string result = sqlite_dialect_->quote_identifier("users");
    EXPECT_EQ(result, "\"users\"");

    result = sqlite_dialect_->quote_identifier("user_name");
    EXPECT_EQ(result, "\"user_name\"");

    result = sqlite_dialect_->quote_identifier("User Name");
    EXPECT_EQ(result, "\"User Name\"");
}

/**
 * @brief Тест плейсхолдеров
 */
TEST_F(QueryCraft, SqliteDialect_Placeholder)
{
    std::string result = sqlite_dialect_->placeholder(1);
    EXPECT_EQ(result, "?");

    result = sqlite_dialect_->placeholder(2);
    EXPECT_EQ(result, "?");

    result = sqlite_dialect_->placeholder(10);
    EXPECT_EQ(result, "?");
}

/**
 * @brief Тест форматирования LIMIT/OFFSET
 */
TEST_F(QueryCraft, SqliteDialect_FormatLimitOffset)
{
    std::string result = sqlite_dialect_->format_limit_offset(10, 0);
    EXPECT_EQ(result, " LIMIT 10");

    result = sqlite_dialect_->format_limit_offset(10, 20);
    EXPECT_EQ(result, " LIMIT 10 OFFSET 20");

    result = sqlite_dialect_->format_limit_offset(0, 0);
    EXPECT_TRUE(result.empty());
}

/**
 * @brief Тест поддержки RETURNING
 */
TEST_F(QueryCraft, SqliteDialect_SupportsReturning)
{
    // SQLite поддерживает RETURNING начиная с версии 3.35.0
    // В текущей реализации возвращается true
    EXPECT_TRUE(sqlite_dialect_->supports_returning());
}

/**
 * @brief Тест имен функций
 */
TEST_F(QueryCraft, SqliteDialect_FunctionName)
{
    std::string result = sqlite_dialect_->function_name("COUNT");
    EXPECT_EQ(result, "COUNT");

    result = sqlite_dialect_->function_name("custom_function");
    EXPECT_EQ(result, "custom_function");
}

/**
 * @brief Тест пользовательских бинарных операторов
 */
TEST_F(QueryCraft, SqliteDialect_CustomBinaryOperator)
{
    std::string result = sqlite_dialect_->custom_binary_operator("json_get");
    EXPECT_EQ(result, "json_get");
}
