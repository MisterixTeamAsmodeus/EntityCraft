#include "EntityCraft/reflection/referencecolumn.h"

#include <EntityCraft/utils/mapper.h>
#include <gtest/gtest.h>

#include <DatabaseAdapter/model/queryresult.hpp>
#include <string>

namespace one_to_one {
/**
 * @brief Тестовая структура Profile для one_to_one связей
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
 * @brief Сущность User с внешним ключом на Profile (one_to_one)
 */
struct TestUser
{
    int id = 0;
    std::string name;
    TestProfile profile;

    TestUser() = default;

    static auto dto()
    {
        return entity_craft::make_table<TestUser>(
            "users_one_to_one",
            "",
            entity_craft::make_column("id", &TestUser::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("name", &TestUser::name, entity_craft::not_null()),
            entity_craft::make_reference_column(
                "profile_id",
                &TestUser::profile,
                TestProfile::dto(),
                entity_craft::relation_type::one_to_one,
                entity_craft::not_null()));
    }
};

} // namespace one_to_one

/**
 * @brief Фикстура для one_to_one маппинга
 */
class MapperTestOneToOne : public ::testing::Test
{
protected:
    decltype(one_to_one::TestUser::dto()) _user_table = one_to_one::TestUser::dto();
};

/**
 * @brief Успешный маппинг one_to_one по совпадающим ключам
 */
TEST_F(MapperTestOneToOne, MapRowToEntityWithDependencies_Basic)
{
    database_adapter::query_result::row row;
    row["users_one_to_one_id"] = "1";
    row["users_one_to_one_name"] = "User With Profile";
    row["users_one_to_one_profile_id"] = "10"; // FK на профиль
    row["profiles_id"] = "10"; // PK профиля совпадает с FK
    row["profiles_user_id"] = "1";
    row["profiles_bio"] = "Test bio";

    auto user = entity_craft::map_row_to_entity_with_dependencies(_user_table, row);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, "User With Profile");
    EXPECT_EQ(user.profile, one_to_one::TestProfile(10, 1, "Test bio"));
}

/**
 * @brief one_to_one: FK основной сущности не совпадает с PK зависимой
 */
TEST_F(MapperTestOneToOne, MapRowToEntityWithDependencies_MismatchedKeys)
{
    database_adapter::query_result::row row;
    row["users_one_to_one_id"] = "2";
    row["users_one_to_one_name"] = "User With Wrong Profile";
    row["users_one_to_one_profile_id"] = "20"; // FK пользователя
    row["profiles_id"] = "21"; // PK профиля не совпадает
    row["profiles_user_id"] = "2";
    row["profiles_bio"] = "Should be skipped";

    auto user = entity_craft::map_row_to_entity_with_dependencies(_user_table, row);

    EXPECT_EQ(user.id, 2);
    EXPECT_EQ(user.name, "User With Wrong Profile");
    EXPECT_EQ(user.profile.id, 0);
    EXPECT_EQ(user.profile.user_id, 0);
    EXPECT_TRUE(user.profile.bio.empty());
}

/**
 * @brief one_to_one: отсутствуют данные зависимой сущности
 */
TEST_F(MapperTestOneToOne, MapRowToEntityWithDependencies_MissingDependentRow)
{
    database_adapter::query_result::row row;
    row["users_one_to_one_id"] = "3";
    row["users_one_to_one_name"] = "User Without Profile";
    row["users_one_to_one_profile_id"] = "30"; // FK задан, но профиль не пришел

    auto user = entity_craft::map_row_to_entity_with_dependencies(_user_table, row);

    EXPECT_EQ(user.id, 3);
    EXPECT_EQ(user.name, "User Without Profile");
    EXPECT_EQ(user.profile.id, 0);
    EXPECT_EQ(user.profile.user_id, 0);
    EXPECT_TRUE(user.profile.bio.empty());
}