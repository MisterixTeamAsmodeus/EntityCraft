#include "QueryCraft/table.h"

namespace query_craft {

table::table(std::string table_name, std::string scheme, const std::initializer_list<column_info>& columns)
    : table(std::move(table_name), std::move(scheme), columns.begin(), columns.end())
{
}

table& table::add_column(column_info& column)
{
    std::stringstream alias_stream;
    if(!_scheme.empty())
        alias_stream << _scheme << "_";

    alias_stream << _table_name << "_" << column.name();
    column.set_alias(alias_stream.str());

    std::stringstream full_name_stream;
    full_name_stream << table_name() << "."
                     << "\"" << column.name() << "\"";
    column.set_full_name(full_name_stream.str());

    if(_columns_map.find(column.name()) != _columns_map.end())
        throw std::logic_error("Ошибка. Дублируется название колонки");

    _columns.push_back(column);
    _columns_map.emplace(column.name(), column);

    return *this;
}

std::string table::table_name() const
{
    std::stringstream table_name_stream;

    if(!_scheme.empty())
        table_name_stream << "\"" << _scheme << "\""
                          << ".";

    table_name_stream << "\"" << _table_name << "\"";

    return table_name_stream.str();
}

column_info table::column(const std::string& name) const
{
    if(_columns_map.find(name) == _columns_map.end())
        throw std::invalid_argument("Данной колонки нет в таблице");

    return _columns_map.find(name)->second;
}

column_info table::column(const int index) const
{
    if(_columns.size() <= index || index < 0)
        throw std::invalid_argument("Неправельный индекс колонки");

    return _columns[index];
}

std::vector<column_info> table::columns() const
{
    return _columns;
}

} // namespace query_craft