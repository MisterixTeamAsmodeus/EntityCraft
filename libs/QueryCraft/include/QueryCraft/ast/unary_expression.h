#pragma once

#include "unary_operator.h"

#include <memory>
#include <string>

namespace query_craft {
namespace ast {

struct expression;

/**
 * @brief Узел унарного выражения.
 */
struct unary_expression
{
    /// Оператор. Например, NOT, -, +, IS NULL, EXISTS, etc.
    unary_operator op;
    /// Логическое имя пользовательского оператора (для unary_operator::custom).
    std::string custom_operator_name;
    /// Операнд. Например, expression, literal, parameter, etc.
    std::unique_ptr<expression> operand;
};

} // namespace ast
} // namespace query_craft
