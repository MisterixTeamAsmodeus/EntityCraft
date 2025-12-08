#pragma once

namespace query_craft {
namespace ast {

/**
 * @brief Перечисление доступных бинарных операторов в выражениях.
 */
enum class binary_operator
{
    equals, ///< =
    not_equals, ///< <>
    less, ///< <
    less_or_equals, ///< <=
    greater, ///< >
    greater_or_equals, ///< >=
    and_, ///< AND
    or_, ///< OR
    in_, ///< IN
    /// Произвольный пользовательский бинарный оператор, имя задаётся строкой.
    custom,
};

} // namespace ast
} // namespace query_craft
