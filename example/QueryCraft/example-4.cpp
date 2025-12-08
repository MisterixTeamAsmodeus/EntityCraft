/**
 * @file example-4.cpp
 * @brief Пример использования QueryCraft: Агрегирующие функции и GROUP BY
 * 
 * Этот пример демонстрирует использование агрегирующих функций
 * (COUNT, SUM, AVG, MIN, MAX) и группировку данных.
 */

#include <QueryCraft/builder/select_builder.h>
#include <QueryCraft/builder/dsl.h>
#include <QueryCraft/dialect/postgres_dialect.h>

#include <iostream>
#include <memory>

using namespace query_craft;
using namespace query_craft::dsl;

/**
 * @brief Демонстрация COUNT
 */
void example_count()
{
    std::cout << "=== Пример 1: COUNT ===\n";
    
    select_builder builder;
    builder.from("users")
        .columns({ count({ col("id") }) });
    
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
 * @brief Демонстрация GROUP BY с COUNT
 */
void example_group_by_count()
{
    std::cout << "=== Пример 2: GROUP BY с COUNT ===\n";
    
    select_builder builder;
    builder.from("orders")
        .columns({ col("user_id"), count({ col("id") }) })
        .group_by({ col("user_id") });
    
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
 * @brief Демонстрация SUM
 */
void example_sum()
{
    std::cout << "=== Пример 3: SUM ===\n";
    
    select_builder builder;
    builder.from("orders")
        .columns({ col("user_id"), sum({ col("amount") }) })
        .group_by({ col("user_id") });
    
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
 * @brief Демонстрация AVG
 */
void example_avg()
{
    std::cout << "=== Пример 4: AVG ===\n";
    
    select_builder builder;
    builder.from("products")
        .columns({ col("category"), avg({ col("price") }) })
        .group_by({ col("category") });
    
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
 * @brief Демонстрация MIN и MAX
 */
void example_min_max()
{
    std::cout << "=== Пример 5: MIN и MAX ===\n";
    
    select_builder builder;
    builder.from("products")
        .columns({ col("category"), min({ col("price") }), max({ col("price") }) })
        .group_by({ col("category") });
    
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
 * @brief Демонстрация HAVING
 */
void example_having()
{
    std::cout << "=== Пример 6: HAVING ===\n";
    
    select_builder builder;
    builder.from("orders")
        .columns({ col("user_id"), sum({ col("amount") }) })
        .group_by({ col("user_id") })
        .having(sum({ col("amount") }) > param(1000));
    
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
 * @brief Демонстрация нескольких агрегирующих функций
 */
void example_multiple_aggregates()
{
    std::cout << "=== Пример 7: Несколько агрегирующих функций ===\n";
    
    select_builder builder;
    builder.from("orders")
        .columns({ 
            col("user_id"), 
            count({ col("id") }),
            sum({ col("amount") }),
            avg({ col("amount") })
        })
        .group_by({ col("user_id") })
        .having(count({ col("id") }) > param(5));
    
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
 * @brief Демонстрация GROUP BY с ORDER BY
 */
void example_group_by_with_order_by()
{
    std::cout << "=== Пример 8: GROUP BY с ORDER BY ===\n";
    
    select_builder builder;
    builder.from("orders")
        .columns({ col("user_id"), sum({ col("amount") }) })
        .group_by({ col("user_id") })
        .order_by({ { sum({ col("amount") }), false } })
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
 * @brief Демонстрация пользовательской агрегирующей функции
 */
void example_custom_aggregate()
{
    std::cout << "=== Пример 9: Пользовательская агрегирующая функция ===\n";
    
    select_builder builder;
    builder.from("data")
        .columns({ col("category"), custom_aggregate("json_array_agg_logical", { col("value") }) })
        .group_by({ col("category") });
    
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
    std::cout << "Примеры использования QueryCraft: Агрегирующие функции и GROUP BY\n";
    std::cout << "==================================================================\n\n";
    
    example_count();
    example_group_by_count();
    example_sum();
    example_avg();
    example_min_max();
    example_having();
    example_multiple_aggregates();
    example_group_by_with_order_by();
    example_custom_aggregate();
    
    return 0;
}

