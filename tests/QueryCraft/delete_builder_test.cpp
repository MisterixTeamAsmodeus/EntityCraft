#include "querycraft.hpp"
#include "QueryCraft/builder/delete_builder.h"

/**
 * @brief Тест создания простого DELETE запроса
 */
TEST_F(QueryCraft, DeleteBuilder_SimpleDelete)
{
    using namespace query_craft::dsl;

    delete_builder builder;
    builder.from("users")
        .where(col("id") == param(1));

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "DELETE FROM \"users\" WHERE (\"id\" = $1);");
    EXPECT_EQ(query.parameters.size(), 1);
    EXPECT_EQ(query.parameters[0], "1");
}

/**
 * @brief Тест DELETE с RETURNING (PostgreSQL)
 */
TEST_F(QueryCraft, DeleteBuilder_DeleteWithReturning)
{
    using namespace query_craft::dsl;

    delete_builder builder;
    builder.from("users")
        .where(col("id") == param(1))
        .returning({ "id", "name" });

    auto pg_query = builder.compile(postgres_dialect_);
    EXPECT_EQ(pg_query.sql, "DELETE FROM \"users\" WHERE (\"id\" = $1) RETURNING \"id\", \"name\";");
    EXPECT_EQ(pg_query.parameters.size(), 1);
    EXPECT_EQ(pg_query.parameters[0], "1");
}

/**
 * @brief Тест DELETE с схемой таблицы
 */
TEST_F(QueryCraft, DeleteBuilder_DeleteWithSchema)
{
    using namespace query_craft::dsl;

    delete_builder builder;
    builder.from("users", "public")
        .where(col("id") == param(1));

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "DELETE FROM \"public\".\"users\" WHERE (\"id\" = $1);");
    EXPECT_EQ(query.parameters.size(), 1);
    EXPECT_EQ(query.parameters[0], "1");
}

/**
 * @brief Тест DELETE без WHERE (опасный запрос, но должен работать)
 */
TEST_F(QueryCraft, DeleteBuilder_DeleteWithoutWhere)
{
    using namespace query_craft::dsl;
    delete_builder builder;
    builder.from("users");

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "DELETE FROM \"users\";");
    EXPECT_TRUE(query.parameters.empty());
}

/**
 * @brief Тест DELETE с сложным WHERE условием
 */
TEST_F(QueryCraft, DeleteBuilder_DeleteWithComplexWhere)
{
    using namespace query_craft::dsl;
    delete_builder builder;
    builder.from("users")
        .where((col("age") < param(18)) && (col("active") == param(false)));

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "DELETE FROM \"users\" WHERE ((\"age\" < $1) AND (\"active\" = $2));");
    EXPECT_EQ(query.parameters.size(), 2);
    EXPECT_EQ(query.parameters[0], "18");
    EXPECT_EQ(query.parameters[1], "false");
}

/**
 * @brief Тест метода to_ast()
 */
TEST_F(QueryCraft, DeleteBuilder_ToAst)
{
    using namespace query_craft::dsl;
    delete_builder builder;
    builder.from("users")
        .where(col("id") == param(1));

    const auto& ast = builder.to_ast();
    EXPECT_EQ(ast.table.name, "users");
}
