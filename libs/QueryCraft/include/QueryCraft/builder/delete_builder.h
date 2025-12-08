#pragma once

#include "compiled_query.h"

#include <QueryCraft/ast/delete_query.h>
#include <QueryCraft/dialect/sql_dialect.h>

#include <initializer_list>
#include <memory>
#include <string>

namespace query_craft {
namespace dsl {

/**
 * @brief Флюент‑builder для DELETE‑запроса.
 */
class delete_builder
{
public:
    /**
     * @brief Устанавливает таблицу для удаления.
     * @param table_name Имя таблицы.
     * @param schema Схема таблицы (опционально).
     * @return Ссылка на билдер для цепочки вызовов.
     */
    delete_builder& from(const std::string& table_name,
        const std::string& schema = {});

    /**
     * @brief Устанавливает условие WHERE.
     * @param expr Условие.
     * @return Ссылка на билдер для цепочки вызовов.
     */
    delete_builder& where(ast::expression expr);

    /**
     * @brief Устанавливает столбцы для возврата.
     * @param column_names Имена столбцов.
     * @return Ссылка на билдер для цепочки вызовов.
     */
    delete_builder& returning(std::initializer_list<std::string> column_names);

    /**
     * @brief Возвращает AST‑запрос.
     * @return AST‑запрос.
     */
    ast::delete_query to_ast() const noexcept;

    /**
     * @brief Компилирует запрос с помощью переданного диалекта.
     * @param dialect Диалект.
     * @return Скомпилированный запрос.
     */
    compiled_query compile(std::shared_ptr<sql_dialect> dialect) const;

private:
    /// AST‑запрос.
    ast::delete_query query_;
};

} // namespace dsl
} // namespace query_craft
