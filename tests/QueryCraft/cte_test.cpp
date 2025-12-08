#include "querycraft.hpp"

/**
 * @brief Тест создания CTE
 */
TEST_F(QueryCraft, CTE_CreateCTE)
{
    using namespace query_craft::dsl;

    query_craft::ast::cte cte_item;
    cte_item.name = "active_users";

    query_craft::ast::select_query query;
    query.from.name = "users";
    query.columns.push_back(col("id"));
    query.where = col("active") == param(true);

    cte_item.query = query;

    EXPECT_EQ(cte_item.name, "active_users");
    EXPECT_EQ(cte_item.query.from.name, "users");
    EXPECT_EQ(cte_item.query.columns.size(), 1);
}

/**
 * @brief Тест копирования CTE
 */
TEST_F(QueryCraft, CTE_CopyCTE)
{
    using namespace query_craft::dsl;

    query_craft::ast::cte original;
    original.name = "test_cte";
    original.query.from.name = "users";
    original.query.columns.push_back(col("id"));

    query_craft::ast::cte copied = original;
    EXPECT_EQ(copied.name, original.name);
    EXPECT_EQ(copied.query.from.name, original.query.from.name);
    EXPECT_EQ(copied.query.columns.size(), original.query.columns.size());
}
