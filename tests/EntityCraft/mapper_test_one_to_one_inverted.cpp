#include "EntityCraft/reflection/referencecolumn.h"

#include <EntityCraft/utils/mapper.h>
#include <gtest/gtest.h>

#include <DatabaseAdapter/model/queryresult.hpp>
#include <string>

namespace one_to_one_inverted {
/**
 * @brief Тестовая структура Profile для one_to_one_inverted тестов
 */
struct TestProfile
{
    int id = 0;
    std::string bio;

    TestProfile() = default;

    TestProfile(int id, std::string bio)
        : id(id)
        , bio(std::move(bio))
    {
    }

    bool operator==(const TestProfile& other) const
    {
        return id == other.id && bio == other.bio;
    }

    static auto dto()
    {
        return entity_craft::make_table<TestProfile>(
            "profiles",
            "",
            entity_craft::make_column("id", &TestProfile::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("bio", &TestProfile::bio, entity_craft::not_null()));
    }
};

/**
 * @brief Тестовая структура User для one_to_one_inverted тестов
 */
struct TestUser
{
    int id = 0;
    std::string name;
    std::string email;
    int age = 0;
    TestProfile profile;

    TestUser() = default;

    static auto dto()
    {
        return entity_craft::make_table<TestUser>(
            "users",
            "",
            entity_craft::make_column("id", &TestUser::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("name", &TestUser::name, entity_craft::not_null()),
            entity_craft::make_column("email", &TestUser::email, entity_craft::not_null()),
            entity_craft::make_column("age", &TestUser::age, entity_craft::not_null()),
            entity_craft::make_reference_column("profile_id", &TestUser::profile, TestProfile::dto(), entity_craft::relation_type::one_to_one_inverted, entity_craft::not_null()));
    }
};
} // namespace one_to_one_inverted

/**
 * @brief Фикстура для тестов mapper с one_to_one_inverted связью
 */
class MapperTestOneToOneInverted : public ::testing::Test
{
protected:
    decltype(one_to_one_inverted::TestUser::dto()) _user_table = one_to_one_inverted::TestUser::dto();
};

/**
 * @brief Успешный маппинг one_to_one_inverted (FK в зависимой таблице указывает на PK основной)
 */
TEST_F(MapperTestOneToOneInverted, MapRowToEntityWithDependencies_InvertedBasic)
{
    database_adapter::query_result::row row;
    row["users_id"] = "1";
    row["users_name"] = "User With Profile Inverted";
    row["users_email"] = "user@example.com";
    row["users_age"] = "30";

    row["profiles_id"] = "10";
    row["profiles_profile_id"] = "1"; // FK зависимой таблицы на PK пользователя
    row["profiles_bio"] = "Inverted bio";

    auto user = entity_craft::map_row_to_entity_with_dependencies(_user_table, row);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, "User With Profile Inverted");
    EXPECT_EQ(user.email, "user@example.com");
    EXPECT_EQ(user.age, 30);
    EXPECT_EQ(user.profile, one_to_one_inverted::TestProfile(10, "Inverted bio"));
}

/**
 * @brief one_to_one_inverted: FK зависимой сущности не совпадает с PK основной
 */
TEST_F(MapperTestOneToOneInverted, MapRowToEntityWithDependencies_InvertedMismatchedKeys)
{
    database_adapter::query_result::row row;
    row["users_id"] = "2";
    row["users_name"] = "User With Wrong Inverted Profile";
    row["users_email"] = "wrong@example.com";
    row["users_age"] = "25";

    row["profiles_id"] = "20";
    row["profiles_profile_id"] = "999"; // FK профиля не совпадает с PK пользователя
    row["profiles_bio"] = "Should be skipped";

    auto user = entity_craft::map_row_to_entity_with_dependencies(_user_table, row);

    EXPECT_EQ(user.id, 2);
    EXPECT_EQ(user.name, "User With Wrong Inverted Profile");
    EXPECT_EQ(user.email, "wrong@example.com");
    EXPECT_EQ(user.age, 25);
    EXPECT_EQ(user.profile.id, 0);
    EXPECT_TRUE(user.profile.bio.empty());
}

/**
 * @brief one_to_one_inverted: отсутствует строка зависимой сущности
 */
TEST_F(MapperTestOneToOneInverted, MapRowToEntityWithDependencies_InvertedMissingDependentRow)
{
    database_adapter::query_result::row row;
    row["users_id"] = "3";
    row["users_name"] = "User Without Inverted Profile";
    row["users_email"] = "nofk@example.com";
    row["users_age"] = "40";

    auto user = entity_craft::map_row_to_entity_with_dependencies(_user_table, row);

    EXPECT_EQ(user.id, 3);
    EXPECT_EQ(user.name, "User Without Inverted Profile");
    EXPECT_EQ(user.email, "nofk@example.com");
    EXPECT_EQ(user.age, 40);
    EXPECT_EQ(user.profile.id, 0);
    EXPECT_TRUE(user.profile.bio.empty());
}

/**
 * @brief one_to_one_inverted: профиль без внешнего ключа на пользователя
 */
TEST_F(MapperTestOneToOneInverted, MapRowToEntityWithDependencies_InvertedMissingForeignKey)
{
    database_adapter::query_result::row row;
    row["users_id"] = "4";
    row["users_name"] = "User Inverted Profile Without FK";
    row["users_email"] = "missingfk@example.com";
    row["users_age"] = "28";

    row["profiles_id"] = "40";
    row["profiles_bio"] = "Inverted without fk";
    // отсутствует profiles_profile_id, поэтому профиль не должен замапиться

    auto user = entity_craft::map_row_to_entity_with_dependencies(_user_table, row);

    EXPECT_EQ(user.id, 4);
    EXPECT_EQ(user.name, "User Inverted Profile Without FK");
    EXPECT_EQ(user.email, "missingfk@example.com");
    EXPECT_EQ(user.age, 28);
    EXPECT_EQ(user.profile.id, 0);
    EXPECT_TRUE(user.profile.bio.empty());
}