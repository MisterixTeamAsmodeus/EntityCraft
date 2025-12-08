#include "querycraft.hpp"

/**
 * @brief Тест кавычек идентификаторов
 */
TEST_F(QueryCraft, PostgresDialect_QuoteIdentifier)
{
    std::string result = postgres_dialect_->quote_identifier("users");
    EXPECT_EQ(result, "\"users\"");

    result = postgres_dialect_->quote_identifier("user_name");
    EXPECT_EQ(result, "\"user_name\"");

    result = postgres_dialect_->quote_identifier("User Name");
    EXPECT_EQ(result, "\"User Name\"");
}

/**
 * @brief Тест плейсхолдеров
 */
TEST_F(QueryCraft, PostgresDialect_Placeholder)
{
    std::string result = postgres_dialect_->placeholder(1);
    EXPECT_EQ(result, "$1");

    result = postgres_dialect_->placeholder(2);
    EXPECT_EQ(result, "$2");

    result = postgres_dialect_->placeholder(10);
    EXPECT_EQ(result, "$10");
}

/**
 * @brief Тест форматирования LIMIT/OFFSET
 */
TEST_F(QueryCraft, PostgresDialect_FormatLimitOffset)
{
    std::string result = postgres_dialect_->format_limit_offset(10, 0);
    EXPECT_EQ(result, " LIMIT 10");

    result = postgres_dialect_->format_limit_offset(10, 20);
    EXPECT_EQ(result, " LIMIT 10 OFFSET 20");

    result = postgres_dialect_->format_limit_offset(0, 0);
    EXPECT_TRUE(result.empty());
}

/**
 * @brief Тест поддержки RETURNING
 */
TEST_F(QueryCraft, PostgresDialect_SupportsReturning)
{
    EXPECT_TRUE(postgres_dialect_->supports_returning());
}

/**
 * @brief Тест имен функций
 */
TEST_F(QueryCraft, PostgresDialect_FunctionName)
{
    std::string result = postgres_dialect_->function_name("COUNT");
    EXPECT_EQ(result, "COUNT");

    result = postgres_dialect_->function_name("custom_function");
    EXPECT_EQ(result, "custom_function");
}

/**
 * @brief Тест пользовательских бинарных операторов
 */
TEST_F(QueryCraft, PostgresDialect_CustomBinaryOperator)
{
    std::string result = postgres_dialect_->custom_binary_operator("json_get");
    EXPECT_EQ(result, "json_get");
}
