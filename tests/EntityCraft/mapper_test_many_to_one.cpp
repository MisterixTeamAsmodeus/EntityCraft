#include "EntityCraft/reflection/referencecolumn.h"

#include <EntityCraft/utils/mapper.h>
#include <gtest/gtest.h>

#include <DatabaseAdapter/model/queryresult.hpp>
#include <string>

namespace many_to_one {
/**
 * @brief Тестовая структура для зависимой сущности
 */
struct TestGroup
{
    int id = 0;
    std::string name;

    TestGroup() = default;

    TestGroup(int id, std::string group_name)
        : id(id)
        , name(std::move(group_name))
    {
    }

    bool operator==(const TestGroup& other) const
    {
        return id == other.id && name == other.name;
    }

    static auto dto()
    {
        return entity_craft::make_table<TestGroup>(
            "groups",
            "",
            entity_craft::make_column("id", &TestGroup::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("name", &TestGroup::name, entity_craft::not_null()));
    }
};

/**
 * @brief Тестовая структура User с many_to_one связью к Profile
 */
struct TestUserWithGroupManyToOne
{
    int id = 0;
    std::string name;
    TestGroup group;

    TestUserWithGroupManyToOne() = default;

    static auto dto()
    {
        return entity_craft::make_table<TestUserWithGroupManyToOne>(
            "users",
            "",
            entity_craft::make_column("id", &TestUserWithGroupManyToOne::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("name", &TestUserWithGroupManyToOne::name, entity_craft::not_null()),
            entity_craft::make_reference_column("group_id", &TestUserWithGroupManyToOne::group, TestGroup::dto(), entity_craft::relation_type::many_to_one, entity_craft::not_null()));
    }
};
}
/**
 * @brief Фикстура для тестов mapper с many_to_one связью
 */
class MapperTestManyToOne : public ::testing::Test
{
protected:
    decltype(many_to_one::TestUserWithGroupManyToOne::dto()) _user_table = many_to_one::TestUserWithGroupManyToOne::dto();
};

/**
 * @brief Тест map_row_to_entity_with_dependencies - успешный маппинг many_to_one
 */
TEST_F(MapperTestManyToOne, MapRowToEntityWithDependencies_Basic)
{
    database_adapter::query_result::row row;
    row["users_id"] = "1";
    row["users_name"] = "User With Group";
    row["users_group_id"] = "10"; // Внешний ключ на группу
    row["groups_id"] = "10";      // PK группы должен совпадать с FK пользователя
    row["groups_name"] = "Admins";

    auto user = entity_craft::map_row_to_entity_with_dependencies(_user_table, row);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, "User With Group");
    EXPECT_EQ(user.group, many_to_one::TestGroup(10, "Admins"));
}

/**
 * @brief Тест map_row_to_entity_with_dependencies - несовпадающие ключи many_to_one
 */
TEST_F(MapperTestManyToOne, MapRowToEntityWithDependencies_MismatchedKeys)
{
    database_adapter::query_result::row row;
    row["users_id"] = "2";
    row["users_name"] = "Orphan User";
    row["users_group_id"] = "10"; // FK пользователя
    row["groups_id"] = "11";      // PK группы не совпадает с FK
    row["groups_name"] = "Guests";

    auto user = entity_craft::map_row_to_entity_with_dependencies(_user_table, row);

    // Основная сущность маппится, зависимость должна остаться пустой
    EXPECT_EQ(user.id, 2);
    EXPECT_EQ(user.name, "Orphan User");
    EXPECT_EQ(user.group.id, 0);
    EXPECT_TRUE(user.group.name.empty());
}

/**
 * @brief Тест map_row_to_entity_with_dependencies - отсутствие зависимой строки
 */
TEST_F(MapperTestManyToOne, MapRowToEntityWithDependencies_MissingDependentRow)
{
    database_adapter::query_result::row row;
    row["users_id"] = "3";
    row["users_name"] = "User Without Group";
    row["users_group_id"] = "0"; // FK задан, но строки группы в результате нет

    auto user = entity_craft::map_row_to_entity_with_dependencies(_user_table, row);

    EXPECT_EQ(user.id, 3);
    EXPECT_EQ(user.name, "User Without Group");
    EXPECT_EQ(user.group.id, 0);
    EXPECT_TRUE(user.group.name.empty());
}
