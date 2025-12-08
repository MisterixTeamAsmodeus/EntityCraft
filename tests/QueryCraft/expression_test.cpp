#include "querycraft.hpp"

/**
 * @brief Тест создания идентификатора
 */
TEST_F(QueryCraft, Expression_MakeIdentifier)
{
    using namespace query_craft::dsl;
    query_craft::ast::identifier id;
    id.name = "users";
    id.schema = "public";

    query_craft::ast::expression expr = query_craft::ast::expression::make_identifier(id);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::identifier);
    EXPECT_EQ(expr.identifier_value.name, "users");
    EXPECT_EQ(expr.identifier_value.schema, "public");
}

/**
 * @brief Тест создания литерала
 */
TEST_F(QueryCraft, Expression_MakeLiteral)
{
    using namespace query_craft::dsl;
    query_craft::ast::literal lit;
    lit.value = "test_value";

    query_craft::ast::expression expr = query_craft::ast::expression::make_literal(lit);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::literal);
    EXPECT_EQ(expr.literal_value.value, "test_value");
}

/**
 * @brief Тест создания параметра
 */
TEST_F(QueryCraft, Expression_MakeParameter)
{
    query_craft::ast::parameter param;
    param.value = "123";

    query_craft::ast::expression expr = query_craft::ast::expression::make_parameter(param);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::parameter);
    EXPECT_EQ(expr.parameter_value.value, "123");
}

/**
 * @brief Тест создания функции
 */
TEST_F(QueryCraft, Expression_MakeFunction)
{
    using namespace query_craft::dsl;

    query_craft::ast::function_call fn;
    fn.name = "COUNT";
    fn.arguments.push_back(col("id"));

    query_craft::ast::expression expr = query_craft::ast::expression::make_function(fn);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::function_call);
    EXPECT_NE(expr.function_call_value, nullptr);
    EXPECT_EQ(expr.function_call_value->name, "COUNT");
    EXPECT_EQ(expr.function_call_value->arguments.size(), 1);
}

/**
 * @brief Тест создания бинарного выражения
 */
TEST_F(QueryCraft, Expression_MakeBinary)
{
    query_craft::ast::expression left = query_craft::dsl::col("age");
    query_craft::ast::expression right = query_craft::dsl::param(18);

    query_craft::ast::expression expr = query_craft::ast::expression::make_binary(query_craft::ast::binary_operator::greater, left, right);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::binary);
    EXPECT_NE(expr.binary_value, nullptr);
    EXPECT_EQ(expr.binary_value->op, query_craft::ast::binary_operator::greater);
    EXPECT_NE(expr.binary_value->left, nullptr);
    EXPECT_NE(expr.binary_value->right, nullptr);
}

/**
 * @brief Тест создания унарного выражения
 */
TEST_F(QueryCraft, Expression_MakeUnary)
{
    query_craft::ast::expression operand = query_craft::dsl::col("active");

    query_craft::ast::expression expr = query_craft::ast::expression::make_unary(query_craft::ast::unary_operator::not_, operand);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::unary);
    EXPECT_NE(expr.unary_value, nullptr);
    EXPECT_EQ(expr.unary_value->op, query_craft::ast::unary_operator::not_);
    EXPECT_NE(expr.unary_value->operand, nullptr);
}

/**
 * @brief Тест создания подзапроса
 */
TEST_F(QueryCraft, Expression_MakeSubquery)
{
    using namespace query_craft::dsl;

    query_craft::ast::select_query subquery;
    subquery.from.name = "users";
    subquery.columns.push_back(col("id"));

    query_craft::ast::expression expr = query_craft::ast::expression::make_subquery(subquery);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::subquery);
    EXPECT_NE(expr.subquery_value, nullptr);
    EXPECT_EQ(expr.subquery_value->from.name, "users");
}

/**
 * @brief Тест копирования выражения
 */
TEST_F(QueryCraft, Expression_CopyExpression)
{
    using namespace query_craft::dsl;
    query_craft::ast::expression original = col("age") > param(18);
    query_craft::ast::expression copied = original;

    EXPECT_EQ(copied.type, original.type);
    EXPECT_EQ(copied.type, query_craft::ast::expression_type::binary);
    EXPECT_NE(copied.binary_value, nullptr);
    EXPECT_NE(original.binary_value, nullptr);
    // Проверяем, что это разные объекты
    EXPECT_NE(copied.binary_value.get(), original.binary_value.get());
}

/**
 * @brief Тест перемещения выражения
 */
TEST_F(QueryCraft, Expression_MoveExpression)
{
    using namespace query_craft::dsl;
    query_craft::ast::expression original = col("age") > param(18);
    auto original_ptr = original.binary_value.get();

    query_craft::ast::expression moved = std::move(original);
    EXPECT_EQ(moved.type, query_craft::ast::expression_type::binary);
    EXPECT_EQ(moved.binary_value.get(), original_ptr);
    // После перемещения оригинал должен быть в валидном, но пустом состоянии
    EXPECT_EQ(original.type, query_craft::ast::expression_type::literal);
}

/**
 * @brief Тест операторов сравнения через DSL
 */
TEST_F(QueryCraft, Expression_ComparisonOperators)
{
    using namespace query_craft::dsl;

    query_craft::ast::expression eq = col("age") == param(18);
    EXPECT_EQ(eq.type, query_craft::ast::expression_type::binary);
    EXPECT_EQ(eq.binary_value->op, query_craft::ast::binary_operator::equals);

    query_craft::ast::expression ne = col("age") != param(18);
    EXPECT_EQ(ne.type, query_craft::ast::expression_type::binary);
    EXPECT_EQ(ne.binary_value->op, query_craft::ast::binary_operator::not_equals);

    query_craft::ast::expression lt = col("age") < param(18);
    EXPECT_EQ(lt.type, query_craft::ast::expression_type::binary);
    EXPECT_EQ(lt.binary_value->op, query_craft::ast::binary_operator::less);

    query_craft::ast::expression le = col("age") <= param(18);
    EXPECT_EQ(le.type, query_craft::ast::expression_type::binary);
    EXPECT_EQ(le.binary_value->op, query_craft::ast::binary_operator::less_or_equals);

    query_craft::ast::expression gt = col("age") > param(18);
    EXPECT_EQ(gt.type, query_craft::ast::expression_type::binary);
    EXPECT_EQ(gt.binary_value->op, query_craft::ast::binary_operator::greater);

    query_craft::ast::expression ge = col("age") >= param(18);
    EXPECT_EQ(ge.type, query_craft::ast::expression_type::binary);
    EXPECT_EQ(ge.binary_value->op, query_craft::ast::binary_operator::greater_or_equals);
}

/**
 * @brief Тест логических операторов
 */
TEST_F(QueryCraft, Expression_LogicalOperators)
{
    using namespace query_craft::dsl;

    query_craft::ast::expression and_expr = (col("age") > param(18)) && (col("active") == param(true));
    EXPECT_EQ(and_expr.type, query_craft::ast::expression_type::binary);
    EXPECT_EQ(and_expr.binary_value->op, query_craft::ast::binary_operator::and_);

    query_craft::ast::expression or_expr = (col("age") < param(18)) || (col("age") > param(65));
    EXPECT_EQ(or_expr.type, query_craft::ast::expression_type::binary);
    EXPECT_EQ(or_expr.binary_value->op, query_craft::ast::binary_operator::or_);

    query_craft::ast::expression not_expr = !col("active");
    EXPECT_EQ(not_expr.type, query_craft::ast::expression_type::unary);
    EXPECT_EQ(not_expr.unary_value->op, query_craft::ast::unary_operator::not_);
}

/**
 * @brief Тест создания пользовательского бинарного оператора
 */
TEST_F(QueryCraft, Expression_MakeCustomBinary)
{
    using namespace query_craft::dsl;

    query_craft::ast::expression left = col("data");
    query_craft::ast::expression right = param(std::string("key"));

    query_craft::ast::expression expr = query_craft::ast::expression::make_custom_binary("json_get", left, right);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::binary);
    EXPECT_EQ(expr.binary_value->op, query_craft::ast::binary_operator::custom);
    EXPECT_EQ(expr.binary_value->custom_operator_name, "json_get");
}

/**
 * @brief Тест создания унарного выражения с оператором NOT
 */
TEST_F(QueryCraft, Expression_MakeUnaryNot)
{
    using namespace query_craft::dsl;

    query_craft::ast::expression operand = col("active");

    query_craft::ast::expression expr = query_craft::ast::expression::make_unary(query_craft::ast::unary_operator::not_, operand);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::unary);
    EXPECT_NE(expr.unary_value, nullptr);
    EXPECT_EQ(expr.unary_value->op, query_craft::ast::unary_operator::not_);
    EXPECT_TRUE(expr.unary_value->custom_operator_name.empty());
    EXPECT_NE(expr.unary_value->operand, nullptr);
}

/**
 * @brief Тест создания унарного выражения с оператором минус
 */
TEST_F(QueryCraft, Expression_MakeUnaryMinus)
{
    using namespace query_craft::dsl;

    query_craft::ast::expression operand = col("price");

    query_craft::ast::expression expr = query_craft::ast::expression::make_unary(query_craft::ast::unary_operator::minus, operand);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::unary);
    EXPECT_NE(expr.unary_value, nullptr);
    EXPECT_EQ(expr.unary_value->op, query_craft::ast::unary_operator::minus);
    EXPECT_TRUE(expr.unary_value->custom_operator_name.empty());
    EXPECT_NE(expr.unary_value->operand, nullptr);
}

/**
 * @brief Тест создания унарного выражения с оператором плюс
 */
TEST_F(QueryCraft, Expression_MakeUnaryPlus)
{
    using namespace query_craft::dsl;

    query_craft::ast::expression operand = col("value");

    query_craft::ast::expression expr = query_craft::ast::expression::make_unary(query_craft::ast::unary_operator::plus, operand);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::unary);
    EXPECT_NE(expr.unary_value, nullptr);
    EXPECT_EQ(expr.unary_value->op, query_craft::ast::unary_operator::plus);
    EXPECT_TRUE(expr.unary_value->custom_operator_name.empty());
    EXPECT_NE(expr.unary_value->operand, nullptr);
}

/**
 * @brief Тест создания унарного выражения с оператором IS NULL
 */
TEST_F(QueryCraft, Expression_MakeUnaryIsNull)
{
    using namespace query_craft::dsl;
    query_craft::ast::expression operand = col("email");

    query_craft::ast::expression expr = query_craft::ast::expression::make_unary(query_craft::ast::unary_operator::is_null, operand);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::unary);
    EXPECT_NE(expr.unary_value, nullptr);
    EXPECT_EQ(expr.unary_value->op, query_craft::ast::unary_operator::is_null);
    EXPECT_TRUE(expr.unary_value->custom_operator_name.empty());
    EXPECT_NE(expr.unary_value->operand, nullptr);
}

/**
 * @brief Тест создания унарного выражения с оператором IS NOT NULL
 */
TEST_F(QueryCraft, Expression_MakeUnaryIsNotNull)
{
    using namespace query_craft::dsl;
    query_craft::ast::expression operand = col("phone");

    query_craft::ast::expression expr = query_craft::ast::expression::make_unary(query_craft::ast::unary_operator::is_not_null, operand);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::unary);
    EXPECT_NE(expr.unary_value, nullptr);
    EXPECT_EQ(expr.unary_value->op, query_craft::ast::unary_operator::is_not_null);
    EXPECT_TRUE(expr.unary_value->custom_operator_name.empty());
    EXPECT_NE(expr.unary_value->operand, nullptr);
}

/**
 * @brief Тест создания унарного выражения с оператором EXISTS
 */
TEST_F(QueryCraft, Expression_MakeUnaryExists)
{
    using namespace query_craft::dsl;

    query_craft::ast::select_query subquery;
    subquery.from.name = "orders";
    subquery.columns.push_back(col("id"));
    query_craft::ast::expression operand = query_craft::ast::expression::make_subquery(subquery);

    query_craft::ast::expression expr = query_craft::ast::expression::make_unary(query_craft::ast::unary_operator::exists, operand);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::unary);
    EXPECT_NE(expr.unary_value, nullptr);
    EXPECT_EQ(expr.unary_value->op, query_craft::ast::unary_operator::exists);
    EXPECT_TRUE(expr.unary_value->custom_operator_name.empty());
    EXPECT_NE(expr.unary_value->operand, nullptr);
}

/**
 * @brief Тест создания пользовательского унарного оператора
 */
TEST_F(QueryCraft, Expression_MakeCustomUnary)
{
    using namespace query_craft::dsl;

    query_craft::ast::expression operand = col("data");

    query_craft::ast::expression expr = query_craft::ast::expression::make_custom_unary("jsonb_exists", operand);
    EXPECT_EQ(expr.type, query_craft::ast::expression_type::unary);
    EXPECT_NE(expr.unary_value, nullptr);
    EXPECT_EQ(expr.unary_value->op, query_craft::ast::unary_operator::custom);
    EXPECT_EQ(expr.unary_value->custom_operator_name, "jsonb_exists");
    EXPECT_NE(expr.unary_value->operand, nullptr);
}

/**
 * @brief Тест копирования унарного выражения с кастомным оператором
 */
TEST_F(QueryCraft, Expression_CopyUnaryWithCustomOperator)
{
    using namespace query_craft::dsl;
    query_craft::ast::expression original = query_craft::ast::expression::make_custom_unary("jsonb_exists", col("data"));
    query_craft::ast::expression copied = original;

    EXPECT_EQ(copied.type, query_craft::ast::expression_type::unary);
    EXPECT_NE(copied.unary_value, nullptr);
    EXPECT_NE(original.unary_value, nullptr);
    EXPECT_EQ(copied.unary_value->op, query_craft::ast::unary_operator::custom);
    EXPECT_EQ(copied.unary_value->custom_operator_name, "jsonb_exists");
    EXPECT_EQ(original.unary_value->custom_operator_name, "jsonb_exists");
    // Проверяем, что это разные объекты
    EXPECT_NE(copied.unary_value.get(), original.unary_value.get());
}
