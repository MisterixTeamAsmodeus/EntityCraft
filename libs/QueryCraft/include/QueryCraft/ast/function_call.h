#pragma once

#include <string>
#include <vector>

namespace query_craft {
namespace ast {

struct expression;

/**
 * @brief Вызов функции (в том числе агрегирующей).
 */
struct function_call
{
    /// Логическое имя функции (например, "COUNT", "LOWER").
    std::string name;
    /// Набор аргументов функции.
    std::vector<expression> arguments;
};

} // namespace ast
} // namespace query_craft
