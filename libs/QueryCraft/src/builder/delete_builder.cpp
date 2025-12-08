#include <QueryCraft/builder/compiler.h>
#include <QueryCraft/builder/delete_builder.h>

#include <memory>
#include <utility>

namespace query_craft {
namespace dsl {

delete_builder& delete_builder::from(const std::string& table_name, const std::string& schema)
{
    query_.table.name = table_name;
    query_.table.schema = schema;
    return *this;
}

delete_builder& delete_builder::where(ast::expression expr)
{
    query_.where = std::move(expr);
    return *this;
}

delete_builder& delete_builder::returning(const std::initializer_list<std::string> column_names)
{
    query_.returning.clear();
    for(const auto& name : column_names) {
        ast::identifier id;
        id.name = name;
        query_.returning.push_back(id);
    }
    return *this;
}

ast::delete_query delete_builder::to_ast() const noexcept
{
    return query_;
}

compiled_query delete_builder::compile(std::shared_ptr<sql_dialect> dialect) const
{
    return sql_compiler(std::move(dialect)).compile(query_);
}

} // namespace dsl
} // namespace query_craft
