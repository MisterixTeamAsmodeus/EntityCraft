#include "EntityCraft/reflection/referencecolumn.h"

#include <EntityCraft/utils/join_builder.h>
#include <EntityCraft/utils/mapper.h>
#include <gtest/gtest.h>

#include <QueryCraft/querycraft.h>
#include <string>
#include <vector>

/**
 * @brief Тестовая структура User для тестов join_builder
 */
struct TestUserForJoin
{
    int id = 0;
    std::string name;

    TestUserForJoin() = default;

    static auto dto()
    {
        return entity_craft::make_table<TestUserForJoin>(
            "users",
            "",
            entity_craft::make_column("id", &TestUserForJoin::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("name", &TestUserForJoin::name, entity_craft::not_null()));
    }
};

/**
 * @brief Тестовая структура Profile для тестов join_builder
 */
struct TestProfileForJoin
{
    int id = 0;
    int user_id = 0;
    std::string bio;

    TestProfileForJoin() = default;

    static auto dto()
    {
        return entity_craft::make_table<TestProfileForJoin>(
            "profiles",
            "",
            entity_craft::make_column("id", &TestProfileForJoin::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("user_id", &TestProfileForJoin::user_id, entity_craft::not_null()),
            entity_craft::make_column("bio", &TestProfileForJoin::bio, entity_craft::not_null()));
    }
};

/**
 * @brief Тестовая структура Order для тестов join_builder
 */
struct TestOrderForJoin
{
    int id = 0;
    int user_id = 0;
    std::string description;

    TestOrderForJoin() = default;

    static auto dto()
    {
        return entity_craft::make_table<TestOrderForJoin>(
            "orders",
            "",
            entity_craft::make_column("id", &TestOrderForJoin::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("user_id", &TestOrderForJoin::user_id, entity_craft::not_null()),
            entity_craft::make_column("description", &TestOrderForJoin::description, entity_craft::not_null()));
    }
};

/**
 * @brief Тестовая структура User с many_to_one связью к Profile
 */
struct TestUserWithProfileManyToOneForJoin
{
    int id = 0;
    std::string name;
    int profile_id = 0;
    TestProfileForJoin profile;

    TestUserWithProfileManyToOneForJoin() = default;

    static auto dto()
    {
        auto profile_table = TestProfileForJoin::dto();
        return entity_craft::make_table<TestUserWithProfileManyToOneForJoin>(
            "users",
            "",
            entity_craft::make_column("id", &TestUserWithProfileManyToOneForJoin::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("name", &TestUserWithProfileManyToOneForJoin::name, entity_craft::not_null()),
            entity_craft::make_column("profile_id", &TestUserWithProfileManyToOneForJoin::profile_id, entity_craft::not_null()),
            entity_craft::make_reference_column("profile", &TestUserWithProfileManyToOneForJoin::profile, profile_table, entity_craft::relation_type::many_to_one));
    }
};

/**
 * @brief Тестовая структура User с one_to_one связью к Profile
 */
struct TestUserWithProfileOneToOneForJoin
{
    int id = 0;
    std::string name;
    int profile_id = 0;
    TestProfileForJoin profile;

    TestUserWithProfileOneToOneForJoin() = default;

    static auto dto()
    {
        auto profile_table = TestProfileForJoin::dto();
        return entity_craft::make_table<TestUserWithProfileOneToOneForJoin>(
            "users",
            "",
            entity_craft::make_column("id", &TestUserWithProfileOneToOneForJoin::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("name", &TestUserWithProfileOneToOneForJoin::name, entity_craft::not_null()),
            entity_craft::make_column("profile_id", &TestUserWithProfileOneToOneForJoin::profile_id, entity_craft::not_null()),
            entity_craft::make_reference_column("profile", &TestUserWithProfileOneToOneForJoin::profile, profile_table, entity_craft::relation_type::one_to_one));
    }
};

/**
 * @brief Тестовая структура Profile с one_to_one_inverted связью к User
 */
struct TestProfileWithUserOneToOneInvertedForJoin
{
    int id = 0;
    int user_id = 0;
    std::string bio;
    TestUserForJoin user;

    TestProfileWithUserOneToOneInvertedForJoin() = default;

    static auto dto()
    {
        auto user_table = TestUserForJoin::dto();
        return entity_craft::make_table<TestProfileWithUserOneToOneInvertedForJoin>(
            "profiles",
            "",
            entity_craft::make_column("id", &TestProfileWithUserOneToOneInvertedForJoin::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("user_id", &TestProfileWithUserOneToOneInvertedForJoin::user_id, entity_craft::not_null()),
            entity_craft::make_column("bio", &TestProfileWithUserOneToOneInvertedForJoin::bio, entity_craft::not_null()),
            entity_craft::make_reference_column("user", &TestProfileWithUserOneToOneInvertedForJoin::user, user_table, entity_craft::relation_type::one_to_one_inverted));
    }
};

/**
 * @brief Тестовая структура User с one_to_many связью к Orders
 */
struct TestUserWithOrdersForJoin
{
    int id = 0;
    std::string name;
    std::vector<TestOrderForJoin> orders;

    TestUserWithOrdersForJoin() = default;

    static auto dto()
    {
        auto order_table = TestOrderForJoin::dto();
        return entity_craft::make_table<TestUserWithOrdersForJoin>(
            "users",
            "",
            entity_craft::make_column("id", &TestUserWithOrdersForJoin::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("name", &TestUserWithOrdersForJoin::name, entity_craft::not_null()),
            entity_craft::make_reference_column("orders", &TestUserWithOrdersForJoin::orders, order_table, entity_craft::relation_type::one_to_many));
    }
};

/**
 * @brief Тестовая структура Order с one_to_many связью к OrderItems (для рекурсивных зависимостей)
 */
struct TestOrderItemForJoin
{
    int id = 0;
    int order_id = 0;
    std::string name;

    TestOrderItemForJoin() = default;

    static auto dto()
    {
        return entity_craft::make_table<TestOrderItemForJoin>(
            "order_items",
            "",
            entity_craft::make_column("id", &TestOrderItemForJoin::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("order_id", &TestOrderItemForJoin::order_id, entity_craft::not_null()),
            entity_craft::make_column("name", &TestOrderItemForJoin::name, entity_craft::not_null()));
    }
};

/**
 * @brief Тестовая структура Order с one_to_many связью к OrderItems
 */
struct TestOrderWithItemsForJoin
{
    int id = 0;
    int user_id = 0;
    std::string description;
    std::vector<TestOrderItemForJoin> items;

    TestOrderWithItemsForJoin() = default;

    static auto dto()
    {
        auto item_table = TestOrderItemForJoin::dto();
        return entity_craft::make_table<TestOrderWithItemsForJoin>(
            "orders",
            "",
            entity_craft::make_column("id", &TestOrderWithItemsForJoin::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("user_id", &TestOrderWithItemsForJoin::user_id, entity_craft::not_null()),
            entity_craft::make_column("description", &TestOrderWithItemsForJoin::description, entity_craft::not_null()),
            entity_craft::make_reference_column("items", &TestOrderWithItemsForJoin::items, item_table, entity_craft::relation_type::one_to_many));
    }
};

/**
 * @brief Тестовая структура User с рекурсивными зависимостями (User -> Orders -> OrderItems)
 */
struct TestUserWithOrdersAndItemsForJoin
{
    int id = 0;
    std::string name;
    std::vector<TestOrderWithItemsForJoin> orders;

    TestUserWithOrdersAndItemsForJoin() = default;

    static auto dto()
    {
        auto order_table = TestOrderWithItemsForJoin::dto();
        return entity_craft::make_table<TestUserWithOrdersAndItemsForJoin>(
            "users",
            "",
            entity_craft::make_column("id", &TestUserWithOrdersAndItemsForJoin::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("name", &TestUserWithOrdersAndItemsForJoin::name, entity_craft::not_null()),
            entity_craft::make_reference_column("orders", &TestUserWithOrdersAndItemsForJoin::orders, order_table, entity_craft::relation_type::one_to_many));
    }
};

/**
 * @brief Фикстура для тестов join_builder
 */
class JoinBuilderTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        postgres_dialect_ = std::make_shared<query_craft::postgres_dialect>();
    }

protected:
    std::shared_ptr<query_craft::postgres_dialect> postgres_dialect_;
};

/**
 * @brief Тест create_join_condition - many_to_one связь
 */
TEST_F(JoinBuilderTest, CreateJoinCondition_ManyToOne)
{
    entity_craft::dependency_info info;
    info.relation = entity_craft::relation_type::many_to_one;
    info.main_fk_column = "profile_id";
    info.dep_pk_column = "id";

    auto condition = entity_craft::create_join_condition(info);

    // Проверяем, что условие создано (не пустое и не вызывает исключение)
    // Проверяем тип выражения - должно быть binary
    EXPECT_EQ(condition.type, query_craft::ast::expression_type::binary);
}

/**
 * @brief Тест create_join_condition - one_to_one связь
 */
TEST_F(JoinBuilderTest, CreateJoinCondition_OneToOne)
{
    entity_craft::dependency_info info;
    info.relation = entity_craft::relation_type::one_to_one;
    info.main_fk_column = "profile_id";
    info.dep_pk_column = "id";

    auto condition = entity_craft::create_join_condition(info);

    // Проверяем, что условие создано
    EXPECT_EQ(condition.type, query_craft::ast::expression_type::binary);
}

/**
 * @brief Тест create_join_condition - one_to_one_inverted связь
 */
TEST_F(JoinBuilderTest, CreateJoinCondition_OneToOneInverted)
{
    entity_craft::dependency_info info;
    info.relation = entity_craft::relation_type::one_to_one_inverted;
    info.main_pk_column = "id";
    info.dep_fk_column = "user_id";

    auto condition = entity_craft::create_join_condition(info);

    // Проверяем, что условие создано
    EXPECT_EQ(condition.type, query_craft::ast::expression_type::binary);
}

/**
 * @brief Тест create_join_condition - one_to_many связь
 */
TEST_F(JoinBuilderTest, CreateJoinCondition_OneToMany)
{
    entity_craft::dependency_info info;
    info.relation = entity_craft::relation_type::one_to_many;
    info.main_pk_column = "id";
    info.dep_fk_column = "user_id";

    auto condition = entity_craft::create_join_condition(info);

    // Проверяем, что условие создано
    EXPECT_EQ(condition.type, query_craft::ast::expression_type::binary);
}

/**
 * @brief Тест create_join_condition - недопустимый тип связи
 */
TEST_F(JoinBuilderTest, CreateJoinCondition_InvalidRelationType)
{
    entity_craft::dependency_info info;
    // Используем недопустимое значение (предполагая, что relation_type - это enum)
    // В реальности это может не скомпилироваться, но проверим обработку default case
    info.relation = static_cast<entity_craft::relation_type>(999);

    EXPECT_THROW(entity_craft::create_join_condition(info), std::invalid_argument);
}

/**
 * @brief Тест build_select_with_joins - many_to_one связь
 */
TEST_F(JoinBuilderTest, BuildSelectWithJoins_ManyToOne)
{
    auto user_table = TestUserWithProfileManyToOneForJoin::dto();
    auto builder = entity_craft::build_select_with_joins(user_table);
    auto query = builder.compile(postgres_dialect_);

    EXPECT_EQ(query.sql, "SELECT \"users\".\"id\" AS \"users_id\", \"users\".\"name\" AS \"users_name\", \"users\".\"profile_id\" AS \"users_profile_id\", \"profiles\".\"id\" AS \"profiles_id\", \"profiles\".\"user_id\" AS \"profiles_user_id\", \"profiles\".\"bio\" AS \"profiles_bio\" FROM \"users\" LEFT JOIN \"profiles\" ON (\"users\".\"profile_id\" = \"profiles\".\"id\");");
    EXPECT_TRUE(query.parameters.empty());
}

/**
 * @brief Тест build_select_with_joins - one_to_one связь
 */
TEST_F(JoinBuilderTest, BuildSelectWithJoins_OneToOne)
{
    auto user_table = TestUserWithProfileOneToOneForJoin::dto();
    auto builder = entity_craft::build_select_with_joins(user_table);
    auto query = builder.compile(postgres_dialect_);

    EXPECT_EQ(query.sql, "SELECT \"users\".\"id\" AS \"users_id\", \"users\".\"name\" AS \"users_name\", \"users\".\"profile_id\" AS \"users_profile_id\", \"profiles\".\"id\" AS \"profiles_id\", \"profiles\".\"user_id\" AS \"profiles_user_id\", \"profiles\".\"bio\" AS \"profiles_bio\" FROM \"users\" LEFT JOIN \"profiles\" ON (\"users\".\"profile_id\" = \"profiles\".\"id\");");
    EXPECT_TRUE(query.parameters.empty());
}

/**
 * @brief Тест build_select_with_joins - one_to_one_inverted связь
 */
TEST_F(JoinBuilderTest, BuildSelectWithJoins_OneToOneInverted)
{
    auto profile_table = TestProfileWithUserOneToOneInvertedForJoin::dto();
    auto builder = entity_craft::build_select_with_joins(profile_table);
    auto query = builder.compile(postgres_dialect_);

    EXPECT_EQ(query.sql, "SELECT \"profiles\".\"id\" AS \"profiles_id\", \"profiles\".\"user_id\" AS \"profiles_user_id\", \"profiles\".\"bio\" AS \"profiles_bio\", \"users\".\"id\" AS \"users_id\", \"users\".\"name\" AS \"users_name\" FROM \"profiles\" LEFT JOIN \"users\" ON (\"profiles\".\"user_id\" = \"users\".\"id\");");
    EXPECT_TRUE(query.parameters.empty());
}

/**
 * @brief Тест build_select_with_joins - one_to_many связь
 */
TEST_F(JoinBuilderTest, BuildSelectWithJoins_OneToMany)
{
    auto user_table = TestUserWithOrdersForJoin::dto();
    auto builder = entity_craft::build_select_with_joins(user_table);
    auto query = builder.compile(postgres_dialect_);

    EXPECT_EQ(query.sql, "SELECT \"users\".\"id\" AS \"users_id\", \"users\".\"name\" AS \"users_name\", \"orders\".\"id\" AS \"orders_id\", \"orders\".\"user_id\" AS \"orders_user_id\", \"orders\".\"description\" AS \"orders_description\" FROM \"users\" LEFT JOIN \"orders\" ON (\"users\".\"id\" = \"orders\".\"user_id\");");
    EXPECT_TRUE(query.parameters.empty());
}

/**
 * @brief Тест build_select_with_joins - рекурсивные зависимости
 */
TEST_F(JoinBuilderTest, BuildSelectWithJoins_RecursiveDependencies)
{
    auto user_table = TestUserWithOrdersAndItemsForJoin::dto();
    auto builder = entity_craft::build_select_with_joins(user_table);
    auto query = builder.compile(postgres_dialect_);

    EXPECT_EQ(query.sql, "SELECT \"users\".\"id\" AS \"users_id\", \"users\".\"name\" AS \"users_name\", \"orders\".\"id\" AS \"orders_id\", \"orders\".\"user_id\" AS \"orders_user_id\", \"orders\".\"description\" AS \"orders_description\", \"order_items\".\"id\" AS \"order_items_id\", \"order_items\".\"order_id\" AS \"order_items_order_id\", \"order_items\".\"name\" AS \"order_items_name\" FROM \"users\" LEFT JOIN \"orders\" ON (\"users\".\"id\" = \"orders\".\"user_id\") LEFT JOIN \"order_items\" ON (\"orders\".\"id\" = \"order_items\".\"order_id\");");
    EXPECT_TRUE(query.parameters.empty());
}

/**
 * @brief Тест build_select_with_joins - с условием WHERE
 */
TEST_F(JoinBuilderTest, BuildSelectWithJoins_WithWhere)
{
    using namespace query_craft::dsl;
    auto user_table = TestUserWithProfileManyToOneForJoin::dto();
    auto condition = col("users.id") == param(1);
    auto builder = entity_craft::build_select_with_joins(user_table, condition);
    auto query = builder.compile(postgres_dialect_);

    EXPECT_EQ(query.sql, "SELECT \"users\".\"id\" AS \"users_id\", \"users\".\"name\" AS \"users_name\", \"users\".\"profile_id\" AS \"users_profile_id\", \"profiles\".\"id\" AS \"profiles_id\", \"profiles\".\"user_id\" AS \"profiles_user_id\", \"profiles\".\"bio\" AS \"profiles_bio\" FROM \"users\" LEFT JOIN \"profiles\" ON (\"users\".\"profile_id\" = \"profiles\".\"id\") WHERE (\"users\".\"id\" = $1);");
    EXPECT_EQ(query.parameters.size(), 1);
    EXPECT_EQ(query.parameters[0], "1");
}

/**
 * @brief Тест collect_dependencies - базовая зависимость
 */
TEST_F(JoinBuilderTest, CollectDependencies_Basic)
{
    using namespace query_craft::dsl;
    select_builder builder;
    auto user_table = TestUserWithProfileManyToOneForJoin::dto();
    
    auto dependencies = entity_craft::collect_dependencies(builder, user_table);

    // Должна быть одна зависимость
    EXPECT_EQ(dependencies.size(), 1);
    EXPECT_EQ(dependencies[0].relation, entity_craft::relation_type::many_to_one);
    EXPECT_EQ(dependencies[0].dep_table_name, "profiles");
}

/**
 * @brief Тест collect_dependencies - рекурсивные зависимости
 */
TEST_F(JoinBuilderTest, CollectDependencies_Recursive)
{
    using namespace query_craft::dsl;
    select_builder builder;
    auto user_table = TestUserWithOrdersAndItemsForJoin::dto();
    
    auto dependencies = entity_craft::collect_dependencies(builder, user_table);

    // Должны быть зависимости: orders и order_items (рекурсивно)
    EXPECT_GE(dependencies.size(), 1);
    
    // Проверяем, что есть зависимость orders
    bool found_orders = false;
    bool found_order_items = false;
    for(const auto& dep : dependencies) {
        if(dep.dep_table_name == "orders") {
            found_orders = true;
        }
        if(dep.dep_table_name == "order_items") {
            found_order_items = true;
        }
    }
    
    EXPECT_TRUE(found_orders);
    EXPECT_TRUE(found_order_items);
}

/**
 * @brief Тест add_joins_for_dependencies - проверка корректности JOIN
 */
TEST_F(JoinBuilderTest, AddJoinsForDependencies_Correctness)
{
    using namespace query_craft::dsl;
    select_builder builder;
    auto user_table = TestUserWithProfileManyToOneForJoin::dto();
    
    builder.from(user_table.table_name(), user_table.scheme());
    entity_craft::add_joins_for_dependencies(builder, user_table);
    
    auto query = builder.compile(postgres_dialect_);

    EXPECT_EQ(query.sql, "SELECT * FROM \"users\" LEFT JOIN \"profiles\" ON (\"users\".\"profile_id\" = \"profiles\".\"id\");");
    EXPECT_TRUE(query.parameters.empty());
}

/**
 * @brief Тест add_joins_for_dependencies - множественные JOIN
 */
TEST_F(JoinBuilderTest, AddJoinsForDependencies_MultipleJoins)
{
    using namespace query_craft::dsl;
    select_builder builder;
    auto user_table = TestUserWithOrdersAndItemsForJoin::dto();
    
    builder.from(user_table.table_name(), user_table.scheme());
    entity_craft::add_joins_for_dependencies(builder, user_table);
    
    auto query = builder.compile(postgres_dialect_);

    EXPECT_EQ(query.sql, "SELECT * FROM \"users\" LEFT JOIN \"orders\" ON (\"users\".\"id\" = \"orders\".\"user_id\") LEFT JOIN \"order_items\" ON (\"orders\".\"id\" = \"order_items\".\"order_id\");");
    EXPECT_TRUE(query.parameters.empty());
}

/**
 * @brief Тест add_table_columns_to_select - проверка добавления колонок с алиасами
 */
TEST_F(JoinBuilderTest, AddTableColumnsToSelect_WithAliases)
{
    using namespace query_craft::dsl;
    select_builder builder;
    auto user_table = TestUserForJoin::dto();
    
    builder.from(user_table.table_name(), user_table.scheme());
    entity_craft::add_table_columns_to_select(builder, user_table);
    
    auto query = builder.compile(postgres_dialect_);

    EXPECT_EQ(query.sql, "SELECT \"users\".\"id\" AS \"users_id\", \"users\".\"name\" AS \"users_name\" FROM \"users\";");
    EXPECT_TRUE(query.parameters.empty());
}
