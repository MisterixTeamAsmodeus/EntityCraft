#pragma once

#include "expression.h"
#include "identifier.h"
#include "query_base.h"

#include <utility>
#include <vector>

namespace query_craft {
namespace ast {

/**
 * @brief AST‑узел UPDATE‑запроса.
 */
struct update_query final : query_base
{
    /// Таблица для обновления.
    identifier table;
    /// Ключи для обновления.
    std::vector<std::pair<identifier, expression>> set_clauses;
    /// Условие WHERE.
    expression where;
    /// Список столбцов для возврата.
    std::vector<identifier> returning;
};

} // namespace ast
} // namespace query_craft
