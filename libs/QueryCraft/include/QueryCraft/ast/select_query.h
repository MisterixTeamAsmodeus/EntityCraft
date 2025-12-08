#pragma once

#include "expression.h"
#include "group_by_item.h"
#include "identifier.h"
#include "join.h"
#include "order_by_item.h"
#include "query_base.h"

#include <cstdint>
#include <string>
#include <vector>

namespace query_craft {
namespace ast {

// Forward declaration для cte (определение ниже, после select_query)
struct cte;

/**
 * @brief AST‑узел SELECT‑запроса.
 */
struct select_query final : query_base
{
    /// CTE (WITH clauses).
    std::vector<cte> with_clauses;
    /// Столбцы.
    std::vector<expression> columns;
    /// Таблица FROM.
    identifier from;
    /// JOIN-ы.
    std::vector<join> joins;
    /// Условие WHERE.
    expression where;
    /// GROUP BY.
    std::vector<group_by_item> group_by;
    expression having;
    /// ORDER BY.
    std::vector<order_by_item> order_by;
    /// LIMIT.
    std::uint64_t limit { 0 };
    /// OFFSET.
    std::uint64_t offset { 0 };
    /// Признак использования блокировки строк (SELECT ... FOR UPDATE).
    bool for_update { false };
};

/**
 * @brief Описание CTE (Common Table Expression).
 *
 * CTE позволяет определить временное именованное результирующее множество,
 * которое существует только в рамках выполнения одного запроса.
 *
 * Пример SQL:
 *   WITH cte_name AS (SELECT ...) SELECT * FROM cte_name;
 */
struct cte
{
    /// Имя CTE.
    std::string name;
    /// SELECT-запрос, определяющий CTE.
    select_query query;
};

} // namespace ast
} // namespace query_craft
