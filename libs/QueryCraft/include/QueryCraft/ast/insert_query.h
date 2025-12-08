#pragma once

#include "expression.h"
#include "identifier.h"
#include "query_base.h"

#include <vector>

namespace query_craft {
namespace ast {

/**
 * @brief AST‑узел INSERT‑запроса.
 */
struct insert_query final : query_base
{
    /// Таблица для вставки.
    identifier table;
    /// Столбцы для вставки.
    std::vector<identifier> columns;
    /// Значения для вставки.
    std::vector<std::vector<expression>> values;
    /// Список столбцов для возврата.
    std::vector<identifier> returning;
};

} // namespace ast
} // namespace query_craft
