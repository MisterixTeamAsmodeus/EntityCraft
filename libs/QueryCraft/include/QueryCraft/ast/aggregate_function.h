#pragma once

namespace query_craft {
namespace ast {

/**
 * @brief Логическое имя стандартной агрегирующей функции.
 *
 * Логическое имя впоследствии конвертируется диалектом
 * в конкретное строковое представление.
 */
enum class aggregate_function
{
    count, // COUNT(*)
    sum, // SUM(column)
    avg, // AVG(column)
    min, // MIN(column)
    max, // MAX(column)
};

} // namespace ast
} // namespace query_craft
