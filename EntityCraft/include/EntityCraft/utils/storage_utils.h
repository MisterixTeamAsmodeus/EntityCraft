#pragma once

#include <QueryCraft/querycraft.h>

#include <sstream>
#include <string>

namespace entity_craft {

/**
 * @brief Создает WHERE условие для поиска по ID
 * @param column_name Имя колонки с ID
 * @param id_value Значение ID
 * @return Выражение для WHERE условия
 */
template<typename IdType>
query_craft::ast::expression create_where_condition(const std::string& column_name, const IdType& id_value)
{
    using namespace query_craft::dsl;
    return col(column_name) == param(id_value);
}

/**
 * @brief Генерирует уникальное имя prepared statement на основе SQL запроса и параметров
 * @param base_name Базовое имя для prepared statement (например, "insert_users")
 * @param sql SQL запрос
 * @return Уникальное имя prepared statement
 */
inline std::string generate_unique_statement_name(
    const std::string& base_name,
    const std::string& sql)
{
    // Создаем уникальное имя: base_name + хеш
    std::ostringstream result;
    result << base_name << "_" << std::hex << std::hash<std::string>()(sql);
    return result.str();
}

} // namespace entity_craft
