#include "EntityCraft/reflection/referencecolumn.h"

#include <EntityCraft/utils/mapper.h>
#include <gtest/gtest.h>

#include <DatabaseAdapter/model/queryresult.hpp>
#include <algorithm>
#include <string>
#include <vector>

/**
 * @brief Тестовая структура User для тестов mapper
 */
struct TestUser
{
    int id = 0;
    std::string name;
    std::string email;
    int age = 0;

    TestUser() = default;

    TestUser(int id, std::string name, std::string email, int age)
        : id(id)
        , name(std::move(name))
        , email(std::move(email))
        , age(age)
    {
    }

    bool operator==(const TestUser& other) const
    {
        return id == other.id && name == other.name && email == other.email && age == other.age;
    }

    static auto dto()
    {
        return entity_craft::make_table<TestUser>(
            "users",
            "",
            entity_craft::make_column("id", &TestUser::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("name", &TestUser::name, entity_craft::not_null()),
            entity_craft::make_column("email", &TestUser::email, entity_craft::not_null()),
            entity_craft::make_column("age", &TestUser::age, entity_craft::not_null()));
    }
};

/**
 * @brief Тестовая структура для зависимой сущности
 */
struct TestProfile
{
    int id = 0;
    int user_id = 0;
    std::string bio;

    TestProfile() = default;

    TestProfile(int id, int user_id, std::string bio)
        : id(id)
        , user_id(user_id)
        , bio(std::move(bio))
    {
    }

    bool operator==(const TestProfile& other) const
    {
        return id == other.id && user_id == other.user_id && bio == other.bio;
    }

    static auto dto()
    {
        return entity_craft::make_table<TestProfile>(
            "profiles",
            "",
            entity_craft::make_column("id", &TestProfile::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("user_id", &TestProfile::user_id, entity_craft::not_null()),
            entity_craft::make_column("bio", &TestProfile::bio, entity_craft::not_null()));
    }
};

/**
 * @brief Тестовая структура для элементов заказа (3-й уровень вложенности)
 */
struct TestOrderItem
{
    int id = 0;
    std::string name;
    int quantity = 0;

    TestOrderItem() = default;

    TestOrderItem(int id, std::string name, int quantity)
        : id(id)
        , name(std::move(name))
        , quantity(quantity)
    {
    }

    bool operator==(const TestOrderItem& other) const
    {
        return id == other.id && name == other.name && quantity == other.quantity;
    }

    static auto dto()
    {
        return entity_craft::make_table<TestOrderItem>(
            "order_items",
            "",
            entity_craft::make_column("id", &TestOrderItem::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("name", &TestOrderItem::name, entity_craft::not_null()),
            entity_craft::make_column("quantity", &TestOrderItem::quantity, entity_craft::not_null()));
    }
};

/**
 * @brief Тестовая структура с коллекцией зависимых сущностей
 */
struct TestOrder
{
    int id = 0;
    std::string description;
    std::vector<TestOrderItem> items;

    TestOrder() = default;

    TestOrder(int id, std::string description)
        : id(id)
        , description(std::move(description))
    {
    }

    TestOrder(int id, std::string description, std::vector<TestOrderItem> items)
        : id(id)
        , description(std::move(description))
        , items(std::move(items))
    {
    }

    bool operator==(const TestOrder& other) const
    {
        return id == other.id && description == other.description;
    }

    static auto dto()
    {
        auto order_item_table = TestOrderItem::dto();
        return entity_craft::make_table<TestOrder>(
            "orders",
            "",
            entity_craft::make_column("id", &TestOrder::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("description", &TestOrder::description, entity_craft::not_null()),
            entity_craft::make_reference_column("items", &TestOrder::items, order_item_table, entity_craft::relation_type::one_to_many));
    }
};

/**
 * @brief Тестовая структура User с коллекцией orders для тестирования merge_entities
 */
struct TestUserWithOrders
{
    int id = 0;
    std::string name;
    std::vector<TestOrder> orders;

    TestUserWithOrders() = default;

    TestUserWithOrders(int id, std::string name, std::vector<TestOrder> orders)
        : id(id)
        , name(std::move(name))
        , orders(std::move(orders))
    {
    }

    static auto dto()
    {
        auto order_table = TestOrder::dto();
        return entity_craft::make_table<TestUserWithOrders>(
            "users",
            "",
            entity_craft::make_column("id", &TestUserWithOrders::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("name", &TestUserWithOrders::name, entity_craft::not_null()),
            entity_craft::make_reference_column("orders", &TestUserWithOrders::orders, order_table, entity_craft::relation_type::one_to_many));
    }
};

/**
 * @brief Тестовая структура User с many_to_one связью к Profile
 */
struct TestUserWithProfileManyToOne
{
    int id = 0;
    std::string name;
    int profile_id = 0;
    TestProfile profile;

    TestUserWithProfileManyToOne() = default;

    static auto dto()
    {
        auto profile_table = TestProfile::dto();
        return entity_craft::make_table<TestUserWithProfileManyToOne>(
            "users",
            "",
            entity_craft::make_column("id", &TestUserWithProfileManyToOne::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("name", &TestUserWithProfileManyToOne::name, entity_craft::not_null()),
            entity_craft::make_column("profile_id", &TestUserWithProfileManyToOne::profile_id, entity_craft::not_null()),
            entity_craft::make_reference_column("profile", &TestUserWithProfileManyToOne::profile, profile_table, entity_craft::relation_type::many_to_one));
    }
};

/**
 * @brief Тестовая структура User с one_to_one связью к Profile
 */
struct TestUserWithProfileOneToOne
{
    int id = 0;
    std::string name;
    int profile_id = 0;
    TestProfile profile;

    TestUserWithProfileOneToOne() = default;

    static auto dto()
    {
        auto profile_table = TestProfile::dto();
        return entity_craft::make_table<TestUserWithProfileOneToOne>(
            "users",
            "",
            entity_craft::make_column("id", &TestUserWithProfileOneToOne::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("name", &TestUserWithProfileOneToOne::name, entity_craft::not_null()),
            entity_craft::make_column("profile_id", &TestUserWithProfileOneToOne::profile_id, entity_craft::not_null()),
            entity_craft::make_reference_column("profile", &TestUserWithProfileOneToOne::profile, profile_table, entity_craft::relation_type::one_to_one));
    }
};

/**
 * @brief Тестовая структура Profile с one_to_one_inverted связью к User
 */
struct TestProfileWithUserOneToOneInverted
{
    int id = 0;
    int user_id = 0;
    std::string bio;
    TestUser user;

    TestProfileWithUserOneToOneInverted() = default;

    static auto dto()
    {
        auto user_table = TestUser::dto();
        return entity_craft::make_table<TestProfileWithUserOneToOneInverted>(
            "profiles",
            "",
            entity_craft::make_column("id", &TestProfileWithUserOneToOneInverted::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("user_id", &TestProfileWithUserOneToOneInverted::user_id, entity_craft::not_null()),
            entity_craft::make_column("bio", &TestProfileWithUserOneToOneInverted::bio, entity_craft::not_null()),
            entity_craft::make_reference_column("user", &TestProfileWithUserOneToOneInverted::user, user_table, entity_craft::relation_type::one_to_one_inverted));
    }
};

/**
 * @brief Фикстура для тестов mapper
 */
class MapperTest : public ::testing::Test
{
protected:
    decltype(TestUser::dto()) _user_table = TestUser::dto();
    decltype(TestProfile::dto()) _profile_table = TestProfile::dto();
    decltype(TestOrder::dto()) _order_table = TestOrder::dto();
};

/**
 * @brief Тест map_row_to_entity - базовый маппинг строки в сущность
 */
TEST_F(MapperTest, MapRowToEntity_Basic)
{
    database_adapter::query_result::row row;
    row["id"] = "1";
    row["name"] = "Test User";
    row["email"] = "test@example.com";
    row["age"] = "25";

    TestUser user = entity_craft::map_row_to_entity(_user_table, row, false);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, "Test User");
    EXPECT_EQ(user.email, "test@example.com");
    EXPECT_EQ(user.age, 25);
}

/**
 * @brief Тест map_row_to_entity - маппинг с использованием алиасов
 */
TEST_F(MapperTest, MapRowToEntity_WithAlias)
{
    database_adapter::query_result::row row;
    row["users_id"] = "2";
    row["users_name"] = "Alias User";
    row["users_email"] = "alias@example.com";
    row["users_age"] = "30";

    TestUser user = entity_craft::map_row_to_entity(_user_table, row, true);

    EXPECT_EQ(user.id, 2);
    EXPECT_EQ(user.name, "Alias User");
    EXPECT_EQ(user.email, "alias@example.com");
    EXPECT_EQ(user.age, 30);
}

/**
 * @brief Тест map_row_to_entity - обработка NULL значений
 */
TEST_F(MapperTest, MapRowToEntity_WithNullValues)
{
    database_adapter::query_result::row row;
    row["id"] = "3";
    row["name"] = "Null User";
    row["email"] = NULL_VALUE;
    row["age"] = "35";

    TestUser user = entity_craft::map_row_to_entity(_user_table, row, false);

    EXPECT_EQ(user.id, 3);
    EXPECT_EQ(user.name, "Null User");
    EXPECT_EQ(user.email, ""); // NULL значения должны быть пропущены
    EXPECT_EQ(user.age, 35);
}

/**
 * @brief Тест map_row_to_entity - обработка пустых значений
 */
TEST_F(MapperTest, MapRowToEntity_WithEmptyValues)
{
    database_adapter::query_result::row row;
    row["id"] = "4";
    row["name"] = "";
    row["email"] = "empty@example.com";
    row["age"] = "40";

    TestUser user = entity_craft::map_row_to_entity(_user_table, row, false);

    EXPECT_EQ(user.id, 4);
    EXPECT_EQ(user.name, "");
    EXPECT_EQ(user.email, "empty@example.com");
    EXPECT_EQ(user.age, 40);
}

/**
 * @brief Тест map_entity_to_params - базовый маппинг сущности в параметры
 */
TEST_F(MapperTest, MapEntityToParams_Basic)
{
    TestUser user(5, "Param User", "param@example.com", 45);

    std::vector<std::string> params = entity_craft::map_entity_to_params(_user_table, user, true);

    EXPECT_EQ(params.size(), 4);
    EXPECT_EQ(params[0], "5"); // id
    EXPECT_EQ(params[1], "Param User"); // name
    EXPECT_EQ(params[2], "param@example.com"); // email
    EXPECT_EQ(params[3], "45"); // age
}

/**
 * @brief Тест map_entity_to_params - исключение auto_increment
 */
TEST_F(MapperTest, MapEntityToParams_ExcludeAutoIncrement)
{
    TestUser user(6, "No Auto User", "noauto@example.com", 50);

    std::vector<std::string> params = entity_craft::map_entity_to_params(_user_table, user, false);

    // auto_increment колонка должна быть исключена
    EXPECT_EQ(params.size(), 3);
    EXPECT_EQ(params[0], "No Auto User"); // name
    EXPECT_EQ(params[1], "noauto@example.com"); // email
    EXPECT_EQ(params[2], "50"); // age
}

/**
 * @brief Тест map_result_to_entities - маппинг результата в вектор сущностей
 */
TEST_F(MapperTest, MapResultToEntities_Basic)
{
    database_adapter::query_result result;

    database_adapter::query_result::row row1;
    row1["users_id"] = "10";
    row1["users_name"] = "Result User 1";
    row1["users_email"] = "result1@example.com";
    row1["users_age"] = "20";
    result.add(row1);

    database_adapter::query_result::row row2;
    row2["users_id"] = "11";
    row2["users_name"] = "Result User 2";
    row2["users_email"] = "result2@example.com";
    row2["users_age"] = "25";
    result.add(row2);

    std::vector<TestUser> users = entity_craft::map_result_to_entities(_user_table, result);

    EXPECT_EQ(users.size(), 2);
    EXPECT_EQ(users[0].id, 10);
    EXPECT_EQ(users[0].name, "Result User 1");
    EXPECT_EQ(users[1].id, 11);
    EXPECT_EQ(users[1].name, "Result User 2");
}

/**
 * @brief Тест map_result_to_entities - пустой результат
 */
TEST_F(MapperTest, MapResultToEntities_Empty)
{
    database_adapter::query_result result;

    std::vector<TestUser> users = entity_craft::map_result_to_entities(_user_table, result);

    EXPECT_TRUE(users.empty());
}

/**
 * @brief Тест extract_dependent_entities - many_to_one связь с совпадающими ключами
 * 
 * Для many_to_one связи:
 * - main_fk_column - это FK в основной таблице (users), который ссылается на PK зависимой таблицы (profiles.id)
 * - dep_pk_column - это PK зависимой таблицы (profiles.id)
 * - Проверяется совпадение: main_fk_column == dep_pk_column
 */
TEST_F(MapperTest, ExtractDependentEntities_ManyToOne_MatchingKeys)
{
    database_adapter::query_result::row row;
    row["users_id"] = "1";
    row["users_name"] = "Main User";
    row["users_profile_id"] = "100"; // FK в основной таблице, ссылается на profiles.id
    row["profiles_id"] = "100"; // PK зависимой таблицы
    row["profiles_user_id"] = "1";
    row["profiles_bio"] = "Test Bio";

    auto profile_table = _profile_table;
    auto ref_column = entity_craft::make_reference_column(
        "profile",
        &TestUser::id,
        profile_table,
        entity_craft::relation_type::many_to_one);

    TestProfile profile = entity_craft::extract_dependent_entities(
        std::string("users_id"),        // main_pk_column
        std::string("users_profile_id"), // main_fk_column - FK в основной таблице
        std::string("profiles_id"),      // dep_pk_column - PK зависимой таблицы
        std::string("profiles_user_id"), // dep_fk_column
        ref_column,
        row);

    EXPECT_EQ(profile.id, 100);
    EXPECT_EQ(profile.user_id, 1);
    EXPECT_EQ(profile.bio, "Test Bio");
}

/**
 * @brief Тест extract_dependent_entities - many_to_one связь с несовпадающими ключами
 * 
 * Для many_to_one связи проверяется совпадение main_fk_column (FK в основной таблице) 
 * и dep_pk_column (PK зависимой таблицы). Если они не совпадают, возвращается пустая сущность.
 */
TEST_F(MapperTest, ExtractDependentEntities_ManyToOne_NonMatchingKeys)
{
    database_adapter::query_result::row row;
    row["users_id"] = "1";
    row["users_name"] = "Main User";
    row["users_profile_id"] = "200"; // FK в основной таблице, НЕ совпадает с profiles.id
    row["profiles_id"] = "100"; // PK зависимой таблицы
    row["profiles_user_id"] = "1";
    row["profiles_bio"] = "Test Bio";

    auto profile_table = _profile_table;    
    auto ref_column = entity_craft::make_reference_column(
        "profile",
        &TestUser::id,
        profile_table,
        entity_craft::relation_type::many_to_one);

    auto profile = entity_craft::extract_dependent_entities(
        std::string("users_id"),        // main_pk_column
        std::string("users_profile_id"), // main_fk_column - не совпадает с profiles.id
        std::string("profiles_id"),      // dep_pk_column
        std::string("profiles_user_id"), // dep_fk_column
        ref_column,
        row);

    // Должна вернуться пустая сущность, так как users_profile_id (200) != profiles_id (100)
    EXPECT_EQ(profile.id, 0);
    EXPECT_EQ(profile.user_id, 0);
    EXPECT_TRUE(profile.bio.empty());
}

/**
 * @brief Тест extract_dependent_entities - one_to_one_inverted связь
 * 
 * Для one_to_one_inverted связи:
 * - main_pk_column - это PK основной таблицы (users.id)
 * - dep_fk_column - это FK в зависимой таблице (profiles.user_id), который ссылается на users.id
 * - Проверяется совпадение: main_pk_column == dep_fk_column
 * - После проверки маппинг использует алиасы (true), поэтому нужны алиасы типа "profiles_id", "profiles_user_id", "profiles_bio"
 */
TEST_F(MapperTest, ExtractDependentEntities_OneToOneInverted)
{
    database_adapter::query_result::row row;
    row["users_id"] = "1";           // main_pk_column - PK основной таблицы
    row["users_name"] = "Main User";
    row["profiles_id"] = "100";       // PK зависимой таблицы (с алиасом)
    row["profiles_user_id"] = "1";     // dep_fk_column - FK в зависимой таблице, должен совпадать с users_id
    row["profiles_bio"] = "Test Bio";  // bio с алиасом

    auto profile_table = _profile_table;
    auto ref_column = entity_craft::make_reference_column(
        "profile",
        &TestUser::id,
        profile_table,
        entity_craft::relation_type::one_to_one_inverted);

    TestProfile profile = entity_craft::extract_dependent_entities(
        std::string("users_id"),        // main_pk_column - PK основной таблицы
        std::string("profiles_user_id"), // main_fk_column (не используется для one_to_one_inverted)
        std::string("profiles_id"),      // dep_pk_column - PK зависимой таблицы
        std::string("profiles_user_id"), // dep_fk_column - FK в зависимой таблице, должен совпадать с main_pk_column
        ref_column,
        row);

    EXPECT_EQ(profile.id, 100);
    EXPECT_EQ(profile.user_id, 1);
    EXPECT_EQ(profile.bio, "Test Bio");
}

/**
 * @brief Тест merge_entities - объединение сущностей с одинаковыми ключами
 */
TEST_F(MapperTest, MergeEntities_Basic)
{
    TestUser user1(1, "User 1", "user1@example.com", 25);
    TestUser user2(1, "User 1", "user1@example.com", 25); // Тот же ID

    std::vector<TestUser> entities = { user1, user2 };

    std::vector<TestUser> merged = entity_craft::merge_entities(entities, _user_table);

    // Должна остаться одна сущность
    EXPECT_EQ(merged.size(), 1);
    EXPECT_EQ(merged[0].id, 1);
}

/**
 * @brief Тест merge_entities - объединение сущностей с разными ключами
 */
TEST_F(MapperTest, MergeEntities_DifferentKeys)
{
    TestUser user1(1, "User 1", "user1@example.com", 25);
    TestUser user2(2, "User 2", "user2@example.com", 30);

    std::vector<TestUser> entities = { user1, user2 };

    std::vector<TestUser> merged = entity_craft::merge_entities(entities, _user_table);

    // Должны остаться обе сущности
    EXPECT_EQ(merged.size(), 2);
}

/**
 * @brief Тест merge_entities - пустой вектор
 */
TEST_F(MapperTest, MergeEntities_Empty)
{
    std::vector<TestUser> entities;

    std::vector<TestUser> merged = entity_craft::merge_entities(entities, _user_table);

    EXPECT_TRUE(merged.empty());
}

/**
 * @brief Тест map_row_to_entity_with_dependencies - базовый маппинг без зависимостей
 */
TEST_F(MapperTest, MapRowToEntityWithDependencies_Basic)
{
    database_adapter::query_result::row row;
    row["users_id"] = "1";
    row["users_name"] = "Dependency User";
    row["users_email"] = "dependency@example.com";
    row["users_age"] = "25";

    TestUser user = entity_craft::map_row_to_entity_with_dependencies(_user_table, row);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, "Dependency User");
    EXPECT_EQ(user.email, "dependency@example.com");
    EXPECT_EQ(user.age, 25);
}

/**
 * @brief Тест map_row_to_entity_with_dependencies - many_to_one связь
 */
TEST_F(MapperTest, MapRowToEntityWithDependencies_ManyToOne)
{
    auto user_table = TestUserWithProfileManyToOne::dto();
    
    database_adapter::query_result::row row;
    row["users_id"] = "1";
    row["users_name"] = "User 1";
    row["users_profile_id"] = "100";
    row["profiles_id"] = "100";
    row["profiles_user_id"] = "1";
    row["profiles_bio"] = "User 1 Bio";

    TestUserWithProfileManyToOne user = entity_craft::map_row_to_entity_with_dependencies(user_table, row);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, "User 1");
    EXPECT_EQ(user.profile_id, 100);
    EXPECT_EQ(user.profile.id, 100);
    EXPECT_EQ(user.profile.user_id, 1);
    EXPECT_EQ(user.profile.bio, "User 1 Bio");
}

/**
 * @brief Тест map_row_to_entity_with_dependencies - many_to_one связь с несовпадающими ключами
 */
TEST_F(MapperTest, MapRowToEntityWithDependencies_ManyToOne_NonMatchingKeys)
{
    auto user_table = TestUserWithProfileManyToOne::dto();
    
    database_adapter::query_result::row row;
    row["users_id"] = "1";
    row["users_name"] = "User 1";
    row["users_profile_id"] = "200"; // Не совпадает с profiles_id
    row["profiles_id"] = "100";
    row["profiles_user_id"] = "1";
    row["profiles_bio"] = "User 1 Bio";

    TestUserWithProfileManyToOne user = entity_craft::map_row_to_entity_with_dependencies(user_table, row);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, "User 1");
    EXPECT_EQ(user.profile_id, 200);
    // Профиль должен быть пустым, так как ключи не совпадают
    EXPECT_EQ(user.profile.id, 0);
    EXPECT_EQ(user.profile.user_id, 0);
    EXPECT_TRUE(user.profile.bio.empty());
}

/**
 * @brief Тест map_row_to_entity_with_dependencies - one_to_one связь
 */
TEST_F(MapperTest, MapRowToEntityWithDependencies_OneToOne)
{
    auto user_table = TestUserWithProfileOneToOne::dto();
    
    database_adapter::query_result::row row;
    row["users_id"] = "1";
    row["users_name"] = "User 1";
    row["users_profile_id"] = "100";
    row["profiles_id"] = "100";
    row["profiles_user_id"] = "1";
    row["profiles_bio"] = "User 1 Bio";

    TestUserWithProfileOneToOne user = entity_craft::map_row_to_entity_with_dependencies(user_table, row);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, "User 1");
    EXPECT_EQ(user.profile_id, 100);
    EXPECT_EQ(user.profile.id, 100);
    EXPECT_EQ(user.profile.user_id, 1);
    EXPECT_EQ(user.profile.bio, "User 1 Bio");
}

/**
 * @brief Тест map_row_to_entity_with_dependencies - one_to_one_inverted связь
 */
TEST_F(MapperTest, MapRowToEntityWithDependencies_OneToOneInverted)
{
    auto profile_table = TestProfileWithUserOneToOneInverted::dto();
    
    database_adapter::query_result::row row;
    row["profiles_id"] = "100";
    row["profiles_user_id"] = "1";
    row["profiles_bio"] = "User 1 Bio";
    row["users_id"] = "1";
    row["users_name"] = "User 1";
    row["users_email"] = "user1@example.com";
    row["users_age"] = "25";

    TestProfileWithUserOneToOneInverted profile = entity_craft::map_row_to_entity_with_dependencies(profile_table, row);

    EXPECT_EQ(profile.id, 100);
    EXPECT_EQ(profile.user_id, 1);
    EXPECT_EQ(profile.bio, "User 1 Bio");
    EXPECT_EQ(profile.user.id, 1);
    EXPECT_EQ(profile.user.name, "User 1");
    EXPECT_EQ(profile.user.email, "user1@example.com");
    EXPECT_EQ(profile.user.age, 25);
}

/**
 * @brief Тест map_row_to_entity_with_dependencies - one_to_one_inverted связь с несовпадающими ключами
 */
TEST_F(MapperTest, MapRowToEntityWithDependencies_OneToOneInverted_NonMatchingKeys)
{
    auto profile_table = TestProfileWithUserOneToOneInverted::dto();
    
    database_adapter::query_result::row row;
    row["profiles_id"] = "100";
    row["profiles_user_id"] = "2"; // Не совпадает с users_id
    row["profiles_bio"] = "User 1 Bio";
    row["users_id"] = "1";
    row["users_name"] = "User 1";
    row["users_email"] = "user1@example.com";
    row["users_age"] = "25";

    TestProfileWithUserOneToOneInverted profile = entity_craft::map_row_to_entity_with_dependencies(profile_table, row);

    EXPECT_EQ(profile.id, 100);
    EXPECT_EQ(profile.user_id, 2);
    EXPECT_EQ(profile.bio, "User 1 Bio");
    // User должен быть пустым, так как ключи не совпадают
    EXPECT_EQ(profile.user.id, 0);
    EXPECT_TRUE(profile.user.name.empty());
    EXPECT_TRUE(profile.user.email.empty());
    EXPECT_EQ(profile.user.age, 0);
}

/**
 * @brief Тест map_row_to_entity_with_dependencies - one_to_many связь
 */
TEST_F(MapperTest, MapRowToEntityWithDependencies_OneToMany)
{
    auto user_table = TestUserWithOrders::dto();
    
    database_adapter::query_result::row row;
    row["users_id"] = "1";
    row["users_name"] = "User 1";
    row["orders_id"] = "10";
    row["orders_description"] = "Order 1";
    row["order_items_id"] = "100";
    row["order_items_name"] = "Item 1";
    row["order_items_quantity"] = "5";

    TestUserWithOrders user = entity_craft::map_row_to_entity_with_dependencies(user_table, row);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, "User 1");
    EXPECT_EQ(user.orders.size(), 1);
    EXPECT_EQ(user.orders[0].id, 10);
    EXPECT_EQ(user.orders[0].description, "Order 1");
    EXPECT_EQ(user.orders[0].items.size(), 1);
    EXPECT_EQ(user.orders[0].items[0].id, 100);
    EXPECT_EQ(user.orders[0].items[0].name, "Item 1");
    EXPECT_EQ(user.orders[0].items[0].quantity, 5);
}

/**
 * @brief Тест map_row_to_entity_with_dependencies - one_to_many связь с несколькими зависимостями
 */
TEST_F(MapperTest, MapRowToEntityWithDependencies_OneToMany_MultipleDependencies)
{
    auto user_table = TestUserWithOrders::dto();
    
    database_adapter::query_result::row row;
    row["users_id"] = "1";
    row["users_name"] = "User 1";
    row["orders_id"] = "10";
    row["orders_description"] = "Order 1";
    row["order_items_id"] = "100";
    row["order_items_name"] = "Item 1";
    row["order_items_quantity"] = "5";

    TestUserWithOrders user = entity_craft::map_row_to_entity_with_dependencies(user_table, row);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, "User 1");
    EXPECT_EQ(user.orders.size(), 1);
    EXPECT_EQ(user.orders[0].id, 10);
    EXPECT_EQ(user.orders[0].description, "Order 1");
    EXPECT_EQ(user.orders[0].items.size(), 1);
}

/**
 * @brief Тест map_entity_to_params - обработка NULL значений
 */
TEST_F(MapperTest, MapEntityToParams_WithNullValues)
{
    TestUser user;
    user.id = 7;
    user.name = "Null Param User";
    user.email = "";
    user.age = 55;

    std::vector<std::string> params = entity_craft::map_entity_to_params(_user_table, user, true);

    EXPECT_EQ(params.size(), 4);
    EXPECT_EQ(params[0], "7");
    EXPECT_EQ(params[1], "Null Param User");
    EXPECT_EQ(params[2], ""); // Пустая строка
    EXPECT_EQ(params[3], "55");
}

/**
 * @brief Тест map_row_to_entity - отсутствующие колонки в row
 */
TEST_F(MapperTest, MapRowToEntity_MissingColumns)
{
    database_adapter::query_result::row row;
    row["id"] = "1";
    row["name"] = "Partial User";
    // email и age отсутствуют

    TestUser user = entity_craft::map_row_to_entity(_user_table, row, false);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, "Partial User");
    EXPECT_EQ(user.email, ""); // Не заполнено, так как колонка отсутствует
    EXPECT_EQ(user.age, 0);    // Не заполнено, так как колонка отсутствует
}

/**
 * @brief Тест map_row_to_entity - строка с лишними колонками
 */
TEST_F(MapperTest, MapRowToEntity_ExtraColumns)
{
    database_adapter::query_result::row row;
    row["id"] = "1";
    row["name"] = "Extra User";
    row["email"] = "extra@example.com";
    row["age"] = "25";
    row["extra_column"] = "extra_value"; // Лишняя колонка

    TestUser user = entity_craft::map_row_to_entity(_user_table, row, false);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, "Extra User");
    EXPECT_EQ(user.email, "extra@example.com");
    EXPECT_EQ(user.age, 25);
    // Лишняя колонка игнорируется
}

/**
 * @brief Тест map_row_to_entity - пустая строка row
 */
TEST_F(MapperTest, MapRowToEntity_EmptyRow)
{
    database_adapter::query_result::row row;

    TestUser user = entity_craft::map_row_to_entity(_user_table, row, false);

    EXPECT_EQ(user.id, 0);
    EXPECT_TRUE(user.name.empty());
    EXPECT_TRUE(user.email.empty());
    EXPECT_EQ(user.age, 0);
}

/**
 * @brief Тест map_row_to_entity - NULL_VALUE и пустая строка (краевой случай)
 */
TEST_F(MapperTest, MapRowToEntity_NullAndEmptyString)
{
    database_adapter::query_result::row row;
    row["id"] = "1";
    row["name"] = NULL_VALUE; // NULL значение
    row["email"] = "";        // Пустая строка (не NULL)
    row["age"] = "25";

    TestUser user = entity_craft::map_row_to_entity(_user_table, row, false);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, ""); // NULL_VALUE пропускается
    EXPECT_EQ(user.email, ""); // Пустая строка обрабатывается
    EXPECT_EQ(user.age, 25);
}

/**
 * @brief Тест extract_dependent_entities - one_to_one связь
 */
TEST_F(MapperTest, ExtractDependentEntities_OneToOne)
{
    database_adapter::query_result::row row;
    row["users_id"] = "1";
    row["users_name"] = "Main User";
    row["users_profile_id"] = "100"; // FK в основной таблице
    row["profiles_id"] = "100";       // PK зависимой таблицы
    row["profiles_user_id"] = "1";
    row["profiles_bio"] = "Test Bio";

    auto profile_table = _profile_table;
    auto ref_column = entity_craft::make_reference_column(
        "profile",
        &TestUser::id,
        profile_table,
        entity_craft::relation_type::one_to_one);

    TestProfile profile = entity_craft::extract_dependent_entities(
        std::string("users_id"),        // main_pk_column
        std::string("users_profile_id"), // main_fk_column
        std::string("profiles_id"),      // dep_pk_column
        std::string("profiles_user_id"), // dep_fk_column
        ref_column,
        row);

    EXPECT_EQ(profile.id, 100);
    EXPECT_EQ(profile.user_id, 1);
    EXPECT_EQ(profile.bio, "Test Bio");
}

/**
 * @brief Тест extract_dependent_entities - one_to_many связь
 */
TEST_F(MapperTest, ExtractDependentEntities_OneToMany)
{
    database_adapter::query_result::row row;
    row["users_id"] = "1";           // main_pk_column
    row["users_name"] = "Main User";
    row["orders_id"] = "200";        // PK зависимой таблицы
    row["orders_user_id"] = "1";     // dep_fk_column - FK в зависимой таблице, должен совпадать с users_id
    row["orders_description"] = "Test Order";

    auto order_table = _order_table;
    auto ref_column = entity_craft::make_reference_column(
        "orders",
        &TestUser::id,
        order_table,
        entity_craft::relation_type::one_to_many);

    TestOrder order = entity_craft::extract_dependent_entities(
        std::string("users_id"),        // main_pk_column
        std::string("orders_user_id"),  // main_fk_column (не используется для one_to_many)
        std::string("orders_id"),       // dep_pk_column
        std::string("orders_user_id"),   // dep_fk_column - должен совпадать с main_pk_column
        ref_column,
        row);

    EXPECT_EQ(order.id, 200);
    EXPECT_EQ(order.description, "Test Order");
}

/**
 * @brief Тест extract_dependent_entities - отсутствующие колонки в row
 */
TEST_F(MapperTest, ExtractDependentEntities_MissingColumns)
{
    database_adapter::query_result::row row;
    row["users_id"] = "1";
    // main_fk_column отсутствует
    row["profiles_id"] = "100";

    auto profile_table = _profile_table;
    auto ref_column = entity_craft::make_reference_column(
        "profile",
        &TestUser::id,
        profile_table,
        entity_craft::relation_type::many_to_one);

    TestProfile profile = entity_craft::extract_dependent_entities(
        std::string("users_id"),
        std::string("users_profile_id"), // Отсутствует в row
        std::string("profiles_id"),
        std::string("profiles_user_id"),
        ref_column,
        row);

    // Должна вернуться пустая сущность, так как колонка не найдена
    EXPECT_EQ(profile.id, 0);
    EXPECT_EQ(profile.user_id, 0);
    EXPECT_TRUE(profile.bio.empty());
}

/**
 * @brief Тест extract_dependent_entities - one_to_one_inverted с несовпадающими ключами
 */
TEST_F(MapperTest, ExtractDependentEntities_OneToOneInverted_NonMatchingKeys)
{
    database_adapter::query_result::row row;
    row["users_id"] = "1";
    row["users_name"] = "Main User";
    row["profiles_id"] = "100";
    row["profiles_user_id"] = "2"; // Не совпадает с users_id
    row["profiles_bio"] = "Test Bio";

    auto profile_table = _profile_table;
    auto ref_column = entity_craft::make_reference_column(
        "profile",
        &TestUser::id,
        profile_table,
        entity_craft::relation_type::one_to_one_inverted);

    TestProfile profile = entity_craft::extract_dependent_entities(
        std::string("users_id"),
        std::string("profiles_user_id"),
        std::string("profiles_id"),
        std::string("profiles_user_id"),
        ref_column,
        row);

    // Должна вернуться пустая сущность, так как users_id (1) != profiles_user_id (2)
    EXPECT_EQ(profile.id, 0);
    EXPECT_EQ(profile.user_id, 0);
    EXPECT_TRUE(profile.bio.empty());
}

/**
 * @brief Тест merge_entities - множественные дубликаты
 */
TEST_F(MapperTest, MergeEntities_MultipleDuplicates)
{
    TestUser user1(1, "User 1", "user1@example.com", 25);
    TestUser user2(1, "User 1", "user1@example.com", 25); // Дубликат
    TestUser user3(1, "User 1", "user1@example.com", 25); // Дубликат
    TestUser user4(2, "User 2", "user2@example.com", 30);

    std::vector<TestUser> entities = { user1, user2, user3, user4 };

    std::vector<TestUser> merged = entity_craft::merge_entities(entities, _user_table);

    // Должны остаться 2 сущности (1 и 2)
    EXPECT_EQ(merged.size(), 2);
    EXPECT_EQ(merged[0].id, 1);
    EXPECT_EQ(merged[1].id, 2);
}

/**
 * @brief Тест map_result_to_entities - большой объем данных
 */
TEST_F(MapperTest, MapResultToEntities_LargeDataset)
{
    database_adapter::query_result result;

    const size_t count = 100;
    for(size_t i = 0; i < count; ++i) {
        database_adapter::query_result::row row;
        row["users_id"] = std::to_string(i + 1);
        row["users_name"] = "User " + std::to_string(i + 1);
        row["users_email"] = "user" + std::to_string(i + 1) + "@example.com";
        row["users_age"] = std::to_string(20 + i);
        result.add(row);
    }

    std::vector<TestUser> users = entity_craft::map_result_to_entities(_user_table, result);

    EXPECT_EQ(users.size(), count);
    EXPECT_EQ(users[0].id, 1);
    EXPECT_EQ(users[count - 1].id, static_cast<int>(count));
}

/**
 * @brief Тест merge_entities - объединение с one_to_many зависимостями
 * 
 * Проверяет, что merge_entities правильно объединяет сущности с одинаковым ID,
 * собирая все зависимые сущности из коллекции one_to_many в одну объединенную коллекцию.
 */
TEST_F(MapperTest, MergeEntities_WithOneToManyRelations)
{
    // Создаем несколько сущностей User с одинаковым ID, но разными orders
    TestUserWithOrders user1(1, "User 1", {TestOrder(10, "Order 1"), TestOrder(11, "Order 2")});
    TestUserWithOrders user2(1, "User 1", {TestOrder(12, "Order 3")}); // Тот же ID, но другие orders
    TestUserWithOrders user3(1, "User 1", {TestOrder(10, "Order 1"), TestOrder(13, "Order 4")}); // Дубликат Order 1

    std::vector<TestUserWithOrders> entities = {user1, user2, user3};

    auto user_table = TestUserWithOrders::dto();
    std::vector<TestUserWithOrders> merged = entity_craft::merge_entities(entities, user_table);

    // Должна остаться одна сущность с объединенными orders
    EXPECT_EQ(merged.size(), 1);
    EXPECT_EQ(merged[0].id, 1);
    EXPECT_EQ(merged[0].name, "User 1");

    // Orders должны быть объединены и дедуплицированы по ID
    // Ожидаем: Order 10 (Order 1), Order 11 (Order 2), Order 12 (Order 3), Order 13 (Order 4)
    EXPECT_EQ(merged[0].orders.size(), 4);

    // Проверяем наличие всех уникальных orders
    bool found_order10 = false;
    bool found_order11 = false;
    bool found_order12 = false;
    bool found_order13 = false;

    for(const auto& order : merged[0].orders) {
        if(order.id == 10) {
            EXPECT_EQ(order.description, "Order 1");
            found_order10 = true;
        } else if(order.id == 11) {
            EXPECT_EQ(order.description, "Order 2");
            found_order11 = true;
        } else if(order.id == 12) {
            EXPECT_EQ(order.description, "Order 3");
            found_order12 = true;
        } else if(order.id == 13) {
            EXPECT_EQ(order.description, "Order 4");
            found_order13 = true;
        }
    }

    EXPECT_TRUE(found_order10);
    EXPECT_TRUE(found_order11);
    EXPECT_TRUE(found_order12);
    EXPECT_TRUE(found_order13);
}

/**
 * @brief Тест merge_entities - вложенные структуры с несколькими уровнями one_to_many
 * 
 * Проверяет рекурсивное объединение зависимых сущностей.
 */
TEST_F(MapperTest, MergeEntities_NestedOneToManyRelations)
{
    // Создаем сущности с одинаковым ID, но разными orders
    TestUserWithOrders user1(1, "User 1", {TestOrder(10, "Order 1")});
    TestUserWithOrders user2(1, "User 1", {TestOrder(11, "Order 2")});
    TestUserWithOrders user3(2, "User 2", {TestOrder(20, "Order 3")});
    TestUserWithOrders user4(2, "User 2", {TestOrder(21, "Order 4")});

    std::vector<TestUserWithOrders> entities = {user1, user2, user3, user4};

    auto user_table = TestUserWithOrders::dto();
    auto merged = entity_craft::merge_entities(entities, user_table);

    // Должны остаться 2 сущности (User 1 и User 2)
    EXPECT_EQ(merged.size(), 2);

    // Находим User 1
    auto user1_it = std::find_if(merged.begin(), merged.end(), [](const TestUserWithOrders& u) { return u.id == 1; });
    ASSERT_NE(user1_it, merged.end());
    EXPECT_EQ(user1_it->name, "User 1");
    EXPECT_EQ(user1_it->orders.size(), 2); // Order 1 и Order 2

    // Находим User 2
    auto user2_it = std::find_if(merged.begin(), merged.end(), [](const TestUserWithOrders& u) { return u.id == 2; });
    ASSERT_NE(user2_it, merged.end());
    EXPECT_EQ(user2_it->name, "User 2");
    EXPECT_EQ(user2_it->orders.size(), 2); // Order 3 и Order 4
}

/**
 * @brief Тест merge_entities - пустые коллекции one_to_many
 */
TEST_F(MapperTest, MergeEntities_WithEmptyOneToManyCollections)
{
    TestUserWithOrders user1(1, "User 1", {}); // Пустая коллекция orders
    TestUserWithOrders user2(1, "User 1", {TestOrder(10, "Order 1")}); // С orders

    std::vector<TestUserWithOrders> entities = {user1, user2};

    auto user_table = TestUserWithOrders::dto();
    std::vector<TestUserWithOrders> merged = entity_craft::merge_entities(entities, user_table);

    EXPECT_EQ(merged.size(), 1);
    EXPECT_EQ(merged[0].id, 1);
    EXPECT_EQ(merged[0].orders.size(), 1); // Должен быть один order
    EXPECT_EQ(merged[0].orders[0].id, 10);
    EXPECT_EQ(merged[0].orders[0].description, "Order 1");
}

/**
 * @brief Тест merge_entities - вложенность 3 уровня (User -> Orders -> OrderItems)
 * 
 * Проверяет рекурсивное объединение зависимых сущностей на 3 уровнях вложенности:
 * - Уровень 1: User
 * - Уровень 2: Order (one_to_many от User)
 * - Уровень 3: OrderItem (one_to_many от Order)
 */
TEST_F(MapperTest, MergeEntities_ThreeLevelNesting)
{
    // Создаем сущности с вложенностью 3 уровня
    // User 1 с Order 10, который содержит OrderItem 100 и 101
    TestUserWithOrders user1(1, "User 1", {
        TestOrder(10, "Order 1", {
            TestOrderItem(100, "Item 1", 5),
            TestOrderItem(101, "Item 2", 3)
        })
    });

    // User 1 с Order 10 (дубликат), который содержит OrderItem 102
    TestUserWithOrders user2(1, "User 1", {
        TestOrder(10, "Order 1", {
            TestOrderItem(100, "Item 1", 5), // Дубликат
            TestOrderItem(102, "Item 3", 2)
        })
    });

    // User 1 с Order 11, который содержит OrderItem 103
    TestUserWithOrders user3(1, "User 1", {
        TestOrder(11, "Order 2", {
            TestOrderItem(103, "Item 4", 1)
        })
    });

    // User 2 с Order 20, который содержит OrderItem 200
    TestUserWithOrders user4(2, "User 2", {
        TestOrder(20, "Order 3", {
            TestOrderItem(200, "Item 5", 10)
        })
    });

    std::vector<TestUserWithOrders> entities = {user1, user2, user3, user4};

    auto user_table = TestUserWithOrders::dto();
    std::vector<TestUserWithOrders> merged = entity_craft::merge_entities(entities, user_table);

    // Должны остаться 2 сущности (User 1 и User 2)
    EXPECT_EQ(merged.size(), 2);

    // Находим User 1
    auto user1_it = std::find_if(merged.begin(), merged.end(), [](const TestUserWithOrders& u) { return u.id == 1; });
    ASSERT_NE(user1_it, merged.end());
    EXPECT_EQ(user1_it->name, "User 1");
    
    // User 1 должен иметь 2 orders (Order 10 и Order 11)
    EXPECT_EQ(user1_it->orders.size(), 2);

    // Находим Order 10
    auto order10_it = std::find_if(user1_it->orders.begin(), user1_it->orders.end(), 
        [](const TestOrder& o) { return o.id == 10; });
    ASSERT_NE(order10_it, user1_it->orders.end());
    EXPECT_EQ(order10_it->description, "Order 1");
    
    // Order 10 должен иметь объединенные items: 100, 101, 102 (100 дедуплицирован)
    EXPECT_GE(order10_it->items.size(), 2);
    EXPECT_LE(order10_it->items.size(), 3);

    bool found_item100 = false;
    bool found_item101 = false;
    bool found_item102 = false;

    for(const auto& item : order10_it->items) {
        if(item.id == 100) {
            EXPECT_EQ(item.name, "Item 1");
            EXPECT_EQ(item.quantity, 5);
            found_item100 = true;
        } else if(item.id == 101) {
            EXPECT_EQ(item.name, "Item 2");
            EXPECT_EQ(item.quantity, 3);
            found_item101 = true;
        } else if(item.id == 102) {
            EXPECT_EQ(item.name, "Item 3");
            EXPECT_EQ(item.quantity, 2);
            found_item102 = true;
        }
    }

    EXPECT_TRUE(found_item100);
    EXPECT_TRUE(found_item101);
    EXPECT_TRUE(found_item102);

    // Находим Order 11
    auto order11_it = std::find_if(user1_it->orders.begin(), user1_it->orders.end(), 
        [](const TestOrder& o) { return o.id == 11; });
    ASSERT_NE(order11_it, user1_it->orders.end());
    EXPECT_EQ(order11_it->description, "Order 2");
    EXPECT_EQ(order11_it->items.size(), 1);
    EXPECT_EQ(order11_it->items[0].id, 103);
    EXPECT_EQ(order11_it->items[0].name, "Item 4");
    EXPECT_EQ(order11_it->items[0].quantity, 1);

    // Находим User 2
    auto user2_it = std::find_if(merged.begin(), merged.end(), [](const TestUserWithOrders& u) { return u.id == 2; });
    ASSERT_NE(user2_it, merged.end());
    EXPECT_EQ(user2_it->name, "User 2");
    EXPECT_EQ(user2_it->orders.size(), 1);
    EXPECT_EQ(user2_it->orders[0].id, 20);
    EXPECT_EQ(user2_it->orders[0].description, "Order 3");
    EXPECT_EQ(user2_it->orders[0].items.size(), 1);
    EXPECT_EQ(user2_it->orders[0].items[0].id, 200);
    EXPECT_EQ(user2_it->orders[0].items[0].name, "Item 5");
    EXPECT_EQ(user2_it->orders[0].items[0].quantity, 10);
}

/**
 * @brief Тест merge_entities - вложенность 3 уровня с дубликатами на разных уровнях
 * 
 * Проверяет, что дедупликация работает корректно на всех уровнях вложенности.
 */
TEST_F(MapperTest, MergeEntities_ThreeLevelNestingWithDuplicates)
{
    // User 1 с Order 10, содержащим Item 100
    TestUserWithOrders user1(1, "User 1", {
        TestOrder(10, "Order 1", {
            TestOrderItem(100, "Item 1", 5)
        })
    });

    // User 1 с Order 10 (дубликат), содержащим Item 100 (дубликат) и Item 101
    TestUserWithOrders user2(1, "User 1", {
        TestOrder(10, "Order 1", {
            TestOrderItem(100, "Item 1", 5), // Дубликат Order и Item
            TestOrderItem(101, "Item 2", 3)
        }),
        TestOrder(11, "Order 2", {
            TestOrderItem(200, "Item 3", 2)
        })
    });

    // User 1 с Order 11 (дубликат), содержащим Item 200 (дубликат) и Item 201
    TestUserWithOrders user3(1, "User 1", {
        TestOrder(11, "Order 2", {
            TestOrderItem(200, "Item 3", 2), // Дубликат Order и Item
            TestOrderItem(201, "Item 4", 1)
        })
    });

    std::vector<TestUserWithOrders> entities = {user1, user2, user3};

    auto user_table = TestUserWithOrders::dto();
    std::vector<TestUserWithOrders> merged = entity_craft::merge_entities(entities, user_table);

    EXPECT_EQ(merged.size(), 1);
    EXPECT_EQ(merged[0].id, 1);
    EXPECT_EQ(merged[0].name, "User 1");

    // Должны быть 2 orders (10 и 11)
    EXPECT_EQ(merged[0].orders.size(), 2);

    // Order 10 должен иметь 2 items (100 и 101)
    auto order10_it = std::find_if(merged[0].orders.begin(), merged[0].orders.end(), 
        [](const TestOrder& o) { return o.id == 10; });
    ASSERT_NE(order10_it, merged[0].orders.end());
    EXPECT_EQ(order10_it->items.size(), 2);

    // Order 11 должен иметь 2 items (200 и 201)
    auto order11_it = std::find_if(merged[0].orders.begin(), merged[0].orders.end(), 
        [](const TestOrder& o) { return o.id == 11; });
    ASSERT_NE(order11_it, merged[0].orders.end());
    EXPECT_EQ(order11_it->items.size(), 2);
}
