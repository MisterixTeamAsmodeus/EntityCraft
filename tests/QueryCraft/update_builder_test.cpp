#include "querycraft.hpp"

/**
 * @brief Тест создания простого UPDATE запроса
 */
TEST_F(QueryCraft, UpdateBuilder_SimpleUpdate)
{
    using namespace query_craft::dsl;
    update_builder builder;
    builder.table("users")
        .set("name", param(std::string("John")))
        .where(col("id") == param(1));

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "UPDATE \"users\" SET \"name\" = $1 WHERE (\"id\" = $2);");
    EXPECT_EQ(query.parameters.size(), 2);
    EXPECT_EQ(query.parameters[0], "John");
    EXPECT_EQ(query.parameters[1], "1");
}

/**
 * @brief Тест UPDATE с несколькими SET
 */
TEST_F(QueryCraft, UpdateBuilder_UpdateMultipleColumns)
{
    using namespace query_craft::dsl;
    update_builder builder;
    builder.table("users")
        .set("name", param(std::string("John")))
        .set("age", param(30))
        .set("active", param(true))
        .where(col("id") == param(1));

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "UPDATE \"users\" SET \"name\" = $1, \"age\" = $2, \"active\" = $3 WHERE (\"id\" = $4);");
    EXPECT_EQ(query.parameters.size(), 4);
    EXPECT_EQ(query.parameters[0], "John");
    EXPECT_EQ(query.parameters[1], "30");
    EXPECT_EQ(query.parameters[2], "true");
    EXPECT_EQ(query.parameters[3], "1");
}

/**
 * @brief Тест UPDATE с RETURNING (PostgreSQL)
 */
TEST_F(QueryCraft, UpdateBuilder_UpdateWithReturning)
{
    using namespace query_craft::dsl;
    update_builder builder;
    builder.table("users")
        .set("name", param(std::string("John")))
        .where(col("id") == param(1))
        .returning({ "id", "name" });

    auto pg_query = builder.compile(postgres_dialect_);
    EXPECT_EQ(pg_query.sql, "UPDATE \"users\" SET \"name\" = $1 WHERE (\"id\" = $2) RETURNING \"id\", \"name\";");
    EXPECT_EQ(pg_query.parameters.size(), 2);
    EXPECT_EQ(pg_query.parameters[0], "John");
    EXPECT_EQ(pg_query.parameters[1], "1");
}

/**
 * @brief Тест UPDATE с схемой таблицы
 */
TEST_F(QueryCraft, UpdateBuilder_UpdateWithSchema)
{
    using namespace query_craft::dsl;
    update_builder builder;
    builder.table("users", "public")
        .set("name", param(std::string("John")))
        .where(col("id") == param(1));

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "UPDATE \"public\".\"users\" SET \"name\" = $1 WHERE (\"id\" = $2);");
    EXPECT_EQ(query.parameters.size(), 2);
    EXPECT_EQ(query.parameters[0], "John");
    EXPECT_EQ(query.parameters[1], "1");
}

/**
 * @brief Тест UPDATE без WHERE (опасный запрос, но должен работать)
 */
TEST_F(QueryCraft, UpdateBuilder_UpdateWithoutWhere)
{
    using namespace query_craft::dsl;
    update_builder builder;
    builder.table("users")
        .set("name", param(std::string("John")));

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "UPDATE \"users\" SET \"name\" = $1;");
    EXPECT_EQ(query.parameters.size(), 1);
    EXPECT_EQ(query.parameters[0], "John");
}

/**
 * @brief Тест UPDATE с сложным WHERE условием
 */
TEST_F(QueryCraft, UpdateBuilder_UpdateWithComplexWhere)
{
    using namespace query_craft::dsl;
    update_builder builder;
    builder.table("users")
        .set("active", param(false))
        .where((col("age") < param(18)) || (col("age") > param(65)));

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "UPDATE \"users\" SET \"active\" = $1 WHERE ((\"age\" < $2) OR (\"age\" > $3));");
    EXPECT_EQ(query.parameters.size(), 3);
    EXPECT_EQ(query.parameters[0], "false");
    EXPECT_EQ(query.parameters[1], "18");
    EXPECT_EQ(query.parameters[2], "65");
}

/**
 * @brief Тест метода to_ast()
 */
TEST_F(QueryCraft, UpdateBuilder_ToAst)
{
    using namespace query_craft::dsl;
    update_builder builder;
    builder.table("users")
        .set("name", param(std::string("John")))
        .where(col("id") == param(1));

    const auto& ast = builder.to_ast();
    EXPECT_EQ(ast.table.name, "users");
    EXPECT_EQ(ast.set_clauses.size(), 1);
    EXPECT_EQ(ast.set_clauses[0].first.name, "name");
}
