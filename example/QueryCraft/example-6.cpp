/**
 * @file example-6.cpp
 * @brief Пример использования QueryCraft: Различия между диалектами
 * 
 * Этот пример демонстрирует различия в генерации SQL между
 * PostgreSQL и SQLite диалектами.
 */

#include <QueryCraft/builder/select_builder.h>
#include <QueryCraft/builder/insert_builder.h>
#include <QueryCraft/builder/update_builder.h>
#include <QueryCraft/builder/delete_builder.h>
#include <QueryCraft/builder/dsl.h>
#include <QueryCraft/dialect/postgres_dialect.h>
#include <QueryCraft/dialect/sqlite_dialect.h>

#include <iostream>
#include <memory>

using namespace query_craft;
using namespace query_craft::dsl;

/**
 * @brief Сравнение плейсхолдеров в SELECT запросах
 */
void example_placeholders_comparison()
{
    std::cout << "=== Пример 1: Различия в плейсхолдерах ===\n\n";
    
    select_builder builder;
    builder.from("users")
        .columns({ col("id"), col("name") })
        .where((col("age") > param(18)) && (col("active") == param(true)));
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto sqlite_dialect = std::make_shared<query_craft::sqlite_dialect>();
    
    auto pg_query = builder.compile(postgres_dialect);
    auto sqlite_query = builder.compile(sqlite_dialect);
    
    std::cout << "PostgreSQL:\n";
    std::cout << "  SQL: " << pg_query.sql << "\n";
    std::cout << "  Плейсхолдеры: $1, $2, ...\n";
    std::cout << "  Параметры: ";
    for (const auto& param : pg_query.parameters) {
        std::cout << param << " ";
    }
    std::cout << "\n\n";
    
    std::cout << "SQLite:\n";
    std::cout << "  SQL: " << sqlite_query.sql << "\n";
    std::cout << "  Плейсхолдеры: ?\n";
    std::cout << "  Параметры: ";
    for (const auto& param : sqlite_query.parameters) {
        std::cout << param << " ";
    }
    std::cout << "\n\n";
}

/**
 * @brief Сравнение INSERT запросов
 */
void example_insert_comparison()
{
    std::cout << "=== Пример 2: INSERT запросы ===\n\n";
    
    insert_builder builder;
    builder.into("users")
        .columns({ "name", "email", "age" })
        .values({ param(std::string("Иван")), 
                  param(std::string("ivan@example.com")), 
                  param(30) });
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto sqlite_dialect = std::make_shared<query_craft::sqlite_dialect>();
    
    auto pg_query = builder.compile(postgres_dialect);
    auto sqlite_query = builder.compile(sqlite_dialect);
    
    std::cout << "PostgreSQL:\n";
    std::cout << "  SQL: " << pg_query.sql << "\n\n";
    
    std::cout << "SQLite:\n";
    std::cout << "  SQL: " << sqlite_query.sql << "\n\n";
}

/**
 * @brief Сравнение UPDATE запросов
 */
void example_update_comparison()
{
    std::cout << "=== Пример 3: UPDATE запросы ===\n\n";
    
    update_builder builder;
    builder.table("users")
        .set("email", param(std::string("newemail@example.com")))
        .set("age", param(31))
        .where(col("id") == param(1));
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto sqlite_dialect = std::make_shared<query_craft::sqlite_dialect>();
    
    auto pg_query = builder.compile(postgres_dialect);
    auto sqlite_query = builder.compile(sqlite_dialect);
    
    std::cout << "PostgreSQL:\n";
    std::cout << "  SQL: " << pg_query.sql << "\n\n";
    
    std::cout << "SQLite:\n";
    std::cout << "  SQL: " << sqlite_query.sql << "\n\n";
}

/**
 * @brief Сравнение DELETE запросов
 */
void example_delete_comparison()
{
    std::cout << "=== Пример 4: DELETE запросы ===\n\n";
    
    delete_builder builder;
    builder.from("users")
        .where(col("id") == param(1));
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto sqlite_dialect = std::make_shared<query_craft::sqlite_dialect>();
    
    auto pg_query = builder.compile(postgres_dialect);
    auto sqlite_query = builder.compile(sqlite_dialect);
    
    std::cout << "PostgreSQL:\n";
    std::cout << "  SQL: " << pg_query.sql << "\n\n";
    
    std::cout << "SQLite:\n";
    std::cout << "  SQL: " << sqlite_query.sql << "\n\n";
}

/**
 * @brief Сравнение RETURNING (поддерживается в обоих диалектах)
 */
void example_returning_comparison()
{
    std::cout << "=== Пример 5: RETURNING (поддерживается в обоих диалектах) ===\n\n";
    
    insert_builder builder;
    builder.into("users")
        .columns({ "name", "email" })
        .values({ param(std::string("Новый пользователь")), 
                  param(std::string("new@example.com")) })
        .returning({ "id", "name", "created_at" });
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto sqlite_dialect = std::make_shared<query_craft::sqlite_dialect>();
    
    auto pg_query = builder.compile(postgres_dialect);
    auto sqlite_query = builder.compile(sqlite_dialect);
    
    std::cout << "PostgreSQL:\n";
    std::cout << "  SQL: " << pg_query.sql << "\n";
    std::cout << "  Поддержка RETURNING: Да\n\n";
    
    std::cout << "SQLite:\n";
    std::cout << "  SQL: " << sqlite_query.sql << "\n";
    std::cout << "  Поддержка RETURNING: Да (начиная с версии 3.35.0)\n\n";
}

/**
 * @brief Сравнение LIMIT и OFFSET
 */
void example_limit_offset_comparison()
{
    std::cout << "=== Пример 6: LIMIT и OFFSET ===\n\n";
    
    select_builder builder;
    builder.from("users")
        .columns({ col("id"), col("name") })
        .limit(10)
        .offset(20);
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto sqlite_dialect = std::make_shared<query_craft::sqlite_dialect>();
    
    auto pg_query = builder.compile(postgres_dialect);
    auto sqlite_query = builder.compile(sqlite_dialect);
    
    std::cout << "PostgreSQL:\n";
    std::cout << "  SQL: " << pg_query.sql << "\n";
    std::cout << "  Формат: LIMIT n OFFSET m\n\n";
    
    std::cout << "SQLite:\n";
    std::cout << "  SQL: " << sqlite_query.sql << "\n";
    std::cout << "  Формат: LIMIT n OFFSET m\n\n";
}

/**
 * @brief Сравнение кавычек идентификаторов
 */
void example_identifier_quoting_comparison()
{
    std::cout << "=== Пример 7: Кавычки идентификаторов ===\n\n";
    
    select_builder builder;
    builder.from("users", "public")
        .columns({ col("id"), col("user name") });
    
    auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
    auto sqlite_dialect = std::make_shared<query_craft::sqlite_dialect>();
    
    auto pg_query = builder.compile(postgres_dialect);
    auto sqlite_query = builder.compile(sqlite_dialect);
    
    std::cout << "PostgreSQL:\n";
    std::cout << "  SQL: " << pg_query.sql << "\n";
    std::cout << "  Кавычки: двойные кавычки (\") для идентификаторов\n\n";
    
    std::cout << "SQLite:\n";
    std::cout << "  SQL: " << sqlite_query.sql << "\n";
    std::cout << "  Кавычки: двойные кавычки (\") для идентификаторов\n\n";
}

/**
 * @brief Итоговое сравнение особенностей диалектов
 */
void example_dialect_features_summary()
{
    std::cout << "=== Сводка особенностей диалектов ===\n\n";
    
    std::cout << "PostgreSQL:\n";
    std::cout << "  - Плейсхолдеры: $1, $2, $3, ...\n";
    std::cout << "  - Поддержка RETURNING: Да\n";
    std::cout << "  - Поддержка схем: Да\n";
    std::cout << "  - Кавычки идентификаторов: двойные кавычки\n\n";
    
    std::cout << "SQLite:\n";
    std::cout << "  - Плейсхолдеры: ?\n";
    std::cout << "  - Поддержка RETURNING: Да (с версии 3.35.0)\n";
    std::cout << "  - Поддержка схем: Ограниченная\n";
    std::cout << "  - Кавычки идентификаторов: двойные кавычки\n\n";
}

int main()
{
    std::cout << "Примеры использования QueryCraft: Различия между диалектами\n";
    std::cout << "============================================================\n\n";
    
    example_placeholders_comparison();
    example_insert_comparison();
    example_update_comparison();
    example_delete_comparison();
    example_returning_comparison();
    example_limit_offset_comparison();
    example_identifier_quoting_comparison();
    example_dialect_features_summary();
    
    return 0;
}

