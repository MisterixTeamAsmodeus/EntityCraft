#pragma once

#include "compiled_query.h"

#include <QueryCraft/ast/select_query.h>
#include <QueryCraft/dialect/sql_dialect.h>

#include <cstdint>
#include <initializer_list>
#include <memory>
#include <vector>

namespace query_craft {
namespace dsl {

/**
 * @brief Флюент‑builder для SELECT‑запроса.
 */
class select_builder
{
public:
    /**
     * @brief Устанавливает таблицу для выборки.
     * @param table_name Имя таблицы.
     * @param schema Схема таблицы (опционально).
     * @return Ссылка на билдер для цепочки вызовов.
     */
    select_builder& from(const std::string& table_name, const std::string& schema = {});

    /**
     * @brief Устанавливает столбцы для выборки.
     * @param columns Столбцы.
     * @return Ссылка на билдер для цепочки вызовов.
     */
    select_builder& columns(std::initializer_list<ast::expression> columns);

    /**
     * @brief Устанавливает столбцы для выборки из контейнера.
     * @param columns Столбцы из контейнера.
     * @return Ссылка на билдер для цепочки вызовов.
     */
    select_builder& columns(const std::vector<ast::expression>& columns);

    /**
     * @brief Устанавливает условие WHERE.
     * @param expr Условие.
     * @return Ссылка на билдер для цепочки вызовов.
     */
    select_builder& where(ast::expression expr);
    /**
     * @brief Устанавливает группировку.
     * @param expressions Группировки.
     * @return Ссылка на билдер для цепочки вызовов.
     */
    select_builder& group_by(std::initializer_list<ast::expression> expressions);

    /**
     * @brief Устанавливает условие HAVING.
     * @param expr Условие.
     * @return Ссылка на билдер для цепочки вызовов.
     */
    select_builder& having(ast::expression expr);

    /**
     * @brief Устанавливает сортировку.
     * @param items Сортировки.
     * @return Ссылка на билдер для цепочки вызовов.
     */
    select_builder& order_by(std::initializer_list<ast::order_by_item> items);

    /**
     * @brief Устанавливает лимит.
     * @param value Лимит.
     * @return Ссылка на билдер для цепочки вызовов.
     */
    select_builder& limit(std::uint64_t value);

    /**
     * @brief Устанавливает смещение.
     * @param value Смещение.
     * @return Ссылка на билдер для цепочки вызовов.
     */
    select_builder& offset(std::uint64_t value);

    /**
     * @brief Включает блокировку выбранных строк (SELECT ... FOR UPDATE).
     *
     * Используется для конкурентного доступа к данным в транзакциях.
     * Конкретный синтаксис и поддержка могут зависеть от диалекта.
     *
     * @return Ссылка на билдер для цепочки вызовов.
     */
    select_builder& for_update();

    /**
     * @brief Добавляет JOIN к запросу.
     *
     * @param type Тип JOIN (INNER, LEFT, RIGHT, FULL).
     * @param table_name Имя таблицы для соединения.
     * @param schema Схема таблицы (опционально).
     * @param on Условие соединения (ON clause).
     * @return Ссылка на билдер для цепочки вызовов.
     */
    select_builder& join(ast::join_type type, const std::string& table_name, ast::expression on, const std::string& schema = {});

    /**
     * @brief Добавляет INNER JOIN к запросу.
     *
     * @param table_name Имя таблицы для соединения.
     * @param on Условие соединения (ON clause).
     * @param schema Схема таблицы (опционально).
     * @return Ссылка на билдер для цепочки вызовов.
     */
    select_builder& inner_join(const std::string& table_name, ast::expression on, const std::string& schema = {});

    /**
     * @brief Добавляет LEFT JOIN к запросу.
     *
     * @param table_name Имя таблицы для соединения.
     * @param on Условие соединения (ON clause).
     * @param schema Схема таблицы (опционально).
     * @return Ссылка на билдер для цепочки вызовов.
     */
    select_builder& left_join(const std::string& table_name, ast::expression on, const std::string& schema = {});

    /**
     * @brief Добавляет RIGHT JOIN к запросу.
     *
     * @param table_name Имя таблицы для соединения.
     * @param on Условие соединения (ON clause).
     * @param schema Схема таблицы (опционально).
     * @return Ссылка на билдер для цепочки вызовов.
     */
    select_builder& right_join(const std::string& table_name, ast::expression on, const std::string& schema = {});

    /**
     * @brief Добавляет FULL JOIN к запросу.
     *
     * @param table_name Имя таблицы для соединения.
     * @param on Условие соединения (ON clause).
     * @param schema Схема таблицы (опционально).
     * @return Ссылка на билдер для цепочки вызовов.
     */
    select_builder& full_join(const std::string& table_name, ast::expression on, const std::string& schema = {});

    /**
     * @brief Добавляет CTE (Common Table Expression) к запросу.
     *
     * CTE позволяет определить временное именованное результирующее множество,
     * которое существует только в рамках выполнения одного запроса.
     *
     * @param name Имя CTE.
     * @param query SELECT-запрос, определяющий CTE.
     * @return Ссылка на билдер для цепочки вызовов.
     * @note Пример:
     * @code
     * select_builder builder;
     * select_builder cte_query;
     * cte_query.from("users").columns({col("id")});
     * builder.with("user_ids", cte_query.to_ast())
     *       .from("user_ids")
     *       .columns({col("id")});
     * @endcode
     */
    select_builder& with(const std::string& name, const ast::select_query& query);

    /**
     * @brief Возвращает константную ссылку на внутренний AST.
     * @return AST‑запрос.
     */
    ast::select_query to_ast() const noexcept;

    /**
     * @brief Компилирует запрос с помощью переданного диалекта.
     * @param dialect Диалект.
     * @return Скомпилированный запрос.
     */
    compiled_query compile(std::shared_ptr<sql_dialect> dialect) const;

private:
    /// AST‑запрос.
    ast::select_query query_;
};

} // namespace dsl
} // namespace query_craft
