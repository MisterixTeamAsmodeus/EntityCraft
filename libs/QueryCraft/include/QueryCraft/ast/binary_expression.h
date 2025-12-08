#pragma once

#include "binary_operator.h"

#include <memory>
#include <string>

namespace query_craft {
namespace ast {

struct expression;

/**
 * @brief Узел бинарного выражения.
 */
struct binary_expression
{
    /// Оператор.
    binary_operator op;
    /// Логическое имя пользовательского оператора (для binary_operator::custom).
    std::string custom_operator_name;
    /// Левый операнд.
    std::unique_ptr<expression> left;
    /// Правый операнд.
    std::unique_ptr<expression> right;
};

} // namespace ast
} // namespace query_craft
