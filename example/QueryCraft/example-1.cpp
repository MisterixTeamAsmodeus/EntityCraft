/**
 * @file example-1.cpp
 * @brief Пример использования QueryCraft: Базовые SELECT запросы
 * 
 * Этот пример демонстрирует создание простых SELECT запросов
 * с использованием билдера и DSL хелперов.
 */

#include <QueryCraft/builder/select_builder.h>
#include <QueryCraft/builder/dsl.h>
#include <QueryCraft/dialect/postgres_dialect.h>

#include <iostream>
#include <memory>

/**
 * @brief Демонстрация простого SELECT запроса
 */
void example_simple_select()
{
    std::cout << "=== Пример 1: Простой SELECT запрос ===\n";

    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users")
        .columns({ col("id"), col("name"), col("email") });
    
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
 * @brief Демонстрация SELECT с WHERE условием
 */
void example_select_with_where()
{
    std::cout << "=== Пример 2: SELECT с WHERE условием ===\n";

    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users")
        .columns({ col("id"), col("name") })
        .where(col("age") > param(18));
    
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
 * @brief Демонстрация SELECT с несколькими условиями
 */
void example_select_with_multiple_conditions()
{
    std::cout << "=== Пример 3: SELECT с несколькими условиями ===\n";

    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users")
        .columns({ col("id"), col("name") })
        .where((col("age") > param(18)) && (col("active") == param(true)));
    
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
 * @brief Демонстрация SELECT с ORDER BY
 */
void example_select_with_order_by()
{
    std::cout << "=== Пример 4: SELECT с ORDER BY ===\n";

    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users")
        .columns({ col("id"), col("name"), col("created_at") })
        .order_by({ { col("created_at"), false }, { col("name"), true } })
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

/**
 * @brief Демонстрация SELECT с LIMIT и OFFSET
 */
void example_select_with_limit_offset()
{
    std::cout << "=== Пример 5: SELECT с LIMIT и OFFSET ===\n";

    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users")
        .columns({ col("id"), col("name") })
        .limit(20)
        .offset(40);
    
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
 * @brief Демонстрация SELECT с использованием схемы
 */
void example_select_with_schema()
{
    std::cout << "=== Пример 6: SELECT с использованием схемы ===\n";

    using namespace query_craft::dsl;
    select_builder builder;
    builder.from("users", "public")
        .columns({ col("id"), col("name") });
    
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
    std::cout << "Примеры использования QueryCraft: Базовые SELECT запросы\n";
    std::cout << "========================================================\n\n";
    
    example_simple_select();
    example_select_with_where();
    example_select_with_multiple_conditions();
    example_select_with_order_by();
    example_select_with_limit_offset();
    example_select_with_schema();
    
    return 0;
}

