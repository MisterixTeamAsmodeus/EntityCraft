/**
 * @file example-2.cpp
 * @brief Пример использования QueryCraft: INSERT, UPDATE, DELETE запросы
 * 
 * Этот пример демонстрирует создание INSERT, UPDATE и DELETE запросов
 * с использованием билдеров QueryCraft.
 */

#include <QueryCraft/builder/insert_builder.h>
#include <QueryCraft/builder/update_builder.h>
#include <QueryCraft/builder/delete_builder.h>
#include <QueryCraft/builder/dsl.h>
#include <QueryCraft/dialect/postgres_dialect.h>

#include <iostream>
#include <memory>

using namespace query_craft;
using namespace query_craft::dsl;

/**
 * @brief Демонстрация простого INSERT запроса
 */
void example_simple_insert()
{
    std::cout << "=== Пример 1: Простой INSERT запрос ===\n";
    
    insert_builder builder;
    builder.into("users")
        .columns({ "name", "email", "age" })
        .values({ param(std::string("Иван Иванов")), 
                  param(std::string("ivan@example.com")), 
                  param(30) });
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto query = builder.compile(postgres_dialect);
    
    std::cout << "SQL: " << query.sql << "\n";
    std::cout << "Параметры: ";
    for (const auto& param : query.parameters) {
        std::cout << "\"" << param << "\" ";
    }
    std::cout << "\n\n";
}

/**
 * @brief Демонстрация INSERT с несколькими строками
 */
void example_insert_multiple_rows()
{
    std::cout << "=== Пример 2: INSERT с несколькими строками ===\n";
    
    insert_builder builder;
    builder.into("users")
        .columns({ "name", "age" })
        .values({ param(std::string("Алексей")), param(25) })
        .values({ param(std::string("Мария")), param(28) })
        .values({ param(std::string("Петр")), param(32) });
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto query = builder.compile(postgres_dialect);
    
    std::cout << "SQL: " << query.sql << "\n";
    std::cout << "Параметры: ";
    for (const auto& param : query.parameters) {
        std::cout << "\"" << param << "\" ";
    }
    std::cout << "\n\n";
}

/**
 * @brief Демонстрация INSERT с RETURNING
 */
void example_insert_with_returning()
{
    std::cout << "=== Пример 3: INSERT с RETURNING ===\n";
    
    insert_builder builder;
    builder.into("users")
        .columns({ "name", "email" })
        .values({ param(std::string("Новый пользователь")), 
                  param(std::string("new@example.com")) })
        .returning({ "id", "name", "created_at" });
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto query = builder.compile(postgres_dialect);
    
    std::cout << "SQL: " << query.sql << "\n";
    std::cout << "Параметры: ";
    for (const auto& param : query.parameters) {
        std::cout << "\"" << param << "\" ";
    }
    std::cout << "\n\n";
}

/**
 * @brief Демонстрация простого UPDATE запроса
 */
void example_simple_update()
{
    std::cout << "=== Пример 4: Простой UPDATE запрос ===\n";
    
    update_builder builder;
    builder.table("users")
        .set("email", param(std::string("newemail@example.com")))
        .set("age", param(31))
        .where(col("id") == param(1));
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto query = builder.compile(postgres_dialect);
    
    std::cout << "SQL: " << query.sql << "\n";
    std::cout << "Параметры: ";
    for (const auto& param : query.parameters) {
        std::cout << "\"" << param << "\" ";
    }
    std::cout << "\n\n";
}

/**
 * @brief Демонстрация UPDATE с RETURNING
 */
void example_update_with_returning()
{
    std::cout << "=== Пример 5: UPDATE с RETURNING ===\n";
    
    update_builder builder;
    builder.table("users")
        .set("active", param(false))
        .where(col("last_login") < param(std::string("2024-01-01")))
        .returning({ "id", "name", "email" });
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto query = builder.compile(postgres_dialect);
    
    std::cout << "SQL: " << query.sql << "\n";
    std::cout << "Параметры: ";
    for (const auto& param : query.parameters) {
        std::cout << "\"" << param << "\" ";
    }
    std::cout << "\n\n";
}

/**
 * @brief Демонстрация UPDATE с сложным условием
 */
void example_update_with_complex_condition()
{
    std::cout << "=== Пример 6: UPDATE с сложным условием ===\n";
    
    update_builder builder;
    builder.table("users")
        .set("status", param(std::string("inactive")))
        .where((col("age") < param(18)) || (col("age") > param(65)));
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto query = builder.compile(postgres_dialect);
    
    std::cout << "SQL: " << query.sql << "\n";
    std::cout << "Параметры: ";
    for (const auto& param : query.parameters) {
        std::cout << "\"" << param << "\" ";
    }
    std::cout << "\n\n";
}

/**
 * @brief Демонстрация простого DELETE запроса
 */
void example_simple_delete()
{
    std::cout << "=== Пример 7: Простой DELETE запрос ===\n";
    
    delete_builder builder;
    builder.from("users")
        .where(col("id") == param(1));
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto query = builder.compile(postgres_dialect);
    
    std::cout << "SQL: " << query.sql << "\n";
    std::cout << "Параметры: ";
    for (const auto& param : query.parameters) {
        std::cout << "\"" << param << "\" ";
    }
    std::cout << "\n\n";
}

/**
 * @brief Демонстрация DELETE с RETURNING
 */
void example_delete_with_returning()
{
    std::cout << "=== Пример 8: DELETE с RETURNING ===\n";
    
    delete_builder builder;
    builder.from("users")
        .where(col("active") == param(false))
        .returning({ "id", "name", "email" });
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto query = builder.compile(postgres_dialect);
    
    std::cout << "SQL: " << query.sql << "\n";
    std::cout << "Параметры: ";
    for (const auto& param : query.parameters) {
        std::cout << "\"" << param << "\" ";
    }
    std::cout << "\n\n";
}

/**
 * @brief Демонстрация DELETE с сложным условием
 */
void example_delete_with_complex_condition()
{
    std::cout << "=== Пример 9: DELETE с сложным условием ===\n";
    
    delete_builder builder;
    builder.from("users")
        .where((col("created_at") < param(std::string("2020-01-01"))) && 
               (col("last_login") == param(std::string("NULL"))));
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto query = builder.compile(postgres_dialect);
    
    std::cout << "SQL: " << query.sql << "\n";
    std::cout << "Параметры: ";
    for (const auto& param : query.parameters) {
        std::cout << "\"" << param << "\" ";
    }
    std::cout << "\n\n";
}

int main()
{
    std::cout << "Примеры использования QueryCraft: INSERT, UPDATE, DELETE запросы\n";
    std::cout << "================================================================\n\n";
    
    example_simple_insert();
    example_insert_multiple_rows();
    example_insert_with_returning();
    example_simple_update();
    example_update_with_returning();
    example_update_with_complex_condition();
    example_simple_delete();
    example_delete_with_returning();
    example_delete_with_complex_condition();
    
    return 0;
}

