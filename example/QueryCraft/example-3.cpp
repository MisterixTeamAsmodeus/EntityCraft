/**
 * @file example-3.cpp
 * @brief Пример использования QueryCraft: JOIN запросы
 * 
 * Этот пример демонстрирует создание SELECT запросов с различными типами JOIN.
 */

#include <QueryCraft/builder/select_builder.h>
#include <QueryCraft/builder/dsl.h>
#include <QueryCraft/dialect/postgres_dialect.h>

#include <iostream>
#include <memory>

using namespace query_craft;
using namespace query_craft::dsl;

/**
 * @brief Демонстрация INNER JOIN
 */
void example_inner_join()
{
    std::cout << "=== Пример 1: INNER JOIN ===\n";
    
    select_builder builder;
    builder.from("users")
        .columns({ col("id", "users"), col("name", "users"), col("order_id", "orders") })
        .inner_join("orders", col("id", "users") == col("user_id", "orders"));
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto query = builder.compile(postgres_dialect);
    
    std::cout << "SQL: " << query.sql << "\n";
    std::cout << "Параметры: ";
    for (const auto& param : query.parameters) {
        std::cout << param << " ";
    }
    std::cout << "\n\n";
}

/**
 * @brief Демонстрация LEFT JOIN
 */
void example_left_join()
{
    std::cout << "=== Пример 2: LEFT JOIN ===\n";
    
    select_builder builder;
    builder.from("users")
        .columns({ col("id", "users"), col("name", "users"), col("profile_id", "profiles") })
        .left_join("profiles", col("id", "users") == col("user_id", "profiles"), "public");
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto query = builder.compile(postgres_dialect);
    
    std::cout << "SQL: " << query.sql << "\n";
    std::cout << "Параметры: ";
    for (const auto& param : query.parameters) {
        std::cout << param << " ";
    }
    std::cout << "\n\n";
}

/**
 * @brief Демонстрация RIGHT JOIN
 */
void example_right_join()
{
    std::cout << "=== Пример 3: RIGHT JOIN ===\n";
    
    select_builder builder;
    builder.from("users")
        .columns({ col("id", "users"), col("order_id", "orders"), col("amount", "orders") })
        .right_join("orders", col("id", "users") == col("user_id", "orders"));
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto query = builder.compile(postgres_dialect);
    
    std::cout << "SQL: " << query.sql << "\n";
    std::cout << "Параметры: ";
    for (const auto& param : query.parameters) {
        std::cout << param << " ";
    }
    std::cout << "\n\n";
}

/**
 * @brief Демонстрация FULL JOIN
 */
void example_full_join()
{
    std::cout << "=== Пример 4: FULL JOIN ===\n";
    
    select_builder builder;
    builder.from("users")
        .columns({ col("id", "users"), col("name", "users"), col("order_id", "orders") })
        .full_join("orders", col("id", "users") == col("user_id", "orders"));
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto query = builder.compile(postgres_dialect);
    
    std::cout << "SQL: " << query.sql << "\n";
    std::cout << "Параметры: ";
    for (const auto& param : query.parameters) {
        std::cout << param << " ";
    }
    std::cout << "\n\n";
}

/**
 * @brief Демонстрация нескольких JOIN в одном запросе
 */
void example_multiple_joins()
{
    std::cout << "=== Пример 5: Несколько JOIN в одном запросе ===\n";
    
    select_builder builder;
    builder.from("users")
        .columns({ 
            col("name", "users"), 
            col("order_id", "orders"), 
            col("product_name", "products") 
        })
        .inner_join("orders", col("id", "users") == col("user_id", "orders"))
        .left_join("products", col("product_id", "orders") == col("id", "products"));
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto query = builder.compile(postgres_dialect);
    
    std::cout << "SQL: " << query.sql << "\n";
    std::cout << "Параметры: ";
    for (const auto& param : query.parameters) {
        std::cout << param << " ";
    }
    std::cout << "\n\n";
}

/**
 * @brief Демонстрация JOIN с условиями WHERE
 */
void example_join_with_where()
{
    std::cout << "=== Пример 6: JOIN с условиями WHERE ===\n";
    
    select_builder builder;
    builder.from("users")
        .columns({ col("name", "users"), col("amount", "orders") })
        .inner_join("orders", col("id", "users") == col("user_id", "orders"))
        .where(col("amount", "orders") > param(1000));
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto query = builder.compile(postgres_dialect);
    
    std::cout << "SQL: " << query.sql << "\n";
    std::cout << "Параметры: ";
    for (const auto& param : query.parameters) {
        std::cout << param << " ";
    }
    std::cout << "\n\n";
}

/**
 * @brief Демонстрация JOIN с ORDER BY
 */
void example_join_with_order_by()
{
    std::cout << "=== Пример 7: JOIN с ORDER BY ===\n";
    
    select_builder builder;
    builder.from("users")
        .columns({ col("name", "users"), col("order_date", "orders") })
        .left_join("orders", col("id", "users") == col("user_id", "orders"))
        .order_by({ { col("order_date", "orders"), false } })
        .limit(10);
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto query = builder.compile(postgres_dialect);
    
    std::cout << "SQL: " << query.sql << "\n";
    std::cout << "Параметры: ";
    for (const auto& param : query.parameters) {
        std::cout << param << " ";
    }
    std::cout << "\n\n";
}

int main()
{
    std::cout << "Примеры использования QueryCraft: JOIN запросы\n";
    std::cout << "===============================================\n\n";
    
    example_inner_join();
    example_left_join();
    example_right_join();
    example_full_join();
    example_multiple_joins();
    example_join_with_where();
    example_join_with_order_by();
    
    return 0;
}

