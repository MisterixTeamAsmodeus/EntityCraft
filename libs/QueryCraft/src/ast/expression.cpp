#include <QueryCraft/ast/expression.h>

#include <QueryCraft/ast/binary_expression.h>
#include <QueryCraft/ast/function_call.h>
#include <QueryCraft/ast/select_query.h>
#include <QueryCraft/ast/unary_expression.h>

namespace query_craft {
namespace ast {

expression::expression(const expression& other)
    : type(other.type)
    , identifier_value(other.identifier_value)
    , literal_value(other.literal_value)
    , parameter_value(other.parameter_value)
{
    if(other.function_call_value) {
        function_call_value = std::make_unique<function_call>(*other.function_call_value);
    }

    if(other.binary_value) {
        binary_value = std::make_unique<binary_expression>();
        binary_value->op = other.binary_value->op;
        binary_value->custom_operator_name = other.binary_value->custom_operator_name;
        if(other.binary_value->left) {
            binary_value->left = std::make_unique<expression>(*other.binary_value->left);
        }
        if(other.binary_value->right) {
            binary_value->right = std::make_unique<expression>(*other.binary_value->right);
        }
    }

    if(other.unary_value) {
        unary_value = std::make_unique<unary_expression>();
        unary_value->op = other.unary_value->op;
        unary_value->custom_operator_name = other.unary_value->custom_operator_name;
        if(other.unary_value->operand) {
            unary_value->operand = std::make_unique<expression>(*other.unary_value->operand);
        }
    }

    if(other.subquery_value) {
        subquery_value = std::make_unique<select_query>(*other.subquery_value);
    }
}

expression& expression::operator=(const expression& other)
{
    if(this == &other) {
        return *this;
    }

    type = other.type;
    identifier_value = other.identifier_value;
    literal_value = other.literal_value;
    parameter_value = other.parameter_value;

    if(other.function_call_value) {
        function_call_value = std::make_unique<function_call>(*other.function_call_value);
    } else {
        function_call_value.reset();
    }

    if(other.binary_value) {
        binary_value = std::make_unique<binary_expression>();
        binary_value->op = other.binary_value->op;
        binary_value->custom_operator_name = other.binary_value->custom_operator_name;
        if(other.binary_value->left) {
            binary_value->left = std::make_unique<expression>(*other.binary_value->left);
        } else {
            binary_value->left.reset();
        }
        if(other.binary_value->right) {
            binary_value->right = std::make_unique<expression>(*other.binary_value->right);
        } else {
            binary_value->right.reset();
        }
    } else {
        binary_value.reset();
    }

    if(other.unary_value) {
        unary_value = std::make_unique<unary_expression>();
        unary_value->op = other.unary_value->op;
        unary_value->custom_operator_name = other.unary_value->custom_operator_name;
        if(other.unary_value->operand) {
            unary_value->operand = std::make_unique<expression>(*other.unary_value->operand);
        } else {
            unary_value->operand.reset();
        }
    } else {
        unary_value.reset();
    }

    if(other.subquery_value) {
        subquery_value = std::make_unique<select_query>(*other.subquery_value);
    } else {
        subquery_value.reset();
    }

    return *this;
}

expression::expression(expression&& other) noexcept
    : type(other.type)
    , identifier_value(std::move(other.identifier_value))
    , literal_value(std::move(other.literal_value))
    , parameter_value(std::move(other.parameter_value))
    , function_call_value(std::move(other.function_call_value))
    , binary_value(std::move(other.binary_value))
    , unary_value(std::move(other.unary_value))
    , subquery_value(std::move(other.subquery_value))
{
    // Приводим исходный объект в валидное пустое состояние
    other.type = expression_type::literal;
    other.function_call_value.reset();
    other.binary_value.reset();
    other.unary_value.reset();
    other.subquery_value.reset();
}

expression& expression::operator=(expression&& other) noexcept
{
    if(this == &other) {
        return *this;
    }

    type = other.type;
    identifier_value = std::move(other.identifier_value);
    literal_value = std::move(other.literal_value);
    parameter_value = std::move(other.parameter_value);
    function_call_value = std::move(other.function_call_value);
    binary_value = std::move(other.binary_value);
    unary_value = std::move(other.unary_value);
    subquery_value = std::move(other.subquery_value);

    // Приводим исходный объект в валидное пустое состояние
    other.type = expression_type::literal;
    other.function_call_value.reset();
    other.binary_value.reset();
    other.unary_value.reset();
    other.subquery_value.reset();

    return *this;
}

expression expression::make_identifier(identifier id)
{
    expression e;
    e.type = expression_type::identifier;
    e.identifier_value = std::move(id);
    return e;
}

expression expression::make_literal(literal lit)
{
    expression e;
    e.type = expression_type::literal;
    e.literal_value = std::move(lit);
    return e;
}

expression expression::make_parameter(parameter param)
{
    expression e;
    e.type = expression_type::parameter;
    e.parameter_value = std::move(param);
    return e;
}

expression expression::make_function(function_call fn)
{
    expression e;
    e.type = expression_type::function_call;
    e.function_call_value = std::make_unique<function_call>(std::move(fn));
    return e;
}

expression expression::make_binary(const binary_operator op, expression left, expression right)
{
    expression e;
    e.type = expression_type::binary;
    e.binary_value = std::make_unique<binary_expression>();
    e.binary_value->op = op;
    e.binary_value->custom_operator_name.clear();
    e.binary_value->left = std::make_unique<expression>(std::move(left));
    e.binary_value->right = std::make_unique<expression>(std::move(right));
    return e;
}

expression expression::make_custom_binary(const std::string& logical_name, expression left, expression right)
{
    expression e;
    e.type = expression_type::binary;
    e.binary_value = std::make_unique<binary_expression>();
    e.binary_value->op = binary_operator::custom;
    e.binary_value->custom_operator_name = logical_name;
    e.binary_value->left = std::make_unique<expression>(std::move(left));
    e.binary_value->right = std::make_unique<expression>(std::move(right));
    return e;
}

expression expression::make_unary(const unary_operator op, expression operand)
{
    expression e;
    e.type = expression_type::unary;
    auto unary = std::make_unique<unary_expression>();
    unary->op = op;
    unary->custom_operator_name.clear();
    unary->operand = std::make_unique<expression>(std::move(operand));
    e.unary_value = std::move(unary);
    return e;
}

expression expression::make_custom_unary(const std::string& logical_name, expression operand)
{
    expression e;
    e.type = expression_type::unary;
    auto unary = std::make_unique<unary_expression>();
    unary->op = unary_operator::custom;
    unary->custom_operator_name = logical_name;
    unary->operand = std::make_unique<expression>(std::move(operand));
    e.unary_value = std::move(unary);
    return e;
}

expression expression::make_subquery(select_query query)
{
    expression e;
    e.type = expression_type::subquery;
    e.subquery_value = std::make_unique<select_query>(std::move(query));
    return e;
}

expression::expression() = default;

expression::~expression() = default;

} // namespace ast
} // namespace query_craft