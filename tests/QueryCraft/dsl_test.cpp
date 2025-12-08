#include "querycraft.hpp"

/**
 * @brief Тест функции col()
 */
TEST_F(QueryCraft, Dsl_Col)
{
    using namespace query_craft::dsl;

    query_craft::ast::expression expr = col("name");
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::identifier);
    EXPECT_EQ(expr.identifier_value.name, "name");
    EXPECT_TRUE(expr.identifier_value.alias.empty());
    EXPECT_TRUE(expr.identifier_value.schema.empty());

    expr = col("id", "users");
    EXPECT_EQ(expr.identifier_value.name, "id");
    EXPECT_EQ(expr.identifier_value.alias, "users");

    expr = col("name", "u", "public");
    EXPECT_EQ(expr.identifier_value.name, "name");
    EXPECT_EQ(expr.identifier_value.alias, "u");
    EXPECT_EQ(expr.identifier_value.schema, "public");
}

/**
 * @brief Тест функции value()
 */
TEST_F(QueryCraft, Dsl_Value)
{
    using namespace query_craft::dsl;

    query_craft::ast::expression expr = value(42);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::literal);
    EXPECT_EQ(expr.literal_value.value, "42");

    expr = value(std::string("test"));
    EXPECT_EQ(expr.literal_value.value, "test");

    expr = value(true);
    EXPECT_EQ(expr.literal_value.value, "true");
}

/**
 * @brief Тест функции param()
 */
TEST_F(QueryCraft, Dsl_Param)
{
    using namespace query_craft::dsl;
    query_craft::ast::expression expr = param(42);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::parameter);
    EXPECT_EQ(expr.parameter_value.value, "42");

    expr = param(std::string("test"));
    EXPECT_EQ(expr.parameter_value.value, "test");

    expr = param(true);
    EXPECT_EQ(expr.parameter_value.value, "true");
}

/**
 * @brief Тест функции func()
 */
TEST_F(QueryCraft, Dsl_Func)
{
    using namespace query_craft::dsl;
    query_craft::ast::expression expr = func("COUNT", { col("id") });
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::function_call);
    EXPECT_NE(expr.function_call_value, nullptr);
    EXPECT_EQ(expr.function_call_value->name, "COUNT");
    EXPECT_EQ(expr.function_call_value->arguments.size(), 1);

    expr = func("CONCAT", { col("first_name"), col("last_name") });
    EXPECT_EQ(expr.function_call_value->name, "CONCAT");
    EXPECT_EQ(expr.function_call_value->arguments.size(), 2);
}

/**
 * @brief Тест агрегирующих функций
 */
TEST_F(QueryCraft, Dsl_AggregateFunctions)
{
    using namespace query_craft::dsl;
    query_craft::ast::expression expr = count({ col("id") });
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::function_call);
    EXPECT_EQ(expr.function_call_value->name, "COUNT");

    expr = sum({ col("amount") });
    EXPECT_EQ(expr.function_call_value->name, "SUM");

    expr = avg({ col("score") });
    EXPECT_EQ(expr.function_call_value->name, "AVG");

    expr = min({ col("price") });
    EXPECT_EQ(expr.function_call_value->name, "MIN");

    expr = max({ col("price") });
    EXPECT_EQ(expr.function_call_value->name, "MAX");
}

/**
 * @brief Тест функции custom_aggregate()
 */
TEST_F(QueryCraft, Dsl_CustomAggregate)
{
    using namespace query_craft::dsl;
    query_craft::ast::expression expr = custom_aggregate("json_array_agg_logical", { col("data") });
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::function_call);
    EXPECT_EQ(expr.function_call_value->name, "json_array_agg_logical");
}

/**
 * @brief Тест функции custom_op()
 */
TEST_F(QueryCraft, Dsl_CustomOp)
{
    using namespace query_craft::dsl;
    query_craft::ast::expression left = col("data");
    query_craft::ast::expression right = param(std::string("key"));

    query_craft::ast::expression expr = custom_op(left, "json_get", right);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::binary);
    EXPECT_EQ(expr.binary_value->op, query_craft::ast::binary_operator::custom);
    EXPECT_EQ(expr.binary_value->custom_operator_name, "json_get");
}

/**
 * @brief Тест функции subquery()
 */
TEST_F(QueryCraft, Dsl_Subquery)
{
    using namespace query_craft::dsl;
    query_craft::ast::select_query query;
    query.from.name = "users";
    query.columns.push_back(col("id"));

    query_craft::ast::expression expr = subquery(query);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::subquery);
    EXPECT_NE(expr.subquery_value, nullptr);
    EXPECT_EQ(expr.subquery_value->from.name, "users");
}

/**
 * @brief Тест функции and_()
 */
TEST_F(QueryCraft, Dsl_And)
{
    using namespace query_craft::dsl;
    query_craft::ast::expression expr = and_({ col("age") > param(18), col("active") == param(true) });
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::binary);
    EXPECT_EQ(expr.binary_value->op, query_craft::ast::binary_operator::and_);

    // Тест с одним условием
    expr = and_({ col("active") == param(true) });
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::binary);
    EXPECT_EQ(expr.binary_value->op, query_craft::ast::binary_operator::equals);

    // Тест с пустым списком
    expr = and_({});
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::literal);
}

/**
 * @brief Тест функции or_()
 */
TEST_F(QueryCraft, Dsl_Or)
{
    using namespace query_craft::dsl;
    query_craft::ast::expression expr = or_({ col("status") == param(std::string("active")), col("status") == param(std::string("pending")) });
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::binary);
    EXPECT_EQ(expr.binary_value->op, query_craft::ast::binary_operator::or_);

    // Тест с одним условием
    expr = or_({ col("active") == param(true) });
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::binary);
    EXPECT_EQ(expr.binary_value->op, query_craft::ast::binary_operator::equals);

    // Тест с пустым списком
    expr = or_({});
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::literal);
}
