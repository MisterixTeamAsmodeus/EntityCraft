#include "EntityCraft/reflection/referencecolumn.h"

#include <EntityCraft/utils/mapper.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <DatabaseAdapter/model/queryresult.hpp>
#include <string>
#include <vector>

namespace one_to_many {
/**
 * @brief Тестовая структура для элементов заказа (3-й уровень вложенности)
 */
struct TestOrderItem
{
    int id = 0;
    std::string name;
    int quantity = 0;
    int order_id = 0;

    TestOrderItem() = default;

    TestOrderItem(int id, std::string name, int quantity, int order_id = 0)
        : id(id)
        , name(std::move(name))
        , quantity(quantity)
        , order_id(order_id)
    {
    }

    bool operator==(const TestOrderItem& other) const
    {
        return id == other.id && name == other.name && quantity == other.quantity && order_id == other.order_id;
    }

    static auto dto()
    {
        return entity_craft::make_table<TestOrderItem>(
            "order_items",
            "", entity_craft::make_column("id", &TestOrderItem::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("order_id", &TestOrderItem::order_id, entity_craft::not_null()),
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
    int user_id = 0;
    std::string description;
    std::vector<TestOrderItem> items;

    TestOrder() = default;

    TestOrder(int id, int user_id, std::string description)
        : id(id)
        , user_id(user_id)
        , description(std::move(description))
    {
    }

    TestOrder(int id, int user_id, std::string description, std::vector<TestOrderItem> items)
        : id(id)
        , user_id(user_id)
        , description(std::move(description))
        , items(std::move(items))
    {
    }

    bool operator==(const TestOrder& other) const
    {
        return id == other.id && user_id == other.user_id && description == other.description;
    }

    static auto dto()
    {
        return entity_craft::make_table<TestOrder>(
            "orders",
            "",
            entity_craft::make_column("id", &TestOrder::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("user_id", &TestOrder::user_id, entity_craft::not_null()),
            entity_craft::make_column("description", &TestOrder::description, entity_craft::not_null()),
            entity_craft::make_reference_column("order_id", &TestOrder::items, TestOrderItem::dto(), entity_craft::relation_type::one_to_many));
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
        return entity_craft::make_table<TestUserWithOrders>(
            "users",
            "",
            entity_craft::make_column("id", &TestUserWithOrders::id, entity_craft::primary_key_auto_increment()),
            entity_craft::make_column("name", &TestUserWithOrders::name, entity_craft::not_null()),
            entity_craft::make_reference_column("user_id", &TestUserWithOrders::orders, TestOrder::dto(), entity_craft::relation_type::one_to_many));
    }
};

} // namespace one_to_many

/**
 * @brief Фикстура для тестов mapper с one_to_many связью
 */
class MapperTestOneToMany : public ::testing::Test
{
protected:
    decltype(one_to_many::TestUserWithOrders::dto()) _user_table = one_to_many::TestUserWithOrders::dto();
    decltype(one_to_many::TestOrder::dto()) _order_table = one_to_many::TestOrder::dto();
    decltype(one_to_many::TestOrderItem::dto()) _order_item_table = one_to_many::TestOrderItem::dto();
};

/**
 * @brief Тест map_row_to_entity_with_dependencies - успешный маппинг one_to_many с одним элементом
 */
TEST_F(MapperTestOneToMany, MapRowToEntityWithDependencies_Basic)
{
    database_adapter::query_result::row row;
    row["orders_id"] = "1";
    row["orders_user_id"] = "10";
    row["orders_description"] = "Test Order";
    row["order_items_id"] = "100"; // PK элемента заказа
    row["order_items_order_id"] = "1"; // FK элемента должен совпадать с orders_id
    row["order_items_name"] = "Item 1";
    row["order_items_quantity"] = "5";

    auto order = entity_craft::map_row_to_entity_with_dependencies(_order_table, row);

    EXPECT_EQ(order.id, 1);
    EXPECT_EQ(order.user_id, 10);
    EXPECT_EQ(order.description, "Test Order");
    EXPECT_EQ(order.items.size(), 1);
    EXPECT_EQ(order.items[0].id, 100);
    EXPECT_EQ(order.items[0].order_id, 1);
    EXPECT_EQ(order.items[0].name, "Item 1");
    EXPECT_EQ(order.items[0].quantity, 5);
}

/**
 * @brief Тест map_row_to_entity_with_dependencies - one_to_many без зависимых элементов
 */
TEST_F(MapperTestOneToMany, MapRowToEntityWithDependencies_NoDependentItems)
{
    database_adapter::query_result::row row;
    row["orders_id"] = "2";
    row["orders_user_id"] = "20";
    row["orders_description"] = "Empty Order";
    // Нет данных о элементах заказа

    auto order = entity_craft::map_row_to_entity_with_dependencies(_order_table, row);

    EXPECT_EQ(order.id, 2);
    EXPECT_EQ(order.user_id, 20);
    EXPECT_EQ(order.description, "Empty Order");
    EXPECT_TRUE(order.items.empty());
}

/**
 * @brief Тест map_row_to_entity_with_dependencies - one_to_many с несовпадающими ключами
 */
TEST_F(MapperTestOneToMany, MapRowToEntityWithDependencies_MismatchedKeys)
{
    database_adapter::query_result::row row;
    row["orders_id"] = "3";
    row["orders_user_id"] = "30";
    row["orders_description"] = "Order With Mismatch";
    row["order_items_id"] = "200";
    row["order_items_order_id"] = "999"; // FK не совпадает с orders_id
    row["order_items_name"] = "Item 2";
    row["order_items_quantity"] = "10";

    auto order = entity_craft::map_row_to_entity_with_dependencies(_order_table, row);

    EXPECT_EQ(order.id, 3);
    EXPECT_EQ(order.user_id, 30);
    EXPECT_EQ(order.description, "Order With Mismatch");
    // Элемент не должен быть добавлен, так как ключи не совпадают
    EXPECT_TRUE(order.items.empty());
}

/**
 * @brief Тест extract_dependent_entities - one_to_many связь
 */
TEST_F(MapperTestOneToMany, ExtractDependentEntities_OneToMany)
{
    database_adapter::query_result::row row;
    row["orders_id"] = "1"; // main_pk_column - PK основной таблицы
    row["orders_user_id"] = "10";
    row["orders_description"] = "Test Order";
    row["order_items_id"] = "100"; // PK зависимой таблицы
    row["order_items_order_id"] = "1"; // dep_fk_column - FK должен совпадать с orders_id
    row["order_items_name"] = "Item 1";
    row["order_items_quantity"] = "5";

    auto ref_column = entity_craft::make_reference_column(
        "items",
        &one_to_many::TestOrder::items,
        _order_item_table,
        entity_craft::relation_type::one_to_many);

    auto item = entity_craft::extract_dependent_entities(
        std::string("orders_id"), // main_pk_column
        std::string("orders_items"), // main_fk_column (не используется для one_to_many)
        std::string("order_items_id"), // dep_pk_column
        std::string("order_items_order_id"), // dep_fk_column
        ref_column,
        row);

    EXPECT_EQ(item.id, 100);
    EXPECT_EQ(item.order_id, 1);
    EXPECT_EQ(item.name, "Item 1");
    EXPECT_EQ(item.quantity, 5);
}

/**
 * @brief Тест extract_dependent_entities - one_to_many с несовпадающими ключами
 */
TEST_F(MapperTestOneToMany, ExtractDependentEntities_OneToMany_NonMatchingKeys)
{
    database_adapter::query_result::row row;
    row["orders_id"] = "1";
    row["orders_user_id"] = "10";
    row["orders_description"] = "Test Order";
    row["order_items_id"] = "100";
    row["order_items_order_id"] = "2"; // Не совпадает с orders_id
    row["order_items_name"] = "Item 1";
    row["order_items_quantity"] = "5";

    auto ref_column = entity_craft::make_reference_column(
        "items",
        &one_to_many::TestOrder::items,
        _order_item_table,
        entity_craft::relation_type::one_to_many);

    auto item = entity_craft::extract_dependent_entities(
        std::string("orders_id"),
        std::string("orders_items"),
        std::string("order_items_id"),
        std::string("order_items_order_id"),
        ref_column,
        row);

    // Должна вернуться пустая сущность, так как orders_id (1) != order_items_order_id (2)
    EXPECT_EQ(item.id, 0);
    EXPECT_EQ(item.order_id, 0);
    EXPECT_TRUE(item.name.empty());
    EXPECT_EQ(item.quantity, 0);
}

/**
 * @brief Тест merge_entities - объединение сущностей с one_to_many связью
 */
TEST_F(MapperTestOneToMany, MergeEntities_OneToMany)
{
    // Создаем несколько строк с одним заказом и разными элементами
    one_to_many::TestOrder order1(1, 10, "Order 1");
    order1.items.push_back(one_to_many::TestOrderItem(100, "Item 1", 5, 1));

    one_to_many::TestOrder order2(1, 10, "Order 1"); // Тот же ID
    order2.items.push_back(one_to_many::TestOrderItem(101, "Item 2", 3, 1));

    std::vector<one_to_many::TestOrder> entities = { order1, order2 };

    auto merged = entity_craft::merge_entities(entities, _order_table);

    // Должна остаться одна сущность с объединенными элементами
    EXPECT_EQ(merged.size(), 1);
    EXPECT_EQ(merged[0].id, 1);
    EXPECT_EQ(merged[0].user_id, 10);
    EXPECT_EQ(merged[0].description, "Order 1");
    EXPECT_EQ(merged[0].items.size(), 2);
}

/**
 * @brief Тест merge_entities - объединение сущностей с разными ключами
 */
TEST_F(MapperTestOneToMany, MergeEntities_DifferentKeys)
{
    one_to_many::TestOrder order1(1, 10, "Order 1");
    order1.items.push_back(one_to_many::TestOrderItem(100, "Item 1", 5, 1));

    one_to_many::TestOrder order2(2, 20, "Order 2");
    order2.items.push_back(one_to_many::TestOrderItem(101, "Item 2", 3, 2));

    std::vector<one_to_many::TestOrder> entities = { order1, order2 };

    auto merged = entity_craft::merge_entities(entities, _order_table);

    // Должны остаться обе сущности
    EXPECT_EQ(merged.size(), 2);
    EXPECT_EQ(merged[0].id, 1);
    EXPECT_EQ(merged[1].id, 2);
}

/**
 * @brief Тест merge_entities - пустой вектор
 */
TEST_F(MapperTestOneToMany, MergeEntities_Empty)
{
    std::vector<one_to_many::TestOrder> entities;

    auto merged = entity_craft::merge_entities(entities, _order_table);

    EXPECT_TRUE(merged.empty());
}

/**
 * @brief Тест merge_entities - множественные дубликаты с one_to_many
 */
TEST_F(MapperTestOneToMany, MergeEntities_MultipleDuplicates)
{
    one_to_many::TestOrder order1(1, 10, "Order 1");
    order1.items.push_back(one_to_many::TestOrderItem(100, "Item 1", 5, 1));

    one_to_many::TestOrder order2(1, 10, "Order 1");
    order2.items.push_back(one_to_many::TestOrderItem(101, "Item 2", 3, 1));

    one_to_many::TestOrder order3(1, 10, "Order 1");
    order3.items.push_back(one_to_many::TestOrderItem(102, "Item 3", 2, 1));

    one_to_many::TestOrder order4(2, 20, "Order 2");
    order4.items.push_back(one_to_many::TestOrderItem(103, "Item 4", 1, 2));

    std::vector<one_to_many::TestOrder> entities = { order1, order2, order3, order4 };

    auto merged = entity_craft::merge_entities(entities, _order_table);

    // Должны остаться 2 сущности (1 и 2)
    EXPECT_EQ(merged.size(), 2);
    EXPECT_EQ(merged[0].id, 1);
    EXPECT_EQ(merged[0].items.size(), 3); // Все элементы из order1, order2, order3
    EXPECT_EQ(merged[1].id, 2);
    EXPECT_EQ(merged[1].items.size(), 1);
}

/**
 * @brief Тест map_result_to_entities - маппинг результата с one_to_many связью
 */
TEST_F(MapperTestOneToMany, MapResultToEntities_OneToMany)
{
    database_adapter::query_result result;

    // Первая строка: заказ с элементом
    database_adapter::query_result::row row1;
    row1["orders_id"] = "1";
    row1["orders_user_id"] = "10";
    row1["orders_description"] = "Order 1";
    row1["order_items_id"] = "100";
    row1["order_items_order_id"] = "1";
    row1["order_items_name"] = "Item 1";
    row1["order_items_quantity"] = "5";
    result.add(row1);

    // Вторая строка: тот же заказ с другим элементом
    database_adapter::query_result::row row2;
    row2["orders_id"] = "1";
    row2["orders_user_id"] = "10";
    row2["orders_description"] = "Order 1";
    row2["order_items_id"] = "101";
    row2["order_items_order_id"] = "1";
    row2["order_items_name"] = "Item 2";
    row2["order_items_quantity"] = "3";
    result.add(row2);

    auto orders = entity_craft::map_result_to_entities(_order_table, result);

    // Должно быть 2 сущности (по одной на строку)
    EXPECT_EQ(orders.size(), 2);
    EXPECT_EQ(orders[0].id, 1);
    EXPECT_EQ(orders[0].items.size(), 1);
    EXPECT_EQ(orders[1].id, 1);
    EXPECT_EQ(orders[1].items.size(), 1);
}

/**
 * @brief Тест map_result_to_entities + merge_entities - полный цикл маппинга и объединения
 */
TEST_F(MapperTestOneToMany, MapResultToEntitiesAndMerge_OneToMany)
{
    database_adapter::query_result result;

    // Первая строка: заказ с элементом
    database_adapter::query_result::row row1;
    row1["orders_id"] = "1";
    row1["orders_user_id"] = "10";
    row1["orders_description"] = "Order 1";
    row1["order_items_id"] = "100";
    row1["order_items_order_id"] = "1";
    row1["order_items_name"] = "Item 1";
    row1["order_items_quantity"] = "5";
    result.add(row1);

    // Вторая строка: тот же заказ с другим элементом
    database_adapter::query_result::row row2;
    row2["orders_id"] = "1";
    row2["orders_user_id"] = "10";
    row2["orders_description"] = "Order 1";
    row2["order_items_id"] = "101";
    row2["order_items_order_id"] = "1";
    row2["order_items_name"] = "Item 2";
    row2["order_items_quantity"] = "3";
    result.add(row2);

    // Третья строка: другой заказ
    database_adapter::query_result::row row3;
    row3["orders_id"] = "2";
    row3["orders_user_id"] = "20";
    row3["orders_description"] = "Order 2";
    row3["order_items_id"] = "102";
    row3["order_items_order_id"] = "2";
    row3["order_items_name"] = "Item 3";
    row3["order_items_quantity"] = "10";
    result.add(row3);

    auto orders = entity_craft::map_result_to_entities(_order_table, result);
    auto merged = entity_craft::merge_entities(orders, _order_table);

    // После объединения должно быть 2 заказа
    EXPECT_EQ(merged.size(), 2);

    // Первый заказ должен иметь 2 элемента
    EXPECT_EQ(merged[0].id, 1);
    EXPECT_EQ(merged[0].items.size(), 2);

    // Второй заказ должен иметь 1 элемент
    EXPECT_EQ(merged[1].id, 2);
    EXPECT_EQ(merged[1].items.size(), 1);
}

/**
 * @brief Тест map_row_to_entity_with_dependencies - второй уровень вложенности (User -> Order -> OrderItem)
 */
TEST_F(MapperTestOneToMany, MapRowToEntityWithDependencies_TwoLevelsNested)
{
    database_adapter::query_result::row row;
    row["users_id"] = "1";
    row["users_name"] = "User 1";
    row["orders_id"] = "10"; // PK заказа
    row["orders_user_id"] = "1"; // FK заказа должен совпадать с users_id
    row["orders_description"] = "Order 1";
    row["order_items_id"] = "100"; // PK элемента заказа
    row["order_items_order_id"] = "10"; // FK элемента должен совпадать с orders_id
    row["order_items_name"] = "Item 1";
    row["order_items_quantity"] = "5";

    auto user = entity_craft::map_row_to_entity_with_dependencies(_user_table, row);

    EXPECT_EQ(user.id, 1);
    EXPECT_EQ(user.name, "User 1");
    EXPECT_EQ(user.orders.size(), 1);
    EXPECT_EQ(user.orders[0].id, 10);
    EXPECT_EQ(user.orders[0].user_id, 1);
    EXPECT_EQ(user.orders[0].description, "Order 1");
    EXPECT_EQ(user.orders[0].items.size(), 1);
    EXPECT_EQ(user.orders[0].items[0].id, 100);
    EXPECT_EQ(user.orders[0].items[0].order_id, 10);
    EXPECT_EQ(user.orders[0].items[0].name, "Item 1");
    EXPECT_EQ(user.orders[0].items[0].quantity, 5);
}

/**
 * @brief Тест map_row_to_entity_with_dependencies - второй уровень вложенности без элементов заказа
 */
TEST_F(MapperTestOneToMany, MapRowToEntityWithDependencies_TwoLevelsNested_NoOrderItems)
{
    database_adapter::query_result::row row;
    row["users_id"] = "2";
    row["users_name"] = "User 2";
    row["orders_id"] = "20";
    row["orders_user_id"] = "2";
    row["orders_description"] = "Order 2";
    // Нет данных о элементах заказа

    auto user = entity_craft::map_row_to_entity_with_dependencies(_user_table, row);

    EXPECT_EQ(user.id, 2);
    EXPECT_EQ(user.name, "User 2");
    EXPECT_EQ(user.orders.size(), 1);
    EXPECT_EQ(user.orders[0].id, 20);
    EXPECT_EQ(user.orders[0].user_id, 2);
    EXPECT_EQ(user.orders[0].description, "Order 2");
    EXPECT_TRUE(user.orders[0].items.empty());
}

/**
 * @brief Тест map_row_to_entity_with_dependencies - второй уровень вложенности без заказов
 */
TEST_F(MapperTestOneToMany, MapRowToEntityWithDependencies_TwoLevelsNested_NoOrders)
{
    database_adapter::query_result::row row;
    row["users_id"] = "3";
    row["users_name"] = "User 3";
    // Нет данных о заказах

    auto user = entity_craft::map_row_to_entity_with_dependencies(_user_table, row);

    EXPECT_EQ(user.id, 3);
    EXPECT_EQ(user.name, "User 3");
    EXPECT_TRUE(user.orders.empty());
}

/**
 * @brief Тест map_row_to_entity_with_dependencies - второй уровень вложенности с несовпадающими ключами на первом уровне
 */
TEST_F(MapperTestOneToMany, MapRowToEntityWithDependencies_TwoLevelsNested_MismatchedFirstLevel)
{
    database_adapter::query_result::row row;
    row["users_id"] = "4";
    row["users_name"] = "User 4";
    row["orders_id"] = "30";
    row["orders_user_id"] = "999"; // FK не совпадает с users_id
    row["orders_description"] = "Order 3";
    row["order_items_id"] = "200";
    row["order_items_order_id"] = "30";
    row["order_items_name"] = "Item 2";
    row["order_items_quantity"] = "10";

    auto user = entity_craft::map_row_to_entity_with_dependencies(_user_table, row);

    EXPECT_EQ(user.id, 4);
    EXPECT_EQ(user.name, "User 4");
    // Заказ не должен быть добавлен, так как ключи не совпадают
    EXPECT_TRUE(user.orders.empty());
}

/**
 * @brief Тест map_row_to_entity_with_dependencies - второй уровень вложенности с несовпадающими ключами на втором уровне
 */
TEST_F(MapperTestOneToMany, MapRowToEntityWithDependencies_TwoLevelsNested_MismatchedSecondLevel)
{
    database_adapter::query_result::row row;
    row["users_id"] = "5";
    row["users_name"] = "User 5";
    row["orders_id"] = "40";
    row["orders_user_id"] = "5";
    row["orders_description"] = "Order 4";
    row["order_items_id"] = "300";
    row["order_items_order_id"] = "999"; // FK не совпадает с orders_id
    row["order_items_name"] = "Item 3";
    row["order_items_quantity"] = "15";

    auto user = entity_craft::map_row_to_entity_with_dependencies(_user_table, row);

    EXPECT_EQ(user.id, 5);
    EXPECT_EQ(user.name, "User 5");
    EXPECT_EQ(user.orders.size(), 1);
    EXPECT_EQ(user.orders[0].id, 40);
    EXPECT_EQ(user.orders[0].user_id, 5);
    EXPECT_EQ(user.orders[0].description, "Order 4");
    // Элемент заказа не должен быть добавлен, так как ключи не совпадают
    EXPECT_TRUE(user.orders[0].items.empty());
}

/**
 * @brief Тест map_result_to_entities + merge_entities - второй уровень вложенности
 */
TEST_F(MapperTestOneToMany, MapResultToEntitiesAndMerge_TwoLevelsNested)
{
    database_adapter::query_result result;

    // Первая строка: пользователь с заказом и элементом
    database_adapter::query_result::row row1;
    row1["users_id"] = "1";
    row1["users_name"] = "User 1";
    row1["orders_id"] = "10";
    row1["orders_user_id"] = "1";
    row1["orders_description"] = "Order 1";
    row1["order_items_id"] = "100";
    row1["order_items_order_id"] = "10";
    row1["order_items_name"] = "Item 1";
    row1["order_items_quantity"] = "5";
    result.add(row1);

    // Вторая строка: тот же пользователь, тот же заказ, другой элемент
    database_adapter::query_result::row row2;
    row2["users_id"] = "1";
    row2["users_name"] = "User 1";
    row2["orders_id"] = "10";
    row2["orders_user_id"] = "1";
    row2["orders_description"] = "Order 1";
    row2["order_items_id"] = "101";
    row2["order_items_order_id"] = "10";
    row2["order_items_name"] = "Item 2";
    row2["order_items_quantity"] = "3";
    result.add(row2);

    // Третья строка: тот же пользователь, другой заказ
    database_adapter::query_result::row row3;
    row3["users_id"] = "1";
    row3["users_name"] = "User 1";
    row3["orders_id"] = "11";
    row3["orders_user_id"] = "1";
    row3["orders_description"] = "Order 2";
    row3["order_items_id"] = "102";
    row3["order_items_order_id"] = "11";
    row3["order_items_name"] = "Item 3";
    row3["order_items_quantity"] = "10";
    result.add(row3);

    // Четвертая строка: другой пользователь
    database_adapter::query_result::row row4;
    row4["users_id"] = "2";
    row4["users_name"] = "User 2";
    row4["orders_id"] = "20";
    row4["orders_user_id"] = "2";
    row4["orders_description"] = "Order 3";
    row4["order_items_id"] = "200";
    row4["order_items_order_id"] = "20";
    row4["order_items_name"] = "Item 4";
    row4["order_items_quantity"] = "1";
    result.add(row4);

    auto users = entity_craft::map_result_to_entities(_user_table, result);
    auto merged = entity_craft::merge_entities(users, _user_table);

    // После объединения должно быть 2 пользователя
    EXPECT_EQ(merged.size(), 2);

    // Первый пользователь должен иметь 2 заказа
    EXPECT_EQ(merged[0].id, 1);
    EXPECT_EQ(merged[0].name, "User 1");
    EXPECT_EQ(merged[0].orders.size(), 2);

    // Первый заказ первого пользователя должен иметь 2 элемента
    EXPECT_EQ(merged[0].orders[0].id, 10);
    EXPECT_EQ(merged[0].orders[0].items.size(), 2);

    // Второй заказ первого пользователя должен иметь 1 элемент
    EXPECT_EQ(merged[0].orders[1].id, 11);
    EXPECT_EQ(merged[0].orders[1].items.size(), 1);

    // Второй пользователь должен иметь 1 заказ с 1 элементом
    EXPECT_EQ(merged[1].id, 2);
    EXPECT_EQ(merged[1].name, "User 2");
    EXPECT_EQ(merged[1].orders.size(), 1);
    EXPECT_EQ(merged[1].orders[0].id, 20);
    EXPECT_EQ(merged[1].orders[0].items.size(), 1);
}

/**
 * @brief Тест merge_entities - второй уровень вложенности с множественными дубликатами
 */
TEST_F(MapperTestOneToMany, MergeEntities_TwoLevelsNested_MultipleDuplicates)
{
    // Создаем несколько пользователей с заказами и элементами
    one_to_many::TestUserWithOrders user1(1, "User 1", { });
    one_to_many::TestOrder order1(10, 1, "Order 1");
    order1.items.push_back(one_to_many::TestOrderItem(100, "Item 1", 5, 10));
    user1.orders.push_back(order1);

    one_to_many::TestUserWithOrders user2(1, "User 1", { }); // Тот же ID
    one_to_many::TestOrder order2(10, 1, "Order 1"); // Тот же заказ
    order2.items.push_back(one_to_many::TestOrderItem(101, "Item 2", 3, 10));
    user2.orders.push_back(order2);

    one_to_many::TestUserWithOrders user3(1, "User 1", { }); // Тот же ID
    one_to_many::TestOrder order3(11, 1, "Order 2"); // Другой заказ
    order3.items.push_back(one_to_many::TestOrderItem(102, "Item 3", 2, 11));
    user3.orders.push_back(order3);

    std::vector<one_to_many::TestUserWithOrders> entities = { user1, user2, user3 };

    auto merged = entity_craft::merge_entities(entities, _user_table);

    // Должен остаться один пользователь
    EXPECT_EQ(merged.size(), 1);
    EXPECT_EQ(merged[0].id, 1);
    EXPECT_EQ(merged[0].name, "User 1");
    // Должно быть 2 заказа (10 и 11)
    EXPECT_EQ(merged[0].orders.size(), 2);
    // Первый заказ должен иметь 2 элемента
    EXPECT_EQ(merged[0].orders[0].id, 10);
    EXPECT_EQ(merged[0].orders[0].items.size(), 2);
    // Второй заказ должен иметь 1 элемент
    EXPECT_EQ(merged[0].orders[1].id, 11);
    EXPECT_EQ(merged[0].orders[1].items.size(), 1);
}

/**
 * @brief Тест merge_entities - второй уровень вложенности с пустыми коллекциями
 */
TEST_F(MapperTestOneToMany, MergeEntities_TwoLevelsNested_EmptyCollections)
{
    one_to_many::TestUserWithOrders user1(1, "User 1", { });
    one_to_many::TestOrder order1(10, 1, "Order 1");
    // Заказ без элементов
    user1.orders.push_back(order1);

    one_to_many::TestUserWithOrders user2(1, "User 1", { }); // Тот же ID
    // Пользователь без заказов
    std::vector<one_to_many::TestUserWithOrders> entities = { user1, user2 };

    auto merged = entity_craft::merge_entities(entities, _user_table);

    EXPECT_EQ(merged.size(), 1);
    EXPECT_EQ(merged[0].id, 1);
    EXPECT_EQ(merged[0].name, "User 1");
    EXPECT_EQ(merged[0].orders.size(), 1);
    EXPECT_EQ(merged[0].orders[0].id, 10);
    EXPECT_TRUE(merged[0].orders[0].items.empty());
}