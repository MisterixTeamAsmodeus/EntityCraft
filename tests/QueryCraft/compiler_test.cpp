#include "querycraft.hpp"

#include <string>

/**
 * @brief Тест компиляции SELECT запроса
 */
TEST_F(QueryCraft, Compiler_CompileSelectQuery)
{
    using namespace query_craft::dsl;

    query_craft::ast::select_query query;
    query.from.name = "users";
    query.from.schema = "public";
    query.columns.push_back(col("id"));
    query.columns.push_back(col("name"));

    query_craft::sql_compiler compiler(postgres_dialect_);
    auto result = compiler.compile(query);

    EXPECT_EQ(result.sql, "SELECT \"users\".\"id\", \"users\".\"name\" FROM \"public\".\"users\";");
    EXPECT_TRUE(result.parameters.empty());
}

/**
 * @brief Тест компиляции INSERT запроса
 */
TEST_F(QueryCraft, Compiler_CompileInsertQuery)
{
    using namespace query_craft::dsl;

    query_craft::ast::insert_query query;
    query.table.name = "users";
    query.table.schema = "public";
    query.columns.push_back(query_craft::ast::identifier { "", "name", "" });
    query.columns.push_back(query_craft::ast::identifier { "", "age", "" });

    std::vector<query_craft::ast::expression> row1;
    row1.push_back(param(std::string("John")));
    row1.push_back(param(30));
    query.values.push_back(row1);

    query_craft::sql_compiler compiler(postgres_dialect_);
    auto result = compiler.compile(query);

    EXPECT_EQ(result.sql, "INSERT INTO \"public\".\"users\" (\"name\", \"age\") VALUES ($1, $2);");
    EXPECT_EQ(result.parameters.size(), 2);
    EXPECT_EQ(result.parameters[0], "John");
    EXPECT_EQ(result.parameters[1], "30");
}

/**
 * @brief Тест компиляции UPDATE запроса
 */
TEST_F(QueryCraft, Compiler_CompileUpdateQuery)
{
    using namespace query_craft::dsl;

    query_craft::ast::update_query query;
    query.table.name = "users";
    query.table.schema = "public";
    query.set_clauses.emplace_back(
        query_craft::ast::identifier { "", "name", "" },
        param(std::string("John")));
    query.where = col("id") == param(1);

    query_craft::sql_compiler compiler(postgres_dialect_);
    auto result = compiler.compile(query);

    EXPECT_EQ(result.sql, "UPDATE \"public\".\"users\" SET \"name\" = $1 WHERE (\"id\" = $2);");
    EXPECT_EQ(result.parameters.size(), 2);
    EXPECT_EQ(result.parameters[0], "John");
    EXPECT_EQ(result.parameters[1], "1");
}

/**
 * @brief Тест компиляции DELETE запроса
 */
TEST_F(QueryCraft, Compiler_CompileDeleteQuery)
{
    using namespace query_craft::dsl;

    query_craft::ast::delete_query query;
    query.table.name = "users";
    query.table.schema = "public";
    query.where = col("id") == param(1);

    query_craft::sql_compiler compiler(postgres_dialect_);
    auto result = compiler.compile(query);

    EXPECT_EQ(result.sql, "DELETE FROM \"public\".\"users\" WHERE (\"id\" = $1);");
    EXPECT_EQ(result.parameters.size(), 1);
    EXPECT_EQ(result.parameters[0], "1");
}

/**
 * @brief Тест компиляции подзапроса
 */
TEST_F(QueryCraft, Compiler_CompileSubquery)
{
    using namespace query_craft::dsl;

    query_craft::ast::select_query subquery;
    subquery.from.name = "orders";
    subquery.from.schema = "public";
    subquery.columns.push_back(col("user_id"));
    subquery.where = col("amount") > param(100);

    query_craft::ast::expression subquery_expr = query_craft::ast::expression::make_subquery(subquery);

    query_craft::ast::select_query query;
    query.from.name = "users";
    query.from.schema = "public";
    query.columns.push_back(col("id"));
    query.where = col("id") == subquery_expr;

    query_craft::sql_compiler compiler(postgres_dialect_);
    auto result = compiler.compile(query);

    EXPECT_EQ(result.sql, "SELECT \"users\".\"id\" FROM \"public\".\"users\" WHERE (\"users\".\"id\" = (SELECT \"orders\".\"user_id\" FROM \"public\".\"orders\" WHERE (\"orders\".\"amount\" > $1)));");
    EXPECT_EQ(result.parameters.size(), 1);
    EXPECT_EQ(result.parameters[0], "100");
}

/**
 * @brief Тест компиляции CTE
 */
TEST_F(QueryCraft, Compiler_CompileCTE)
{
    using namespace query_craft::dsl;

    query_craft::ast::select_query cte_query;
    cte_query.from.name = "users";
    cte_query.from.schema = "public";
    cte_query.columns.push_back(col("id"));
    cte_query.where = col("active") == param(true);

    query_craft::ast::cte cte_item;
    cte_item.name = "active_users";
    cte_item.query = cte_query;

    query_craft::ast::select_query query;
    query.with_clauses.push_back(cte_item);
    query.from.name = "active_users";
    query.from.schema = "public";
    query.columns.push_back(col("id"));

    query_craft::sql_compiler compiler(postgres_dialect_);
    auto result = compiler.compile(query);

    EXPECT_EQ(result.sql, "WITH \"active_users\" AS (SELECT \"users\".\"id\" FROM \"public\".\"users\" WHERE (\"users\".\"active\" = $1)) SELECT \"active_users\".\"id\" FROM \"public\".\"active_users\";");
    EXPECT_EQ(result.parameters.size(), 1);
    EXPECT_EQ(result.parameters[0], "true");
}

/**
 * @brief Тест различий между диалектами в компиляции
 */
TEST_F(QueryCraft, Compiler_DialectDifferences)
{
    using namespace query_craft::dsl;

    query_craft::ast::select_query query;
    query.from.name = "users";
    query.from.schema = "public";
    query.columns.push_back(col("id"));
    query.where = col("age") > param(18);
    query.limit = 10;
    query.offset = 20;

    query_craft::sql_compiler pg_compiler(postgres_dialect_);
    query_craft::sql_compiler sqlite_compiler(sqlite_dialect_);

    auto pg_result = pg_compiler.compile(query);
    auto sqlite_result = sqlite_compiler.compile(query);

    // PostgreSQL использует $1, $2, ...
    EXPECT_EQ(pg_result.sql, "SELECT \"users\".\"id\" FROM \"public\".\"users\" WHERE (\"users\".\"age\" > $1) LIMIT 10 OFFSET 20;");
    EXPECT_EQ(pg_result.parameters.size(), 1);
    EXPECT_EQ(pg_result.parameters[0], "18");

    // SQLite использует ?
    EXPECT_EQ(sqlite_result.sql, "SELECT \"users\".\"id\" FROM \"public\".\"users\" WHERE (\"users\".\"age\" > ?) LIMIT 10 OFFSET 20;");
    EXPECT_EQ(sqlite_result.parameters.size(), 1);
    EXPECT_EQ(sqlite_result.parameters[0], "18");
}

/**
 * @brief Тест компиляции сложного выражения
 */
TEST_F(QueryCraft, Compiler_CompileComplexExpression)
{
    using namespace query_craft::dsl;

    query_craft::ast::expression expr = (col("age") > param(18)) && (col("status") == param(std::string("active")));

    query_craft::ast::select_query query;
    query.from.name = "users";
    query.from.schema = "public";
    query.columns.push_back(col("id"));
    query.where = expr;

    query_craft::sql_compiler compiler(postgres_dialect_);
    auto result = compiler.compile(query);

    EXPECT_EQ(result.sql, "SELECT \"users\".\"id\" FROM \"public\".\"users\" WHERE ((\"users\".\"age\" > $1) AND (\"users\".\"status\" = $2));");
    EXPECT_EQ(result.parameters.size(), 2);
    EXPECT_EQ(result.parameters[0], "18");
    EXPECT_EQ(result.parameters[1], "active");
}

/**
 * @brief Тест компиляции унарного оператора NOT
 */
TEST_F(QueryCraft, Compiler_CompileUnaryNot)
{
    using namespace query_craft::dsl;

    query_craft::ast::select_query query;
    query.from.name = "users";
    query.from.schema = "public";
    query.columns.push_back(col("id"));
    query.where = !col("deleted");

    query_craft::sql_compiler compiler(postgres_dialect_);
    auto result = compiler.compile(query);

    EXPECT_EQ(result.sql, "SELECT \"users\".\"id\" FROM \"public\".\"users\" WHERE (NOT \"users\".\"deleted\");");
    EXPECT_TRUE(result.parameters.empty());
}

/**
 * @brief Тест компиляции унарного оператора минус
 */
TEST_F(QueryCraft, Compiler_CompileUnaryMinus)
{
    using namespace query_craft::dsl;

    query_craft::ast::select_query query;
    query.from.name = "products";
    query.from.schema = "public";
    query.columns.push_back(query_craft::ast::expression::make_unary(query_craft::ast::unary_operator::minus, col("price")));

    query_craft::sql_compiler compiler(postgres_dialect_);
    auto result = compiler.compile(query);

    EXPECT_EQ(result.sql, "SELECT -\"products\".\"price\" FROM \"public\".\"products\";");
    EXPECT_TRUE(result.parameters.empty());
}

/**
 * @brief Тест компиляции унарного оператора плюс
 */
TEST_F(QueryCraft, Compiler_CompileUnaryPlus)
{
    using namespace query_craft::dsl;

    query_craft::ast::select_query query;
    query.from.name = "products";
    query.from.schema = "public";
    query.columns.push_back(query_craft::ast::expression::make_unary(query_craft::ast::unary_operator::plus, col("value")));

    query_craft::sql_compiler compiler(postgres_dialect_);
    auto result = compiler.compile(query);

    EXPECT_EQ(result.sql, "SELECT +\"products\".\"value\" FROM \"public\".\"products\";");
    EXPECT_TRUE(result.parameters.empty());
}

/**
 * @brief Тест компиляции унарного оператора IS NULL
 */
TEST_F(QueryCraft, Compiler_CompileUnaryIsNull)
{
    using namespace query_craft::dsl;

    query_craft::ast::select_query query;
    query.from.name = "users";
    query.from.schema = "public";
    query.columns.push_back(col("id"));
    query.where = query_craft::ast::expression::make_unary(query_craft::ast::unary_operator::is_null, col("email"));

    query_craft::sql_compiler compiler(postgres_dialect_);
    auto result = compiler.compile(query);

    EXPECT_EQ(result.sql, "SELECT \"users\".\"id\" FROM \"public\".\"users\" WHERE (\"users\".\"email\" IS NULL);");
    EXPECT_TRUE(result.parameters.empty());
}

/**
 * @brief Тест компиляции унарного оператора IS NOT NULL
 */
TEST_F(QueryCraft, Compiler_CompileUnaryIsNotNull)
{
    using namespace query_craft::dsl;
    query_craft::ast::select_query query;
    query.from.name = "users";
    query.from.schema = "public";
    query.columns.push_back(col("id"));
    query.where = query_craft::ast::expression::make_unary(query_craft::ast::unary_operator::is_not_null, col("phone"));

    query_craft::sql_compiler compiler(postgres_dialect_);
    auto result = compiler.compile(query);

    EXPECT_EQ(result.sql, "SELECT \"users\".\"id\" FROM \"public\".\"users\" WHERE (\"users\".\"phone\" IS NOT NULL);");
    EXPECT_TRUE(result.parameters.empty());
}

/**
 * @brief Тест компиляции унарного оператора EXISTS
 */
TEST_F(QueryCraft, Compiler_CompileUnaryExists)
{
    using namespace query_craft::dsl;
    query_craft::ast::select_query subquery;
    subquery.from.name = "orders";
    subquery.from.schema = "public";
    subquery.columns.push_back(col("id"));

    // Создаем идентификатор для users.id вручную
    // Используем schema для префикса таблицы (в текущей структуре query_craft::ast::identifier
    // schema используется для префикса перед точкой, что подходит для table.column)
    query_craft::ast::identifier users_id;
    users_id.name = "id";
    users_id.schema = "users"; // Используем schema как префикс таблицы
    subquery.where = col("user_id") == query_craft::ast::expression::make_identifier(users_id);

    query_craft::ast::expression exists_expr = query_craft::ast::expression::make_unary(
        query_craft::ast::unary_operator::exists,
        query_craft::ast::expression::make_subquery(subquery));

    query_craft::ast::select_query query;
    query.from.name = "users";
    query.from.schema = "public";
    query.columns.push_back(col("id"));
    query.where = exists_expr;

    query_craft::sql_compiler compiler(postgres_dialect_);
    auto result = compiler.compile(query);

    EXPECT_EQ(result.sql, "SELECT \"users\".\"id\" FROM \"public\".\"users\" WHERE (EXISTS (SELECT \"orders\".\"id\" FROM \"public\".\"orders\" WHERE (\"orders\".\"user_id\" = \"public\".\"users\".\"id\")));");
    EXPECT_TRUE(result.parameters.empty());
}

/**
 * @brief Тест компиляции пользовательского унарного оператора
 */
TEST_F(QueryCraft, Compiler_CompileCustomUnary)
{
    class test_dialect : public query_craft::postgres_dialect
    {
    public:
        std::string custom_unary_operator(const std::string& logical_name) const override
        {
            if(logical_name == "jsonb_exists") {
                return "?";
            }
            return logical_name;
        }
    };

    auto test_dialect_ = std::make_shared<test_dialect>();

    using namespace query_craft::dsl;

    query_craft::ast::select_query query;
    query.from.name = "products";
    query.from.schema = "public";
    query.columns.push_back(col("id"));
    query.where = query_craft::ast::expression::make_custom_unary("jsonb_exists", col("metadata"));

    query_craft::sql_compiler compiler(test_dialect_);
    auto result = compiler.compile(query);

    EXPECT_EQ(result.sql, "SELECT \"products\".\"id\" FROM \"public\".\"products\" WHERE (? \"products\".\"metadata\");");
    EXPECT_TRUE(result.parameters.empty());
}

/**
 * @brief Тест компиляции сложного выражения с унарными операторами
 */
TEST_F(QueryCraft, Compiler_CompileComplexUnaryExpression)
{
    using namespace query_craft::dsl;
    query_craft::ast::select_query query;
    query.from.name = "users";
    query.from.schema = "public";
    query.columns.push_back(col("id"));
    query.where = query_craft::ast::expression::make_unary(query_craft::ast::unary_operator::is_not_null, col("email")) && !query_craft::ast::expression::make_unary(query_craft::ast::unary_operator::is_null, col("phone"));

    query_craft::sql_compiler compiler(postgres_dialect_);
    auto result = compiler.compile(query);

    EXPECT_EQ(result.sql, "SELECT \"users\".\"id\" FROM \"public\".\"users\" WHERE ((\"users\".\"email\" IS NOT NULL) AND (NOT (\"users\".\"phone\" IS NULL)));");
    EXPECT_TRUE(result.parameters.empty());
}
