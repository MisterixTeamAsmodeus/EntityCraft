#pragma once

#include "expression.h"

namespace query_craft {
namespace ast {

/**
 * @brief Описание элемента ORDER BY.
 */
struct order_by_item
{
    /// Выражение.
    expression expr;
    /// Флаг сортировки по возрастанию.
    bool ascending { true };
};

} // namespace ast
} // namespace query_craft
