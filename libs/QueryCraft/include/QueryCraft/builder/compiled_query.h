#pragma once

#include <string>
#include <vector>

namespace query_craft {

/**
 * @brief Описывает скомпилированный SQL‑запрос.
 *
 * Содержит итоговую SQL‑строку и последовательность параметров
 * для prepared‑statement.
 */
struct compiled_query
{
    /// Итоговая SQL‑строка.
    std::string sql;
    /// Последовательность параметров для prepared‑statement.
    std::vector<std::string> parameters;
};

} // namespace query_craft
