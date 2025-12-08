#include "QueryCraft/builder/dsl.h"

#include "QueryCraft/ast/select_query.h"

namespace query_craft {
namespace dsl {
ast::expression col(const std::string& name, const std::string& alias, const std::string& schema)
{
    ast::identifier id;
    id.schema = schema;
    id.name = name;
    id.alias = alias;
    return ast::expression::make_identifier(std::move(id));
}

ast::expression col(const char* name, const char* alias, const char* schema)
{
    ast::identifier id;
    if(schema) {
        id.schema = schema;
    }
    id.name = name;
    if(alias) {
        id.alias = alias;
    }
    return ast::expression::make_identifier(std::move(id));
}

ast::expression param(const char* v)
{
    ast::parameter p;
    p.value = v;
    return ast::expression::make_parameter(std::move(p));
}

ast::expression func(const std::string& name, const std::initializer_list<ast::expression> args)
{
    ast::function_call f;
    f.name = name;
    f.arguments.assign(args.begin(), args.end());
    return ast::expression::make_function(std::move(f));
}

ast::expression count(const std::initializer_list<ast::expression> args)
{
    return func("COUNT", args);
}

ast::expression sum(const std::initializer_list<ast::expression> args)
{
    return func("SUM", args);
}

ast::expression avg(const std::initializer_list<ast::expression> args)
{
    return func("AVG", args);
}

ast::expression min(const std::initializer_list<ast::expression> args)
{
    return func("MIN", args);
}

ast::expression max(const std::initializer_list<ast::expression> args)
{
    return func("MAX", args);
}

ast::expression custom_aggregate(const std::string& logical_name, const std::initializer_list<ast::expression> args)
{
    return func(logical_name, args);
}

ast::expression custom_op(const ast::expression& left, const std::string& logical_operator_name, const ast::expression& right)
{
    return ast::expression::make_custom_binary(logical_operator_name, left, right);
}

ast::expression subquery(const ast::select_query& query)
{
    return ast::expression::make_subquery(query);
}

ast::expression and_(std::initializer_list<ast::expression> conditions)
{
    if(conditions.size() == 0) {
        return ast::expression::make_literal(ast::literal {});
    }
    if(conditions.size() == 1) {
        return *conditions.begin();
    }

    auto it = conditions.begin();
    ast::expression result = *it;
    ++it;
    for(; it != conditions.end(); ++it) {
        result = ast::expression::make_binary(ast::binary_operator::and_, result, *it);
    }
    return result;
}

ast::expression or_(std::initializer_list<ast::expression> conditions)
{
    if(conditions.size() == 0) {
        return ast::expression::make_literal(ast::literal {});
    }
    if(conditions.size() == 1) {
        return *conditions.begin();
    }

    auto it = conditions.begin();
    ast::expression result = *it;
    ++it;
    for(; it != conditions.end(); ++it) {
        result = ast::expression::make_binary(ast::binary_operator::or_, result, *it);
    }
    return result;
}

ast::expression in_(const ast::expression& left, const ast::expression& right)
{
    return ast::expression::make_binary(ast::binary_operator::in_, left, right);
}
} // namespace dsl
} // namespace query_craft

query_craft::ast::expression operator!(const query_craft::ast::expression& expr)
{
    return query_craft::ast::expression::make_unary(query_craft::ast::unary_operator::not_, expr);
}

query_craft::ast::expression operator&&(const query_craft::ast::expression& left, const query_craft::ast::expression& right)
{
    return query_craft::ast::expression::make_binary(query_craft::ast::binary_operator::and_, left, right);
}

query_craft::ast::expression operator||(const query_craft::ast::expression& left, const query_craft::ast::expression& right)
{
    return query_craft::ast::expression::make_binary(query_craft::ast::binary_operator::or_, left, right);
}

query_craft::ast::expression operator==(const query_craft::ast::expression& left, const query_craft::ast::expression& right)
{
    return query_craft::ast::expression::make_binary(query_craft::ast::binary_operator::equals, left, right);
}

query_craft::ast::expression operator!=(const query_craft::ast::expression& left, const query_craft::ast::expression& right)
{
    return query_craft::ast::expression::make_binary(query_craft::ast::binary_operator::not_equals, left, right);
}

query_craft::ast::expression operator<(const query_craft::ast::expression& left, const query_craft::ast::expression& right)
{
    return query_craft::ast::expression::make_binary(query_craft::ast::binary_operator::less, left, right);
}

query_craft::ast::expression operator<=(const query_craft::ast::expression& left, const query_craft::ast::expression& right)
{
    return query_craft::ast::expression::make_binary(query_craft::ast::binary_operator::less_or_equals, left, right);
}

query_craft::ast::expression operator>(const query_craft::ast::expression& left, const query_craft::ast::expression& right)
{
    return query_craft::ast::expression::make_binary(query_craft::ast::binary_operator::greater, left, right);
}

query_craft::ast::expression operator>=(const query_craft::ast::expression& left, const query_craft::ast::expression& right)
{
    return query_craft::ast::expression::make_binary(query_craft::ast::binary_operator::greater_or_equals, left, right);
}