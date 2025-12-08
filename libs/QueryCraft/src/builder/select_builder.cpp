#include <QueryCraft/builder/compiler.h>
#include <QueryCraft/builder/select_builder.h>

#include <memory>
#include <utility>

namespace query_craft {
namespace dsl {

select_builder& select_builder::from(const std::string& table_name, const std::string& schema)
{
    query_.from.name = table_name;
    query_.from.schema = schema;
    return *this;
}

select_builder& select_builder::columns(const std::initializer_list<ast::expression> columns)
{
    query_.columns.assign(columns.begin(), columns.end());
    return *this;
}

select_builder& select_builder::columns(const std::vector<ast::expression>& columns)
{
    query_.columns.assign(columns.begin(), columns.end());
    return *this;
}

select_builder& select_builder::where(ast::expression expr)
{
    query_.where = std::move(expr);
    return *this;
}

select_builder& select_builder::group_by(const std::initializer_list<ast::expression> expressions)
{
    query_.group_by.clear();
    for(const auto& expr : expressions) {
        ast::group_by_item item;
        item.expr = expr;
        query_.group_by.push_back(item);
    }
    return *this;
}

select_builder& select_builder::having(ast::expression expr)
{
    query_.having = std::move(expr);
    return *this;
}

select_builder& select_builder::order_by(const std::initializer_list<ast::order_by_item> items)
{
    query_.order_by.assign(items.begin(), items.end());
    return *this;
}

select_builder& select_builder::limit(const std::uint64_t value)
{
    query_.limit = value;
    return *this;
}

select_builder& select_builder::offset(const std::uint64_t value)
{
    query_.offset = value;
    return *this;
}

select_builder& select_builder::for_update()
{
    query_.for_update = true;
    return *this;
}

select_builder& select_builder::join(const ast::join_type type, const std::string& table_name, ast::expression on, const std::string& schema)
{
    ast::join j;
    j.type = type;
    j.table.name = table_name;
    j.table.schema = schema;
    j.on = std::move(on);
    query_.joins.push_back(std::move(j));
    return *this;
}

select_builder& select_builder::inner_join(const std::string& table_name,ast::expression on,const std::string& schema)
{
    return join(ast::join_type::inner, table_name, std::move(on), schema);
}

select_builder& select_builder::left_join(const std::string& table_name,ast::expression on,const std::string& schema)
{
    return join(ast::join_type::left, table_name, std::move(on), schema);
}

select_builder& select_builder::right_join(const std::string& table_name,ast::expression on,const std::string& schema)
{
    return join(ast::join_type::right, table_name, std::move(on), schema);
}

select_builder& select_builder::full_join(const std::string& table_name,ast::expression on,const std::string& schema)
{
    return join(ast::join_type::full, table_name, std::move(on), schema);
}

select_builder& select_builder::with(const std::string& name, const ast::select_query& query)
{
    ast::cte cte_item;
    cte_item.name = name;
    cte_item.query = query;
    query_.with_clauses.push_back(std::move(cte_item));
    return *this;
}

ast::select_query select_builder::to_ast() const noexcept
{
    return query_;
}

compiled_query select_builder::compile(std::shared_ptr<sql_dialect> dialect) const
{
    return sql_compiler(std::move(dialect)).compile(query_);
}

} // namespace dsl
} // namespace query_craft
