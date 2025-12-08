#include "querycraft.hpp"

/**
 * @brief Тест создания простого SELECT запроса
 */
TEST_F(QueryCraft, SelectBuilder_SimpleSelect)
{
    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users", "public")
        .columns({ col("id"), col("name") });

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "SELECT \"users\".\"id\", \"users\".\"name\" FROM \"public\".\"users\";");
    EXPECT_TRUE(query.parameters.empty());
}

/**
 * @brief Тест SELECT с WHERE условием
 */
TEST_F(QueryCraft, SelectBuilder_SelectWithWhere)
{
    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users")
        .columns({ col("id") })
        .where(col("age") > param(18));

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "SELECT \"users\".\"id\" FROM \"users\" WHERE (\"users\".\"age\" > $1);");
    EXPECT_EQ(query.parameters.size(), 1);
    EXPECT_EQ(query.parameters[0], "18");
}

/**
 * @brief Тест SELECT с GROUP BY
 */
TEST_F(QueryCraft, SelectBuilder_SelectWithGroupBy)
{
    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("orders")
        .columns({ col("user_id"), count({ col("id") }) })
        .group_by({ col("user_id") });

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "SELECT \"orders\".\"user_id\", COUNT(\"orders\".\"id\") FROM \"orders\" GROUP BY \"orders\".\"user_id\";");
    EXPECT_TRUE(query.parameters.empty());
}

/**
 * @brief Тест SELECT с HAVING
 */
TEST_F(QueryCraft, SelectBuilder_SelectWithHaving)
{
    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("orders")
        .columns({ col("user_id"), sum({ col("amount") }) })
        .group_by({ col("user_id") })
        .having(sum({ col("amount") }) > param(1000));

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "SELECT \"orders\".\"user_id\", SUM(\"orders\".\"amount\") FROM \"orders\" GROUP BY \"orders\".\"user_id\" HAVING (SUM(\"orders\".\"amount\") > $1);");
    EXPECT_EQ(query.parameters.size(), 1);
    EXPECT_EQ(query.parameters[0], "1000");
}

/**
 * @brief Тест SELECT с ORDER BY
 */
TEST_F(QueryCraft, SelectBuilder_SelectWithOrderBy)
{
    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users")
        .columns({ col("id"), col("name") })
        .order_by({ { col("name"), true }, { col("id"), false } });

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "SELECT \"users\".\"id\", \"users\".\"name\" FROM \"users\" ORDER BY \"users\".\"name\" ASC, \"users\".\"id\" DESC;");
    EXPECT_TRUE(query.parameters.empty());
}

/**
 * @brief Тест SELECT с LIMIT и OFFSET
 */
TEST_F(QueryCraft, SelectBuilder_SelectWithLimitOffset)
{
    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users")
        .columns({ col("id") })
        .limit(10)
        .offset(20);

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "SELECT \"users\".\"id\" FROM \"users\" LIMIT 10 OFFSET 20;");
    EXPECT_TRUE(query.parameters.empty());
}

/**
 * @brief Тест SELECT с INNER JOIN
 */
TEST_F(QueryCraft, SelectBuilder_SelectWithInnerJoin)
{
    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users")
        .columns({ col("id"), col("name") })
        .inner_join("orders", col("id") == col("user_id"));

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "SELECT \"users\".\"id\", \"users\".\"name\" FROM \"users\" INNER JOIN \"orders\" ON (\"users\".\"id\" = \"orders\".\"user_id\");");
    EXPECT_TRUE(query.parameters.empty());
}

/**
 * @brief Тест SELECT с LEFT JOIN
 */
TEST_F(QueryCraft, SelectBuilder_SelectWithLeftJoin)
{
    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users")
        .columns({ col("id") })
        .left_join("profiles", col("id") == col("user_id"), "public");

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "SELECT \"users\".\"id\" FROM \"users\" LEFT JOIN \"public\".\"profiles\" ON (\"users\".\"id\" = \"profiles\".\"user_id\");");
    EXPECT_TRUE(query.parameters.empty());
}

/**
 * @brief Тест SELECT с RIGHT JOIN
 */
TEST_F(QueryCraft, SelectBuilder_SelectWithRightJoin)
{
    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users")
        .columns({ col("id") })
        .right_join("orders", col("id") == col("user_id"));

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "SELECT \"users\".\"id\" FROM \"users\" RIGHT JOIN \"orders\" ON (\"users\".\"id\" = \"orders\".\"user_id\");");
    EXPECT_TRUE(query.parameters.empty());
}

/**
 * @brief Тест SELECT с FULL JOIN
 */
TEST_F(QueryCraft, SelectBuilder_SelectWithFullJoin)
{
    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users")
        .columns({ col("id") })
        .full_join("orders", col("id") == col("user_id"));

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "SELECT \"users\".\"id\" FROM \"users\" FULL JOIN \"orders\" ON (\"users\".\"id\" = \"orders\".\"user_id\");");
    EXPECT_TRUE(query.parameters.empty());
}

/**
 * @brief Тест SELECT с несколькими JOIN
 */
TEST_F(QueryCraft, SelectBuilder_SelectWithMultipleJoins)
{
    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users")
        .columns({ col("id"), col("order_id") })
        .inner_join("orders", col("id") == col("user_id"))
        .left_join("products", col("product_id") == col("id"));

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "SELECT \"users\".\"id\", \"users\".\"order_id\" FROM \"users\" INNER JOIN \"orders\" ON (\"users\".\"id\" = \"orders\".\"user_id\") LEFT JOIN \"products\" ON (\"orders\".\"product_id\" = \"products\".\"id\");");
    EXPECT_TRUE(query.parameters.empty());
}

/**
 * @brief Тест SELECT с CTE
 */
TEST_F(QueryCraft, SelectBuilder_SelectWithCTE)
{
    using namespace query_craft::dsl;
    select_builder cte_query;
    cte_query.from("users")
        .columns({ col("id") })
        .where(col("active") == param(true));

    select_builder builder;
    builder.with("active_users", cte_query.to_ast())
        .from("active_users")
        .columns({ col("id") });

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "WITH \"active_users\" AS (SELECT \"users\".\"id\" FROM \"users\" WHERE (\"users\".\"active\" = $1)) SELECT \"active_users\".\"id\" FROM \"active_users\";");
    EXPECT_EQ(query.parameters.size(), 1);
    EXPECT_EQ(query.parameters[0], "true");
}

/**
 * @brief Тест SELECT с подзапросом
 */
TEST_F(QueryCraft, SelectBuilder_SelectWithSubquery)
{
    using namespace query_craft::dsl;
    select_builder subquery_builder;
    subquery_builder.from("orders")
        .columns({ col("user_id") })
        .where(col("amount") > param(100));

    select_builder builder;
    builder.from("users")
        .columns({ col("id") })
        .where(col("id") == subquery(subquery_builder.to_ast()));

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "SELECT \"users\".\"id\" FROM \"users\" WHERE (\"users\".\"id\" = (SELECT \"orders\".\"user_id\" FROM \"orders\" WHERE (\"orders\".\"amount\" > $1)));");
    EXPECT_EQ(query.parameters.size(), 1);
    EXPECT_EQ(query.parameters[0], "100");
}

/**
 * @brief Тест SELECT с схемой таблицы
 */
TEST_F(QueryCraft, SelectBuilder_SelectWithSchema)
{
    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users", "public")
        .columns({ col("id") });

    auto query = builder.compile(postgres_dialect_);
    EXPECT_EQ(query.sql, "SELECT \"users\".\"id\" FROM \"public\".\"users\";");
    EXPECT_TRUE(query.parameters.empty());
}

/**
 * @brief Тест метода to_ast()
 */
TEST_F(QueryCraft, SelectBuilder_ToAst)
{
    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users")
        .columns({ col("id") })
        .limit(10);

    const auto& ast = builder.to_ast();
    EXPECT_EQ(ast.from.name, "users");
    EXPECT_EQ(ast.limit, 10);
    EXPECT_EQ(ast.columns.size(), 1);
}

/**
 * @brief Тест различий между PostgreSQL и SQLite диалектами
 */
TEST_F(QueryCraft, SelectBuilder_DialectDifferences)
{
    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users")
        .columns({ col("id") })
        .where(col("age") > param(18))
        .limit(10)
        .offset(20);

    auto pg_query = builder.compile(postgres_dialect_);
    auto sqlite_query = builder.compile(sqlite_dialect_);

    // PostgreSQL использует $1, $2, ... для плейсхолдеров
    EXPECT_EQ(pg_query.sql, "SELECT \"users\".\"id\" FROM \"users\" WHERE (\"users\".\"age\" > $1) LIMIT 10 OFFSET 20;");
    EXPECT_EQ(pg_query.parameters.size(), 1);
    EXPECT_EQ(pg_query.parameters[0], "18");

    // SQLite использует ? для плейсхолдеров
    EXPECT_EQ(sqlite_query.sql, "SELECT \"users\".\"id\" FROM \"users\" WHERE (\"users\".\"age\" > ?) LIMIT 10 OFFSET 20;");
    EXPECT_EQ(sqlite_query.parameters.size(), 1);
    EXPECT_EQ(sqlite_query.parameters[0], "18");
}

/**
 * @brief Тест SELECT ... FOR UPDATE
 */
TEST_F(QueryCraft, SelectBuilder_SelectForUpdate)
{
    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users")
        .columns({ col("id") })
        .where(col("id") == param(1))
        .for_update();

    auto pg_query = builder.compile(postgres_dialect_);
    EXPECT_EQ(pg_query.sql, "SELECT \"users\".\"id\" FROM \"users\" WHERE (\"users\".\"id\" = $1) FOR UPDATE;");
    EXPECT_EQ(pg_query.parameters.size(), 1);
    EXPECT_EQ(pg_query.parameters[0], "1");
}