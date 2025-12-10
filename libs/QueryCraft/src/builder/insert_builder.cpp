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
    return columns(column_names.begin(), column_names.end());
}

insert_builder& insert_builder::columns(const std::vector<std::string>& column_names)
{
    return columns(column_names.begin(), column_names.end());
}

insert_builder& insert_builder::values(const std::initializer_list<ast::expression> row)
{
    return values(row.begin(), row.end());
}

insert_builder& insert_builder::values(const std::vector<ast::expression>& row)
{

    return values(row.begin(), row.end());
}

insert_builder& insert_builder::returning(const std::initializer_list<std::string> column_names)
{
    return returning(column_names.begin(), column_names.end());
}

insert_builder& insert_builder::returning(const std::vector<std::string>& column_names)
{
    return returning(column_names.begin(), column_names.end());
}

ast::insert_query insert_builder::to_ast() const noexcept
{
    return query_;
}

compiled_query insert_builder::compile(std::shared_ptr<sql_dialect> dialect)
{
    auto compile = sql_compiler(std::move(dialect)).compile(query_);
    query_ = {};
    return compile;
}

} // namespace dsl
} // namespace query_craft
