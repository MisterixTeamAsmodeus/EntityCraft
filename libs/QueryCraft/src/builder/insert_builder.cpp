#include <QueryCraft/builder/compiler.h>

#include <QueryCraft/builder/insert_builder.h>

#include <memory>

namespace query_craft {
namespace dsl {

insert_builder& insert_builder::into(const std::string& table_name,const std::string& schema)
{
    query_.table.name = table_name;
    query_.table.schema = schema;
    return *this;
}

insert_builder& insert_builder::columns(const std::initializer_list<std::string> column_names)
{
    query_.columns.clear();
    for(const auto& name : column_names) {
        ast::identifier id;
        id.name = name;
        query_.columns.push_back(id);
    }
    return *this;
}

insert_builder& insert_builder::columns(const std::vector<std::string>& column_names)
{
    query_.columns.clear();
    for(const auto& name : column_names) {
        ast::identifier id;
        id.name = name;
        query_.columns.push_back(id);
    }
    return *this;
}

insert_builder& insert_builder::values(const std::initializer_list<ast::expression> row)
{
    query_.values.emplace_back(row.begin(), row.end());
    return *this;
}

insert_builder& insert_builder::values(const std::vector<ast::expression>& row)
{
    query_.values.emplace_back(row.begin(), row.end());
    return *this;
}

insert_builder& insert_builder::returning(const std::initializer_list<std::string> column_names)
{
    query_.returning.clear();
    for(const auto& name : column_names) {
        ast::identifier id;
        id.name = name;
        query_.returning.push_back(id);
    }
    return *this;
}

ast::insert_query insert_builder::to_ast() const noexcept
{
    return query_;
}

compiled_query insert_builder::compile(std::shared_ptr<sql_dialect> dialect) const
{
    return sql_compiler(std::move(dialect)).compile(query_);
}

} // namespace dsl
} // namespace query_craft
