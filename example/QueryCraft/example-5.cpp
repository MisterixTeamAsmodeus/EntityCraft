/**
 * @file example-5.cpp
 * @brief Пример использования QueryCraft: CTE и подзапросы
 * 
 * Этот пример демонстрирует использование Common Table Expressions (CTE)
 * и подзапросов в SELECT запросах.
 */

#include <QueryCraft/builder/select_builder.h>
#include <QueryCraft/builder/dsl.h>
#include <QueryCraft/dialect/postgres_dialect.h>

#include <iostream>
#include <memory>

using namespace query_craft;
using namespace query_craft::dsl;

/**
 * @brief Демонстрация простого CTE
 */
void example_simple_cte()
{
    std::cout << "=== Пример 1: Простой CTE ===\n";
    
    select_builder cte_query;
    cte_query.from("users")
        .columns({ col("id") })
        .where(col("active") == param(true));
    
    select_builder builder;
    builder.with("active_users", cte_query.to_ast())
        .from("active_users")
        .columns({ col("id") });
    
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
 * @brief Демонстрация CTE с JOIN
 */
void example_cte_with_join()
{
    std::cout << "=== Пример 2: CTE с JOIN ===\n";
    
    select_builder cte_query;
    cte_query.from("orders")
        .columns({ col("user_id"), sum({ col("amount") }) })
        .group_by({ col("user_id") })
        .having(sum({ col("amount") }) > param(1000));
    
    select_builder builder;
    builder.with("big_spenders", cte_query.to_ast())
        .from("users")
        .columns({ col("name", "users"), col("total", "big_spenders") })
        .inner_join("big_spenders", col("id", "users") == col("user_id", "big_spenders"));
    
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
 * @brief Демонстрация нескольких CTE
 */
void example_multiple_cte()
{
    std::cout << "=== Пример 3: Несколько CTE ===\n";
    
    select_builder active_users_cte;
    active_users_cte.from("users")
        .columns({ col("id"), col("name") })
        .where(col("active") == param(true));
    
    select_builder recent_orders_cte;
    recent_orders_cte.from("orders")
        .columns({ col("user_id"), col("amount") })
        .where(col("created_at") > param(std::string("2024-01-01")));
    
    select_builder builder;
    builder.with("active_users", active_users_cte.to_ast())
        .with("recent_orders", recent_orders_cte.to_ast())
        .from("active_users")
        .columns({ col("name", "active_users"), col("amount", "recent_orders") })
        .inner_join("recent_orders", col("id", "active_users") == col("user_id", "recent_orders"));
    
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
 * @brief Демонстрация подзапроса в WHERE
 */
void example_subquery_in_where()
{
    std::cout << "=== Пример 4: Подзапрос в WHERE ===\n";
    
    select_builder subquery_builder;
    subquery_builder.from("orders")
        .columns({ col("user_id") })
        .where(col("amount") > param(1000));
    
    select_builder builder;
    builder.from("users")
        .columns({ col("id"), col("name") })
        .where(col("id") == subquery(subquery_builder.to_ast()));
    
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
 * @brief Демонстрация подзапроса в SELECT
 */
void example_subquery_in_select()
{
    std::cout << "=== Пример 5: Подзапрос в SELECT ===\n";
    
    select_builder subquery_builder;
    subquery_builder.from("orders")
        .columns({ count({ col("id") }) })
        .where(col("user_id") == col("id", "users"));
    
    select_builder builder;
    builder.from("users")
        .columns({ col("id"), col("name"), subquery(subquery_builder.to_ast()) });
    
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
 * @brief Демонстрация подзапроса с IN
 */
void example_subquery_with_in()
{
    std::cout << "=== Пример 6: Подзапрос с IN ===\n";
    
    select_builder subquery_builder;
    subquery_builder.from("orders")
        .columns({ col("user_id") })
        .where(col("status") == param(std::string("completed")));
    
    select_builder builder;
    builder.from("users")
        .columns({ col("id"), col("name") })
        .where(in_(col("id"), subquery(subquery_builder.to_ast())));
    
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
 * @brief Демонстрация рекурсивного CTE (концептуальный пример)
 */
void example_recursive_cte_concept()
{
    std::cout << "=== Пример 7: Концепция рекурсивного CTE ===\n";
    std::cout << "Примечание: Рекурсивные CTE требуют специальной поддержки,\n";
    std::cout << "которая может быть добавлена в будущих версиях библиотеки.\n\n";
}

/**
 * @brief Демонстрация CTE с агрегацией
 */
void example_cte_with_aggregation()
{
    std::cout << "=== Пример 8: CTE с агрегацией ===\n";
    
    select_builder cte_query;
    cte_query.from("orders")
        .columns({ col("user_id"), sum({ col("amount") }), count({ col("id") }) })
        .group_by({ col("user_id") });
    
    select_builder builder;
    builder.with("user_totals", cte_query.to_ast())
        .from("user_totals")
        .columns({ col("user_id"), col("total_amount"), col("order_count") })
        .where(col("total_amount") > param(5000))
        .order_by({ { col("total_amount"), false } });
    
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
    std::cout << "Примеры использования QueryCraft: CTE и подзапросы\n";
    std::cout << "==================================================\n\n";
    
    example_simple_cte();
    example_cte_with_join();
    example_multiple_cte();
    example_subquery_in_where();
    example_subquery_in_select();
    example_subquery_with_in();
    example_recursive_cte_concept();
    example_cte_with_aggregation();
    
    return 0;
}

