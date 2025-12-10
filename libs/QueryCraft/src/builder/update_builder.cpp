#include <QueryCraft/builder/compiler.h>
#include <QueryCraft/builder/update_builder.h>

#include <memory>
#include <utility>

namespace query_craft {
namespace dsl {

update_builder& update_builder::table(const std::string& table_name,
    const std::string& schema)
{
    query_.table.name = table_name;
    query_.table.schema = schema;
    return *this;
}

update_builder& update_builder::set(const std::string& column_name, ast::expression expr)
{
    ast::identifier id;
    id.name = column_name;
    query_.set_clauses.emplace_back(std::move(id), std::move(expr));
    return *this;
}

update_builder& update_builder::where(ast::expression expr)
{
    query_.where = std::move(expr);
    return *this;
}

update_builder& update_builder::returning(const std::initializer_list<std::string> column_names)
{
    return returning(column_names.begin(), column_names.end());
}

update_builder& update_builder::returning(const std::vector<std::string>& column_names)
{
    return returning(column_names.begin(), column_names.end());
}

ast::update_query update_builder::to_ast() const noexcept
{
    return query_;
}

compiled_query update_builder::compile(std::shared_ptr<sql_dialect> dialect) const
{
    return sql_compiler(std::move(dialect)).compile(query_);
}

} // namespace dsl
} // namespace query_craft
