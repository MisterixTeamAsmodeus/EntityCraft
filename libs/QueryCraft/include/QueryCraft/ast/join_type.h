#pragma once

namespace query_craft {
namespace ast {

/**
 * @brief Тип JOIN‑соединения.
 */
enum class join_type
{
    inner, ///< INNER JOIN
    left, ///< LEFT JOIN
    right, ///< RIGHT JOIN
    full, ///< FULL JOIN
};

} // namespace ast
} // namespace query_craft
