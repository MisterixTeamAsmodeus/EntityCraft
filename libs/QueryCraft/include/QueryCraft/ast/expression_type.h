#pragma once

namespace query_craft {
namespace ast {

/**
 * @brief Перечисление возможных типов выражений.
 */
enum class expression_type
{
    identifier, ///< Идентификатор.
    literal, ///< Литерал (константа).
    parameter, ///< Параметр.
    function_call, ///< Вызов функции.
    binary, ///< Бинарное выражение.
    unary, ///< Унарное выражение.
    subquery, ///< Подзапрос.
};

} // namespace ast
} // namespace query_craft
