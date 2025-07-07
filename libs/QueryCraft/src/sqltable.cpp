#include "QueryCraft/sqltable.h"

namespace {
void insert_with_escaping_character(std::stringstream& sql_stream, const std::string& value)
{
    if(value != query_craft::column_info::null_value()) {
        sql_stream << "'";
    }

    for(int i = 0; i < value.size(); i++) {
        const auto ch = value[i];

        switch(ch) {
            case '\'': {
                sql_stream << "\'\'";
                break;
            }

            case '\\': {
                // Доп обработка для json формата
                if(i + 1 < value.size() && value[i + 1] == '"') {
                    sql_stream << ch;
                } else {
                    sql_stream << "\\\\";
                }

                break;
            }

            default: {
                sql_stream << ch;
            }
        }
    }

    if(value != query_craft::column_info::null_value()) {
        sql_stream << "'";
    }
}
} // namespace

namespace query_craft {

sql_table::sql_table(std::string table_name, std::string scheme,
    const std::initializer_list<column_info>& columns)
    : table(std::move(table_name),
          std::move(scheme), columns)
{
}

sql_table::sql_table(const table& other)
    : table(other)
{
}

sql_table::sql_table(table&& other)
    : table(other)
{
}

sql_table& sql_table::add_row(const row& row)
{
    if(row.empty())
        throw std::logic_error("Ошибка. Попытка добавить пустую строку");

    if(!rows.empty() && rows.begin()->size() != row.size())
        throw std::logic_error("Ошибка. Не совпадает размер строки с уже добавленными в таблицу");

    rows.push_back(row);
    return *this;
}

std::string sql_table::insert_args(const std::initializer_list<column_info>& columns, const bool need_returning, const std::initializer_list<column_info>& returning_columns)
{
    return insert_sql(std::vector<column_info>(columns), need_returning, std::vector<column_info>(returning_columns));
}

std::string sql_table::insert_sql(const std::vector<column_info>& columns, bool need_returning, const std::vector<column_info>& returning_columns)
{
    const auto insert_columns = columns.empty() ? _columns : columns;

    if(insert_columns.empty())
        throw std::invalid_argument("Ошибка. Отсутствует информация о колонках");

    if(rows.empty())
        throw std::invalid_argument("Ошибка. Отсутвуют строки для всатвки");

    if(rows.front().size() != insert_columns.size())
        throw std::invalid_argument("Ошибка. Не совпадает колличество колонок с размером данных");

    std::stringstream sql_stream;
    sql_stream << "INSERT INTO " << table_name() << " (";

    for(const auto& column : insert_columns) {
        sql_stream << "\"" << column.name() << "\""
                   << ", ";
    }

    sql_stream.seekp(-2, std::stringstream::cur);

    sql_stream << ") VALUES";

    for(const auto& row : rows) {
        sql_stream << " (";

        for(const auto& value : row) {
            insert_with_escaping_character(sql_stream, value);
            sql_stream << ", ";
        }

        sql_stream.seekp(-2, std::stringstream::cur);

        sql_stream << "),";
    }

    sql_stream.seekp(-1, std::stringstream::cur);

    if(need_returning) {
        sql_stream << " RETURNING ";

        if(returning_columns.empty()) {
            sql_stream << "*";
        } else {
            for(const auto& returning_column : returning_columns) {
                sql_stream << "\"" << returning_column.name() << "\""
                           << ", ";
            }

            sql_stream.seekp(-2, std::stringstream::cur);
        }
    }

    sql_stream << ";";

    rows.clear();

    return sql_stream.str();
}

std::string sql_table::update_args_sql(const condition_group& condition, const std::initializer_list<column_info>& columns)
{
    return update_sql(condition, std::vector<column_info>(columns));
}

std::string sql_table::update_sql(const condition_group& condition, const std::vector<column_info>& columns)
{
    const auto update_columns = columns.empty() ? _columns : columns;

    if(update_columns.empty())
        throw std::invalid_argument("Ошибка. Отсутствует информация о колонках");

    if(rows.empty())
        throw std::invalid_argument("Ошибка. Отсутвуют строки для всатвки");

    if(rows.front().size() != update_columns.size())
        throw std::invalid_argument("Ошибка. Не совпадает колличество колонок с размером данных");

    if(rows.size() != 1)
        throw std::invalid_argument("Ошибка. В рамках запроса update можно обновить использовать только 1 строку");

    std::stringstream sql_stream;

    sql_stream << "UPDATE " << table_name() << " SET ";

    const auto& row = rows.front();
    for(int i = 0; i < update_columns.size(); i++) {
        sql_stream << "\"" << update_columns[i].name() << "\""
                   << " = ";

        insert_with_escaping_character(sql_stream, row[i]);

        sql_stream << ", ";
    }

    sql_stream.seekp(-2, std::stringstream::cur);

    if(condition.is_valid())
        sql_stream << " WHERE " << condition.unwrap();

    sql_stream << ";";

    rows.clear();

    return sql_stream.str();
}

std::string sql_table::remove_sql(const condition_group& condition) const
{
    std::stringstream sql_stream;

    sql_stream << "DELETE FROM " << table_name();

    if(condition.is_valid())
        sql_stream << " WHERE " << condition.unwrap();

    sql_stream << ";";

    return sql_stream.str();
}

std::string sql_table::select_args_sql(
    const std::initializer_list<join_column>& join_columns,
    const condition_group& condition,
    const std::initializer_list<sort_column>& sort_columns,
    const size_t limit,
    const size_t offset,
    const std::initializer_list<column_info>& columns) const
{
    return select_sql(
        std::vector<join_column>(join_columns),
        condition,
        std::vector<sort_column>(sort_columns),
        limit,
        offset,
        std::vector<column_info>(columns));
}

std::string sql_table::select_sql(
    const std::vector<join_column>& join_columns,
    const condition_group& condition,
    const std::vector<sort_column>& sort_columns,
    const size_t limit,
    const size_t offset,
    const std::vector<column_info>& columns) const
{
    // TODO Добавить реализацию group by, having

    const auto select_columns = columns.empty() ? _columns : columns;

    std::stringstream sql_stream;

    sql_stream << "SELECT ";

    if(!select_columns.empty()) {
        for(const column_info& column : select_columns) {
            sql_stream << column.full_name() << " AS " << column.alias() << ", ";
        }
        sql_stream.seekp(-2, std::stringstream::cur);
    } else {
        sql_stream << "*";
    }

    sql_stream << " FROM " << table_name();

    for(const auto& joinColumn : join_columns) {
        sql_stream << joinColumn << " ";
    }
    if(condition.is_valid())
        sql_stream << " WHERE " << condition.unwrap(condion_view_type::full_name);

    if(!sort_columns.empty()) {
        sql_stream << " ORDER BY ";

        for(const auto& sortColumn : sort_columns) {
            sql_stream << sortColumn.column.alias();

            switch(sortColumn.sort_type) {
                case sort_column::type::asc:
                    sql_stream << " ASC, ";
                    break;
                case sort_column::type::desc:
                    sql_stream << " DESC, ";
                    break;
            }
        }

        sql_stream.seekp(-2, std::stringstream::cur);
    }

    if(limit != 0)
        sql_stream << " LIMIT " << limit;

    if(offset != 0)
        sql_stream << " OFFSET " << offset;

    sql_stream << ";";

    return sql_stream.str();
}

} // namespace query_craft