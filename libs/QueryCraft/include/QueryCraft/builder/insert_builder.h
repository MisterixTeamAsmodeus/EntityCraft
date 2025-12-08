#pragma once

#include "compiled_query.h"

#include <QueryCraft/ast/insert_query.h>
#include <QueryCraft/dialect/sql_dialect.h>

#include <initializer_list>
#include <memory>
#include <vector>

namespace query_craft {
namespace dsl {

/**
 * @brief Флюент‑builder для INSERT‑запроса.
 */
class insert_builder
{
public:
    /**
     * @brief Устанавливает таблицу для вставки.
     * @param table_name Имя таблицы.
     * @param schema Схема таблицы (опционально).
     * @return Ссылка на билдер для цепочки вызовов.
     */
    insert_builder& into(const std::string& table_name, const std::string& schema = {});

    /**
     * @brief Устанавливает столбцы для вставки.
     * @param column_names Имена столбцов.
     * @return Ссылка на билдер для цепочки вызовов.
     */
    insert_builder& columns(std::initializer_list<std::string> column_names);

    /**
     * @brief Устанавливает столбцы для вставки из контейнера.
     * @param column_names Имена столбцов из контейнера.
     * @return Ссылка на билдер для цепочки вызовов.
     */
    insert_builder& columns(const std::vector<std::string>& column_names);

    /**
     * @brief Устанавливает значения для вставки.
     * @param row Значения.
     * @return Ссылка на билдер для цепочки вызовов.
     */
    insert_builder& values(std::initializer_list<ast::expression> row);

    /**
     * @brief Устанавливает значения для вставки из контейнера.
     * @param row Значения из контейнера.
     * @return Ссылка на билдер для цепочки вызовов.
     */
    insert_builder& values(const std::vector<ast::expression>& row);

    /**
     * @brief Устанавливает столбцы для возврата.
     * @param column_names Имена столбцов.
     * @return Ссылка на билдер для цепочки вызовов.
     */
    insert_builder& returning(std::initializer_list<std::string> column_names);

    /**
     * @brief Возвращает AST‑запрос.
     * @return AST‑запрос.
     */
    ast::insert_query to_ast() const noexcept;

    /**
     * @brief Компилирует запрос с помощью переданного диалекта.
     * @param dialect Диалект.
     * @return Скомпилированный запрос.
     */
    compiled_query compile(std::shared_ptr<sql_dialect> dialect) const;

private:
    /// AST‑запрос.
    ast::insert_query query_;
};

} // namespace dsl
} // namespace query_craft
