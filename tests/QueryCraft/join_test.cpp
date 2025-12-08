#include "querycraft.hpp"

/**
 * @brief Тест создания JOIN с различными типами
 */
TEST_F(QueryCraft, Join_JoinTypes)
{
    query_craft::ast::join j;

    j.type = query_craft::ast::join_type::inner;
    EXPECT_EQ(j.type, query_craft::ast::join_type::inner);

    j.type = query_craft::ast::join_type::left;
    EXPECT_EQ(j.type, query_craft::ast::join_type::left);

    j.type = query_craft::ast::join_type::right;
    EXPECT_EQ(j.type, query_craft::ast::join_type::right);

    j.type = query_craft::ast::join_type::full;
    EXPECT_EQ(j.type, query_craft::ast::join_type::full);
}

/**
 * @brief Тест создания JOIN с таблицей и условием
 */
TEST_F(QueryCraft, Join_JoinWithTableAndCondition)
{
    using namespace query_craft::dsl;
    query_craft::ast::join j;
    j.type = query_craft::ast::join_type::inner;
    j.table.name = "orders";
    j.table.schema = "public";
    j.on = col("users", "users") == col("user_id", "orders");

    EXPECT_EQ(j.table.name, "orders");
    EXPECT_EQ(j.table.schema, "public");
    EXPECT_EQ(j.on.type, query_craft::ast::expression_type::binary);
}

/**
 * @brief Тест значений по умолчанию
 */
TEST_F(QueryCraft, Join_DefaultValues)
{
    query_craft::ast::join j;
    EXPECT_EQ(j.type, query_craft::ast::join_type::inner);
    EXPECT_TRUE(j.table.name.empty());
    EXPECT_TRUE(j.table.schema.empty());
    EXPECT_TRUE(j.table.alias.empty());
}
