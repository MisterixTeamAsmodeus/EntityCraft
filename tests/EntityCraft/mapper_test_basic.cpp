#include "EntityCraft/reflection/referencecolumn.h"

#include <EntityCraft/utils/mapper.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <DatabaseAdapter/model/queryresult.hpp>
#include <string>
#include <vector>

namespace basic {
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
} // namespace basic

/**
 * @brief Фикстура для тестов mapper
 */
class MapperTestBasic : public ::testing::Test
{
protected:
    decltype(basic::TestUser::dto()) _user_table = basic::TestUser::dto();
};

/**
 * @brief Тест map_row_to_entity - базовый маппинг строки в сущность
 */
TEST_F(MapperTestBasic, MapRowToEntity_Basic)
{
    database_adapter::query_result::row row;
    row["id"] = "1";
    row["name"] = "Test User";
    row["email"] = "test@example.com";
    row["age"] = "25";

    auto user = entity_craft::map_row_to_entity(_user_table, row, false);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, "Test User");
    EXPECT_EQ(user.email, "test@example.com");
    EXPECT_EQ(user.age, 25);
}

/**
 * @brief Тест map_row_to_entity - маппинг с использованием алиасов
 */
TEST_F(MapperTestBasic, MapRowToEntity_WithAlias)
{
    database_adapter::query_result::row row;
    row["users_id"] = "2";
    row["users_name"] = "Alias User";
    row["users_email"] = "alias@example.com";
    row["users_age"] = "30";

    auto user = entity_craft::map_row_to_entity(_user_table, row, true);

    EXPECT_EQ(user.id, 2);
    EXPECT_EQ(user.name, "Alias User");
    EXPECT_EQ(user.email, "alias@example.com");
    EXPECT_EQ(user.age, 30);
}

/**
 * @brief Тест map_row_to_entity - обработка NULL значений
 */
TEST_F(MapperTestBasic, MapRowToEntity_WithNullValues)
{
    database_adapter::query_result::row row;
    row["id"] = "3";
    row["name"] = "Null User";
    row["email"] = NULL_VALUE;
    row["age"] = "35";

    auto user = entity_craft::map_row_to_entity(_user_table, row, false);

    EXPECT_EQ(user.id, 3);
    EXPECT_EQ(user.name, "Null User");
    EXPECT_EQ(user.email, ""); // NULL значения должны быть пропущены
    EXPECT_EQ(user.age, 35);
}

/**
 * @brief Тест map_row_to_entity - обработка пустых значений
 */
TEST_F(MapperTestBasic, MapRowToEntity_WithEmptyValues)
{
    database_adapter::query_result::row row;
    row["id"] = "4";
    row["name"] = "";
    row["email"] = "empty@example.com";
    row["age"] = "40";

    auto user = entity_craft::map_row_to_entity(_user_table, row, false);

    EXPECT_EQ(user.id, 4);
    EXPECT_EQ(user.name, "");
    EXPECT_EQ(user.email, "empty@example.com");
    EXPECT_EQ(user.age, 40);
}

/**
 * @brief Тест map_entity_to_params - базовый маппинг сущности в параметры
 */
TEST_F(MapperTestBasic, MapEntityToParams_Basic)
{
    basic::TestUser user(5, "Param User", "param@example.com", 45);

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
TEST_F(MapperTestBasic, MapEntityToParams_ExcludeAutoIncrement)
{
    basic::TestUser user(6, "No Auto User", "noauto@example.com", 50);

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
TEST_F(MapperTestBasic, MapResultToEntities_Basic)
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

    auto users = entity_craft::map_result_to_entities(_user_table, result);

    EXPECT_EQ(users.size(), 2);
    EXPECT_EQ(users[0].id, 10);
    EXPECT_EQ(users[0].name, "Result User 1");
    EXPECT_EQ(users[1].id, 11);
    EXPECT_EQ(users[1].name, "Result User 2");
}

/**
 * @brief Тест map_result_to_entities - пустой результат
 */
TEST_F(MapperTestBasic, MapResultToEntities_Empty)
{
    database_adapter::query_result result;

    auto users = entity_craft::map_result_to_entities(_user_table, result);

    EXPECT_TRUE(users.empty());
}

/**
 * @brief Тест merge_entities - объединение сущностей с одинаковыми ключами
 */
TEST_F(MapperTestBasic, MergeEntities_Basic)
{
    basic::TestUser user1(1, "User 1", "user1@example.com", 25);
    basic::TestUser user2(1, "User 1", "user1@example.com", 25); // Тот же ID

    std::vector<basic::TestUser> entities = { user1, user2 };

    auto merged = entity_craft::merge_entities(entities, _user_table);

    // Должна остаться одна сущность
    EXPECT_EQ(merged.size(), 1);
    EXPECT_EQ(merged[0].id, 1);
}

/**
 * @brief Тест merge_entities - объединение сущностей с разными ключами
 */
TEST_F(MapperTestBasic, MergeEntities_DifferentKeys)
{
    basic::TestUser user1(1, "User 1", "user1@example.com", 25);
    basic::TestUser user2(2, "User 2", "user2@example.com", 30);

    std::vector<basic::TestUser> entities = { user1, user2 };

    auto merged = entity_craft::merge_entities(entities, _user_table);

    // Должны остаться обе сущности
    EXPECT_EQ(merged.size(), 2);
}

/**
 * @brief Тест merge_entities - пустой вектор
 */
TEST_F(MapperTestBasic, MergeEntities_Empty)
{
    std::vector<basic::TestUser> entities;

    auto merged = entity_craft::merge_entities(entities, _user_table);

    EXPECT_TRUE(merged.empty());
}

/**
 * @brief Тест map_row_to_entity_with_dependencies - базовый маппинг без зависимостей
 */
TEST_F(MapperTestBasic, MapRowToEntityWithDependencies_Basic)
{
    database_adapter::query_result::row row;
    row["users_id"] = "1";
    row["users_name"] = "Dependency User";
    row["users_email"] = "dependency@example.com";
    row["users_age"] = "25";

    auto user = entity_craft::map_row_to_entity_with_dependencies(_user_table, row);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, "Dependency User");
    EXPECT_EQ(user.email, "dependency@example.com");
    EXPECT_EQ(user.age, 25);
}

/**
 * @brief Тест map_entity_to_params - обработка NULL значений
 */
TEST_F(MapperTestBasic, MapEntityToParams_WithNullValues)
{
    basic::TestUser user;
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
TEST_F(MapperTestBasic, MapRowToEntity_MissingColumns)
{
    database_adapter::query_result::row row;
    row["id"] = "1";
    row["name"] = "Partial User";
    // email и age отсутствуют

    auto user = entity_craft::map_row_to_entity(_user_table, row, false);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, "Partial User");
    EXPECT_EQ(user.email, ""); // Не заполнено, так как колонка отсутствует
    EXPECT_EQ(user.age, 0); // Не заполнено, так как колонка отсутствует
}

/**
 * @brief Тест map_row_to_entity - строка с лишними колонками
 */
TEST_F(MapperTestBasic, MapRowToEntity_ExtraColumns)
{
    database_adapter::query_result::row row;
    row["id"] = "1";
    row["name"] = "Extra User";
    row["email"] = "extra@example.com";
    row["age"] = "25";
    row["extra_column"] = "extra_value"; // Лишняя колонка

    auto user = entity_craft::map_row_to_entity(_user_table, row, false);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, "Extra User");
    EXPECT_EQ(user.email, "extra@example.com");
    EXPECT_EQ(user.age, 25);
    // Лишняя колонка игнорируется
}

/**
 * @brief Тест map_row_to_entity - пустая строка row
 */
TEST_F(MapperTestBasic, MapRowToEntity_EmptyRow)
{
    database_adapter::query_result::row row;

    auto user = entity_craft::map_row_to_entity(_user_table, row, false);

    EXPECT_EQ(user.id, 0);
    EXPECT_TRUE(user.name.empty());
    EXPECT_TRUE(user.email.empty());
    EXPECT_EQ(user.age, 0);
}

/**
 * @brief Тест map_row_to_entity - NULL_VALUE и пустая строка (краевой случай)
 */
TEST_F(MapperTestBasic, MapRowToEntity_NullAndEmptyString)
{
    database_adapter::query_result::row row;
    row["id"] = "1";
    row["name"] = NULL_VALUE; // NULL значение
    row["email"] = ""; // Пустая строка (не NULL)
    row["age"] = "25";

    auto user = entity_craft::map_row_to_entity(_user_table, row, false);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, ""); // NULL_VALUE пропускается
    EXPECT_EQ(user.email, ""); // Пустая строка обрабатывается
    EXPECT_EQ(user.age, 25);
}

/**
 * @brief Тест merge_entities - множественные дубликаты
 */
TEST_F(MapperTestBasic, MergeEntities_MultipleDuplicates)
{
    basic::TestUser user1(1, "User 1", "user1@example.com", 25);
    basic::TestUser user2(1, "User 1", "user1@example.com", 25); // Дубликат
    basic::TestUser user3(1, "User 1", "user1@example.com", 25); // Дубликат
    basic::TestUser user4(2, "User 2", "user2@example.com", 30);

    std::vector<basic::TestUser> entities = { user1, user2, user3, user4 };

    auto merged = entity_craft::merge_entities(entities, _user_table);

    // Должны остаться 2 сущности (1 и 2)
    EXPECT_EQ(merged.size(), 2);
    EXPECT_EQ(merged[0].id, 1);
    EXPECT_EQ(merged[1].id, 2);
}

/**
 * @brief Тест map_result_to_entities - большой объем данных
 */
TEST_F(MapperTestBasic, MapResultToEntities_LargeDataset)
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

    auto users = entity_craft::map_result_to_entities(_user_table, result);

    EXPECT_EQ(users.size(), count);
    EXPECT_EQ(users[0].id, 1);
    EXPECT_EQ(users[count - 1].id, static_cast<int>(count));
}
