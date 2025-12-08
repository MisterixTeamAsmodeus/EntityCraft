#pragma once

#include "expression.h"

namespace query_craft {
namespace ast {

/**
 * @brief Описание элемента GROUP BY.
 */
struct group_by_item
{
    /// Выражение.
    expression expr;
};

} // namespace ast
} // namespace query_craft
