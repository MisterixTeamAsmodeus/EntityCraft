#pragma once

#include "expression.h"
#include "identifier.h"
#include "query_base.h"

#include <vector>

namespace query_craft {
namespace ast {

/**
 * @brief AST‑узел DELETE‑запроса.
 */
struct delete_query final : query_base
{
    /// Таблица для удаления.
    identifier table;
    /// Условие WHERE.
    expression where;
    /// Список столбцов для возврата.
    std::vector<identifier> returning;
};

} // namespace ast
} // namespace query_craft
