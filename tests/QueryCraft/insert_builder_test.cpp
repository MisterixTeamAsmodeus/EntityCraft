#include "querycraft.hpp"
#include "QueryCraft/builder/insert_builder.h"

/**
 * @brief Тест создания простого INSERT запроса
 */
TEST_F(QueryCraft, InsertBuilder_SimpleInsert)
{
    using namespace query_craft::dsl;
    insert_builder builder;
    builder.into("users")
        .columns({ "name", "age" })
        .values({ param(std::string("John")), param(30) });

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "INSERT INTO \"users\" (\"name\", \"age\") VALUES ($1, $2);");
    EXPECT_EQ(query.parameters.size(), 2);
    EXPECT_EQ(query.parameters[0], "John");
    EXPECT_EQ(query.parameters[1], "30");
}

/**
 * @brief Тест INSERT с несколькими строками
 */
TEST_F(QueryCraft, InsertBuilder_InsertMultipleRows)
{
    using namespace query_craft::dsl;

    insert_builder builder;
    builder.into("users")
        .columns({ "name", "age" })
        .values({ param(std::string("John")), param(30) })
        .values({ param(std::string("Jane")), param(25) });

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "INSERT INTO \"users\" (\"name\", \"age\") VALUES ($1, $2), ($3, $4);");
    EXPECT_EQ(query.parameters.size(), 4);
    EXPECT_EQ(query.parameters[0], "John");
    EXPECT_EQ(query.parameters[1], "30");
    EXPECT_EQ(query.parameters[2], "Jane");
    EXPECT_EQ(query.parameters[3], "25");
}

/**
 * @brief Тест INSERT с RETURNING (PostgreSQL)
 */
TEST_F(QueryCraft, InsertBuilder_InsertWithReturning)
{
    using namespace query_craft::dsl;
    insert_builder builder;
    builder.into("users", "public")
        .columns({ "name" })
        .values({ param(std::string("John")) })
        .returning({ "id" });

    auto pg_query = builder.compile(postgres_dialect_);
    EXPECT_EQ(pg_query.sql, "INSERT INTO \"public\".\"users\" (\"name\") VALUES ($1) RETURNING \"id\";");
    EXPECT_EQ(pg_query.parameters.size(), 1);
    EXPECT_EQ(pg_query.parameters[0], "John");

    // SQLite поддерживает RETURNING
    auto sqlite_query = builder.compile(sqlite_dialect_);
    EXPECT_EQ(sqlite_query.sql, "INSERT INTO \"public\".\"users\" (\"name\") VALUES (?) RETURNING \"id\";");
    EXPECT_EQ(sqlite_query.parameters.size(), 1);
    EXPECT_EQ(sqlite_query.parameters[0], "John");
}

/**
 * @brief Тест INSERT с схемой таблицы
 */
TEST_F(QueryCraft, InsertBuilder_InsertWithSchema)
{
    using namespace query_craft::dsl;
    insert_builder builder;
    builder.into("users", "public")
        .columns({ "name" })
        .values({ param(std::string("John")) });

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "INSERT INTO \"public\".\"users\" (\"name\") VALUES ($1);");
    EXPECT_EQ(query.parameters.size(), 1);
    EXPECT_EQ(query.parameters[0], "John");
}

/**
 * @brief Тест INSERT с различными типами данных
 */
TEST_F(QueryCraft, InsertBuilder_InsertWithDifferentTypes)
{
    using namespace query_craft::dsl;
    insert_builder builder;
    builder.into("users")
        .columns({ "name", "age", "active", "score" })
        .values({ param(std::string("John")),
            param(30),
            param(true),
            param(95.5) });

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.parameters.size(), 4);
    EXPECT_EQ(query.parameters[0], "John");
    EXPECT_EQ(query.parameters[1], "30");
    EXPECT_EQ(query.parameters[2], "true");
    EXPECT_NE(query.parameters[3].find("95.5"), std::string::npos);
}

/**
 * @brief Тест метода to_ast()
 */
TEST_F(QueryCraft, InsertBuilder_ToAst)
{
    using namespace query_craft::dsl;
    insert_builder builder;
    builder.into("users")
        .columns({ "name", "age" })
        .values({ param(std::string("John")), param(30) });

    const auto& ast = builder.to_ast();
    EXPECT_EQ(ast.table.name, "users");
    EXPECT_EQ(ast.columns.size(), 2);
    EXPECT_EQ(ast.values.size(), 1);
    EXPECT_EQ(ast.values[0].size(), 2);
}

/**
 * @brief Тест INSERT с пустыми значениями
 */
TEST_F(QueryCraft, InsertBuilder_InsertWithEmptyValues)
{
    using namespace query_craft::dsl;
    insert_builder builder;
    builder.into("users")
        .columns({ "name" })
        .values({ param(std::string("")) });

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.parameters.size(), 1);
    EXPECT_EQ(query.parameters[0], "");
}
