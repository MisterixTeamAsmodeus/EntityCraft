#include "QueryCraft/ast/delete_query.h"
#include "QueryCraft/ast/expression.h"
#include "QueryCraft/ast/function_call.h"
#include "QueryCraft/ast/identifier.h"
#include "QueryCraft/ast/insert_query.h"
#include "QueryCraft/ast/select_query.h"
#include "QueryCraft/ast/update_query.h"
#include "QueryCraft/dialect/sql_dialect.h"

#include <QueryCraft/builder/compiler.h>

#include <sstream>

namespace query_craft {

namespace {

/**
 * @brief Добавляет идентификатор таблицы в поток (для FROM, JOIN и т.д.).
 * @param stream Поток.
 * @param dialect Диалект.
 * @param id Идентификатор таблицы.
 * @param default_schema Схема по умолчанию для таблиц (пустая строка, если не указана).
 */
void append_table_identifier(std::ostringstream& stream, 
    const std::shared_ptr<sql_dialect>& dialect, 
    const ast::identifier& id,
    const std::string& default_schema = "")
{
    // Для таблиц: если schema не указана, добавляем default_schema
    if(id.schema.empty()) {
        if(!default_schema.empty()) {
            stream << dialect->quote_identifier(default_schema) << '.';
        }
    } else {
        stream << dialect->quote_identifier(id.schema) << '.';
    }
    
    stream << dialect->quote_identifier(id.name);

    if(!id.alias.empty()) {
        stream << " AS " << dialect->quote_identifier(id.alias);
    }
}

/**
 * @brief Добавляет идентификатор колонки в поток.
 * @param stream Поток.
 * @param dialect Диалект.
 * @param id Идентификатор колонки.
 * @param table_context Имя таблицы для префикса колонки (используется только если id.schema пуста).
 * @param default_schema Схема по умолчанию (пустая строка, если не указана).
 */
void append_column_identifier(std::ostringstream& stream, 
    const std::shared_ptr<sql_dialect>& dialect, 
    const ast::identifier& id,
    const std::string& table_context = "",
    const std::string& default_schema = "")
{
    // Если id.schema не пуста, она используется как имя таблицы для колонки (schema.table.column формат)
    // При этом добавляется default_schema перед schema
    // Если id.schema пуста и table_context не пуст, используется table_context как имя таблицы (table.column формат)
    
    if(!id.schema.empty()) {
        // schema.table.column формат (schema используется как имя таблицы, default_schema - схема БД)
        if(!default_schema.empty()) {
            stream << dialect->quote_identifier(default_schema) << '.';
        }
        stream << dialect->quote_identifier(id.schema) << '.';
    } else if(!table_context.empty()) {
        // table.column формат (table_context используется как имя таблицы)
        stream << dialect->quote_identifier(table_context) << '.';
    }

    stream << dialect->quote_identifier(id.name);

    if(!id.alias.empty()) {
        stream << " AS " << dialect->quote_identifier(id.alias);
    }
}

/**
 * @brief Добавляет JOIN выражение в поток (левая часть - одна из доступных таблиц, правая - присоединяемая).
 * @param stream Поток.
 * @param dialect Диалект.
 * @param expr Выражение.
 * @param parameters Параметры.
 * @param parameter_index Индекс параметра.
 * @param compiler Компилятор.
 * @param available_tables Список доступных таблиц для левой части выражения.
 * @param right_table Имя присоединяемой таблицы для правой части выражения.
 * @param default_schema Схема по умолчанию (пустая строка, если не указана).
 */
void append_join_expression(std::ostringstream& stream,
    const std::shared_ptr<sql_dialect>& dialect,
    const ast::expression& expr,
    std::vector<std::string>& parameters,
    std::size_t& parameter_index,
    const sql_compiler& compiler,
    const std::vector<std::string>& available_tables,
    const std::string& right_table,
    const std::string& default_schema = "")
{
    using ast::expression_type;

    switch(expr.type) {
        case expression_type::identifier: {
            // В JOIN условиях без явного указания таблицы, пытаемся определить правильную таблицу
            // Эвристика: если имя колонки содержит имя таблицы (например, "user_id" содержит "user"),
            // то используем соответствующую таблицу, иначе используем первую доступную
            std::string table_context = "";
            if(!expr.identifier_value.schema.empty()) {
                // Если schema указана, используем её как имя таблицы
                table_context = expr.identifier_value.schema;
            } else if(!available_tables.empty()) {
                // Пытаемся найти таблицу по имени колонки (эвристика)
                std::string col_name = expr.identifier_value.name;
                bool found = false;
                for(const auto& table : available_tables) {
                    // Проверяем, содержит ли имя колонки имя таблицы (например, "user_id" содержит "user")
                    if(col_name.find(table) != std::string::npos || 
                       (col_name.size() > table.size() && col_name.substr(0, table.size()) == table)) {
                        table_context = table;
                        found = true;
                        break;
                    }
                }
                // Если не нашли, используем последнюю доступную таблицу (обычно это предыдущая JOIN таблица)
                if(!found && available_tables.size() > 1) {
                    table_context = available_tables.back();
                } else if(!found) {
                    table_context = available_tables[0];
                }
            }
            append_column_identifier(stream, dialect, expr.identifier_value, table_context, default_schema);
            break;
        }
        case expression_type::literal:
            parameters.emplace_back(expr.literal_value.value);
            stream << dialect->placeholder(parameter_index++);
            break;
        case expression_type::parameter:
            parameters.emplace_back(expr.parameter_value.value);
            stream << dialect->placeholder(parameter_index++);
            break;
        case expression_type::function_call: {
            const auto& fn = *expr.function_call_value;
            stream << dialect->function_name(fn.name) << '(';
            for(std::size_t i = 0; i < fn.arguments.size(); ++i) {
                append_join_expression(stream, dialect, fn.arguments[i], parameters, parameter_index, compiler, available_tables, right_table, default_schema);
                if(i + 1 < fn.arguments.size()) {
                    stream << ", ";
                }
            }
            stream << ')';
            break;
        }
        case expression_type::binary: {
            const auto& bin = *expr.binary_value;
            stream << '(';
            // Левая часть - последняя доступная таблица (обычно предыдущая JOIN таблица)
            std::vector<std::string> left_tables = available_tables;
            if(!left_tables.empty()) {
                // Используем последнюю доступную таблицу для левой части
                std::vector<std::string> last_table = { left_tables.back() };
                append_join_expression(stream, dialect, *bin.left, parameters, parameter_index, compiler, last_table, right_table, default_schema);
            } else {
                append_join_expression(stream, dialect, *bin.left, parameters, parameter_index, compiler, available_tables, right_table, default_schema);
            }

            using ast::binary_operator;
            switch(bin.op) {
                case binary_operator::equals:
                    stream << " = ";
                    break;
                case binary_operator::not_equals:
                    stream << " <> ";
                    break;
                case binary_operator::less:
                    stream << " < ";
                    break;
                case binary_operator::less_or_equals:
                    stream << " <= ";
                    break;
                case binary_operator::greater:
                    stream << " > ";
                    break;
                case binary_operator::greater_or_equals:
                    stream << " >= ";
                    break;
                case binary_operator::and_:
                    stream << " AND ";
                    break;
                case binary_operator::or_:
                    stream << " OR ";
                    break;
                case binary_operator::in_:
                    stream << " IN ";
                    break;
                case binary_operator::custom: {
                    const std::string op = dialect->custom_binary_operator(bin.custom_operator_name);
                    stream << ' ' << op << ' ';
                    break;
                }
            }

            // Правая часть - присоединяемая таблица (используем только right_table)
            std::vector<std::string> right_table_list = { right_table };
            append_join_expression(stream, dialect, *bin.right, parameters, parameter_index, compiler, right_table_list, right_table, default_schema);
            stream << ')';
            break;
        }
        case expression_type::unary: {
            const auto& un = *expr.unary_value;
            using ast::unary_operator;
            switch(un.op) {
                case unary_operator::not_:
                    stream << "(NOT ";
                    append_join_expression(stream, dialect, *un.operand, parameters, parameter_index, compiler, available_tables, right_table, default_schema);
                    stream << ')';
                    break;
                case unary_operator::minus:
                    stream << "-";
                    append_join_expression(stream, dialect, *un.operand, parameters, parameter_index, compiler, available_tables, right_table, default_schema);
                    break;
                case unary_operator::plus:
                    stream << "+";
                    append_join_expression(stream, dialect, *un.operand, parameters, parameter_index, compiler, available_tables, right_table, default_schema);
                    break;
                case unary_operator::is_null:
                    stream << '(';
                    append_join_expression(stream, dialect, *un.operand, parameters, parameter_index, compiler, available_tables, right_table, default_schema);
                    stream << " IS NULL)";
                    break;
                case unary_operator::is_not_null:
                    stream << '(';
                    append_join_expression(stream, dialect, *un.operand, parameters, parameter_index, compiler, available_tables, right_table, default_schema);
                    stream << " IS NOT NULL)";
                    break;
                case unary_operator::exists:
                    stream << "(EXISTS ";
                    append_join_expression(stream, dialect, *un.operand, parameters, parameter_index, compiler, available_tables, right_table, default_schema);
                    stream << ')';
                    break;
                case unary_operator::custom: {
                    const std::string op = dialect->custom_unary_operator(un.custom_operator_name);
                    stream << '(' << op << ' ';
                    append_join_expression(stream, dialect, *un.operand, parameters, parameter_index, compiler, available_tables, right_table, default_schema);
                    stream << ')';
                    break;
                }
            }
            break;
        }
        case expression_type::subquery: {
            stream << '(';
            const compiled_query subquery_result = compiler.compile(*expr.subquery_value);
            std::string sql = subquery_result.sql;
            if(!sql.empty() && sql.back() == ';') {
                sql.pop_back();
            }
            stream << sql;
            for(const auto& param : subquery_result.parameters) {
                parameters.push_back(param);
            }
            stream << ')';
            break;
        }
    }
}

/**
 * @brief Добавляет выражение в поток.
 * @param stream Поток.
 * @param dialect Диалект.
 * @param expr Выражение.
 * @param parameters Параметры.
 * @param parameter_index Индекс параметра.
 * @param compiler Компилятор.
 * @param table_context Имя таблицы для префикса колонок.
 * @param default_schema Схема по умолчанию (пустая строка, если не указана).
 */
void append_expression(std::ostringstream& stream,
    const std::shared_ptr<sql_dialect>& dialect,
    const ast::expression& expr,
    std::vector<std::string>& parameters,
    std::size_t& parameter_index,
    const sql_compiler& compiler,
    const std::string& table_context = "",
    const std::string& default_schema = "")
{
    using ast::expression_type;

    switch(expr.type) {
        case expression_type::identifier:
            append_column_identifier(stream, dialect, expr.identifier_value, table_context, default_schema);
            break;
        case expression_type::literal:
            // Литералы всегда выводим как параметры prepared‑statement.
            parameters.emplace_back(expr.literal_value.value);
            stream << dialect->placeholder(parameter_index++);
            break;
        case expression_type::parameter:
            parameters.emplace_back(expr.parameter_value.value);
            stream << dialect->placeholder(parameter_index++);
            break;
        case expression_type::function_call: {
            const auto& fn = *expr.function_call_value;
            stream << dialect->function_name(fn.name) << '(';
            for(std::size_t i = 0; i < fn.arguments.size(); ++i) {
                append_expression(stream, dialect, fn.arguments[i], parameters, parameter_index, compiler, table_context, default_schema);
                if(i + 1 < fn.arguments.size()) {
                    stream << ", ";
                }
            }
            stream << ')';
            break;
        }
        case expression_type::binary: {
            const auto& bin = *expr.binary_value;
            stream << '(';
            append_expression(stream, dialect, *bin.left, parameters, parameter_index, compiler, table_context, default_schema);

            using ast::binary_operator;
            switch(bin.op) {
                case binary_operator::equals:
                    stream << " = ";
                    break;
                case binary_operator::not_equals:
                    stream << " <> ";
                    break;
                case binary_operator::less:
                    stream << " < ";
                    break;
                case binary_operator::less_or_equals:
                    stream << " <= ";
                    break;
                case binary_operator::greater:
                    stream << " > ";
                    break;
                case binary_operator::greater_or_equals:
                    stream << " >= ";
                    break;
                case binary_operator::and_:
                    stream << " AND ";
                    break;
                case binary_operator::or_:
                    stream << " OR ";
                    break;
                case binary_operator::in_:
                    stream << " IN ";
                    break;
                case binary_operator::custom: {
                    const std::string op = dialect->custom_binary_operator(bin.custom_operator_name);
                    stream << ' ' << op << ' ';
                    break;
                }
            }

            append_expression(stream, dialect, *bin.right, parameters, parameter_index, compiler, table_context, default_schema);
            stream << ')';
            break;
        }
        case expression_type::unary: {
            const auto& un = *expr.unary_value;
            using ast::unary_operator;
            switch(un.op) {
                case unary_operator::not_:
                    stream << "(NOT ";
                    append_expression(stream, dialect, *un.operand, parameters, parameter_index, compiler, table_context, default_schema);
                    stream << ')';
                    break;
                case unary_operator::minus:
                    stream << "-";
                    append_expression(stream, dialect, *un.operand, parameters, parameter_index, compiler, table_context, default_schema);
                    break;
                case unary_operator::plus:
                    stream << "+";
                    append_expression(stream, dialect, *un.operand, parameters, parameter_index, compiler, table_context, default_schema);
                    break;
                case unary_operator::is_null:
                    stream << '(';
                    append_expression(stream, dialect, *un.operand, parameters, parameter_index, compiler, table_context, default_schema);
                    stream << " IS NULL)";
                    break;
                case unary_operator::is_not_null:
                    stream << '(';
                    append_expression(stream, dialect, *un.operand, parameters, parameter_index, compiler, table_context, default_schema);
                    stream << " IS NOT NULL)";
                    break;
                case unary_operator::exists:
                    stream << "(EXISTS ";
                    append_expression(stream, dialect, *un.operand, parameters, parameter_index, compiler, table_context, default_schema);
                    stream << ')';
                    break;
                case unary_operator::custom: {
                    const std::string op = dialect->custom_unary_operator(un.custom_operator_name);
                    stream << '(' << op << ' ';
                    append_expression(stream, dialect, *un.operand, parameters, parameter_index, compiler, table_context, default_schema);
                    stream << ')';
                    break;
                }
            }
            break;
        }
        case expression_type::subquery: {
            stream << '(';
            const compiled_query subquery_result = compiler.compile(*expr.subquery_value);
            // Удаляем точку с запятой в конце подзапроса, если она есть
            std::string sql = subquery_result.sql;
            if(!sql.empty() && sql.back() == ';') {
                sql.pop_back();
            }
            stream << sql;
            // Объединяем параметры подзапроса с основными
            for(const auto& param : subquery_result.parameters) {
                parameters.push_back(param);
            }
            stream << ')';
            break;
        }
    }
}

} // namespace

sql_compiler::sql_compiler(std::shared_ptr<sql_dialect> dialect) noexcept
    : dialect_(std::move(dialect))
{
}

compiled_query sql_compiler::compile(const ast::select_query& query) const
{
    std::ostringstream stream;
    compiled_query result;
    std::size_t parameter_index = 1;

    // Генерируем WITH clauses (CTE), если они есть
    if(!query.with_clauses.empty()) {
        stream << "WITH ";
        for(std::size_t i = 0; i < query.with_clauses.size(); ++i) {
            const auto& cte_item = query.with_clauses[i];
            stream << dialect_->quote_identifier(cte_item.name) << " AS (";
            compiled_query cte_result = compile(cte_item.query);
            // Удаляем точку с запятой в конце подзапроса CTE, если она есть
            std::string cte_sql = cte_result.sql;
            if(!cte_sql.empty() && cte_sql.back() == ';') {
                cte_sql.pop_back();
            }
            stream << cte_sql;
            // Объединяем параметры CTE с основными
            for(const auto& param : cte_result.parameters) {
                result.parameters.push_back(param);
            }
            stream << ')';
            if(i + 1 < query.with_clauses.size()) {
                stream << ", ";
            }
        }
        stream << " ";
    }

    stream << "SELECT ";
    // Используем схему таблицы как default_schema для компиляции выражений
    std::string default_schema = query.from.schema.empty() ? "" : query.from.schema;
    if(!query.columns.empty()) {
        for(std::size_t i = 0; i < query.columns.size(); ++i) {
            append_expression(stream, dialect_, query.columns[i], result.parameters, parameter_index, *this, query.from.name, default_schema);
            if(i + 1 < query.columns.size()) {
                stream << ", ";
            }
        }
    } else {
        stream << '*';
    }

    stream << " FROM ";
    append_table_identifier(stream, dialect_, query.from);

    // Собираем список доступных таблиц для JOIN условий
    std::vector<std::string> available_tables;
    available_tables.push_back(query.from.name);
    
    for(const auto& j : query.joins) {
        using ast::join_type;
        switch(j.type) {
            case join_type::inner:
                stream << " INNER JOIN ";
                break;
            case join_type::left:
                stream << " LEFT JOIN ";
                break;
            case join_type::right:
                stream << " RIGHT JOIN ";
                break;
            case join_type::full:
                stream << " FULL JOIN ";
                break;
        }

        append_table_identifier(stream, dialect_, j.table);
        stream << " ON ";
        // Для JOIN условий: левая часть выражения относится к одной из доступных таблиц, правая - к присоединяемой
        append_join_expression(stream, dialect_, j.on, result.parameters, parameter_index, *this, available_tables, j.table.name, default_schema);
        // Добавляем присоединяемую таблицу в список доступных для следующих JOIN
        available_tables.push_back(j.table.name);
    }

    if(query.where.type != ast::expression_type::literal || !query.where.literal_value.value.empty()) {
        stream << " WHERE ";
        append_expression(stream, dialect_, query.where, result.parameters, parameter_index, *this, query.from.name, default_schema);
    }

    if(!query.group_by.empty()) {
        stream << " GROUP BY ";
        for(std::size_t i = 0; i < query.group_by.size(); ++i) {
            append_expression(stream, dialect_, query.group_by[i].expr, result.parameters, parameter_index, *this, query.from.name, default_schema);
            if(i + 1 < query.group_by.size()) {
                stream << ", ";
            }
        }
    }

    if(query.having.type != ast::expression_type::literal || !query.having.literal_value.value.empty()) {
        stream << " HAVING ";
        append_expression(stream, dialect_, query.having, result.parameters, parameter_index, *this, query.from.name, default_schema);
    }

    if(!query.order_by.empty()) {
        stream << " ORDER BY ";
        for(std::size_t i = 0; i < query.order_by.size(); ++i) {
            append_expression(stream, dialect_, query.order_by[i].expr, result.parameters, parameter_index, *this, query.from.name, default_schema);
            stream << (query.order_by[i].ascending ? " ASC" : " DESC");
            if(i + 1 < query.order_by.size()) {
                stream << ", ";
            }
        }
    }

    // Блокировка выбранных строк (SELECT ... FOR UPDATE)
    if(query.for_update) {
        stream << dialect_->select_for_update_clause();
    }

    stream << dialect_->format_limit_offset(query.limit, query.offset);
    stream << ';';

    result.sql = stream.str();
    return result;
}

compiled_query sql_compiler::compile(const ast::insert_query& query) const
{
    std::ostringstream stream;
    compiled_query result;
    std::size_t parameter_index = 1;

    // Резервируем память для параметров заранее
    if(!query.values.empty() && !query.values[0].empty()) {
        result.parameters.reserve(query.values.size() * query.values[0].size());
    }

    stream << "INSERT INTO ";
    append_table_identifier(stream, dialect_, query.table);
    stream << " (";

    for(std::size_t i = 0; i < query.columns.size(); ++i) {
        // В INSERT запросах колонки в списке колонок не имеют префикса таблицы
        append_column_identifier(stream, dialect_, query.columns[i], "", "");
        if(i + 1 < query.columns.size()) {
            stream << ", ";
        }
    }

    stream << ") VALUES ";

    for(std::size_t row_index = 0; row_index < query.values.size(); ++row_index) {
        const auto& row = query.values[row_index];
        stream << '(';
        for(std::size_t i = 0; i < row.size(); ++i) {
            append_expression(stream, dialect_, row[i], result.parameters, parameter_index, *this, query.table.name);
            if(i + 1 < row.size()) {
                stream << ", ";
            }
        }
        stream << ')';
        if(row_index + 1 < query.values.size()) {
            stream << ", ";
        }
    }

    if(!query.returning.empty() && dialect_->supports_returning()) {
        stream << " RETURNING ";
        for(std::size_t i = 0; i < query.returning.size(); ++i) {
            // В RETURNING колонки также не имеют префикса таблицы
            append_column_identifier(stream, dialect_, query.returning[i], "", "");
            if(i + 1 < query.returning.size()) {
                stream << ", ";
            }
        }
    }

    stream << ';';
    result.sql = stream.str();
    return result;
}

compiled_query sql_compiler::compile(const ast::update_query& query) const
{
    std::ostringstream stream;
    compiled_query result;
    std::size_t parameter_index = 1;

    stream << "UPDATE ";
    append_table_identifier(stream, dialect_, query.table);
    stream << " SET ";

    for(std::size_t i = 0; i < query.set_clauses.size(); ++i) {
        // В UPDATE запросах колонки в SET не имеют префикса таблицы
        append_column_identifier(stream, dialect_, query.set_clauses[i].first, "", "");
        stream << " = ";
        // Значения в SET также не требуют префикса таблицы
        append_expression(stream, dialect_, query.set_clauses[i].second, result.parameters, parameter_index, *this, "", "");
        if(i + 1 < query.set_clauses.size()) {
            stream << ", ";
        }
    }

    if(query.where.type != ast::expression_type::literal || !query.where.literal_value.value.empty()) {
        stream << " WHERE ";
        // В UPDATE запросах колонки в WHERE не имеют префикса таблицы
        append_expression(stream, dialect_, query.where, result.parameters, parameter_index, *this, "", "");
    }

    if(!query.returning.empty() && dialect_->supports_returning()) {
        stream << " RETURNING ";
        for(std::size_t i = 0; i < query.returning.size(); ++i) {
            // В RETURNING колонки также не имеют префикса таблицы
            append_column_identifier(stream, dialect_, query.returning[i], "", "");
            if(i + 1 < query.returning.size()) {
                stream << ", ";
            }
        }
    }

    stream << ';';
    result.sql = stream.str();
    return result;
}

compiled_query sql_compiler::compile(const ast::delete_query& query) const
{
    std::ostringstream stream;
    compiled_query result;

    stream << "DELETE FROM ";
    append_table_identifier(stream, dialect_, query.table);

    if(query.where.type != ast::expression_type::literal || !query.where.literal_value.value.empty()) {
        std::size_t parameter_index = 1;
        stream << " WHERE ";
        // В DELETE запросах колонки в WHERE не имеют префикса таблицы
        append_expression(stream, dialect_, query.where, result.parameters, parameter_index, *this, "", "");
    }

    if(!query.returning.empty() && dialect_->supports_returning()) {
        stream << " RETURNING ";
        for(std::size_t i = 0; i < query.returning.size(); ++i) {
            // В RETURNING колонки также не имеют префикса таблицы
            append_column_identifier(stream, dialect_, query.returning[i], "", "");
            if(i + 1 < query.returning.size()) {
                stream << ", ";
            }
        }
    }

    stream << ';';
    result.sql = stream.str();
    return result;
}

} // namespace query_craft
