#pragma once

namespace query_craft {
namespace ast {

/**
 * @brief Перечисление доступных унарных операторов.
 */
enum class unary_operator
{
    not_, ///< NOT
    minus, ///< - (унарный минус)
    plus, ///< + (унарный плюс)
    is_null, ///< IS NULL
    is_not_null, ///< IS NOT NULL
    exists, ///< EXISTS (для подзапросов)
    /// Произвольный пользовательский унарный оператор, имя задаётся строкой.
    custom,
};

} // namespace ast
} // namespace query_craft
