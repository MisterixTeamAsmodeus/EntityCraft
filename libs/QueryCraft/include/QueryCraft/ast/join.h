#pragma once

#include "expression.h"
#include "identifier.h"
#include "join_type.h"

namespace query_craft {
namespace ast {

/**
 * @brief Описание JOIN‑соединения.
 */
struct join
{
    /// Тип JOIN.
    join_type type { join_type::inner };
    /// Таблица для соединения.
    identifier table;
    /// Условие соединения.
    expression on;
};

} // namespace ast
} // namespace query_craft
