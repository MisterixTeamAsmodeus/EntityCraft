#pragma once

#include "column.h"
#include "table.h"

#include <QueryCraft/querycraft.h>

#include <functional>
#include <memory>
#include <sstream>
#include <string>

namespace entity_craft {

/**
 * @brief Находит колонку с primary key в таблице
 * @tparam ClassType Тип класса сущности
 * @tparam Columns Типы колонок
 * @param dto Описание таблицы
 * @return Уникальный указатель на имя колонки с primary key, или nullptr если не найдена
 */
template<typename ClassType, typename... Columns>
std::unique_ptr<std::string> get_primary_key_column_name(const table<ClassType, Columns...>& dto)
{
    std::unique_ptr<std::string> primary_key_name;

    dto.for_each([&primary_key_name](const auto& column) {
        column_settings settings = column.settings();
        if(has_setting(settings, column_settings::primary_key)) {
            if(primary_key_name == nullptr) {
                primary_key_name = std::unique_ptr<std::string>(new std::string(column.name()));
            }
        }
    });

    return primary_key_name;
}

/**
 * @brief Создает WHERE условие для поиска по ID
 * @param column_name Имя колонки с ID
 * @param id_value Значение ID
 * @return Выражение для WHERE условия
 */
template<typename IdType>
query_craft::ast::expression create_id_where_condition(const std::string& column_name, const IdType& id_value)
{
    using namespace query_craft::dsl;
    return col(column_name) == param(id_value);
}

/**
 * @brief Получает список имен всех колонок таблицы
 * @tparam ClassType Тип класса сущности
 * @tparam Columns Типы колонок
 * @param dto Описание таблицы
 * @param exclude_primary_key Исключать ли primary key из списка
 * @return Вектор имен колонок
 */
template<typename ClassType, typename... Columns>
std::vector<std::string> get_column_names(const table<ClassType, Columns...>& dto, bool exclude_primary_key = false)
{
    std::vector<std::string> column_names;

    dto.for_each([&column_names, exclude_primary_key](const auto& column) {
        if(exclude_primary_key) {
            column_settings settings = column.settings();
            if(has_setting(settings, column_settings::primary_key) || has_setting(settings, column_settings::auto_increment)) {
                return;
            }
        }
        column_names.push_back(column.name());
    });

    return column_names;
}

/**
 * @brief Проверяет, есть ли в таблице колонка с auto_increment
 * @tparam ClassType Тип класса сущности
 * @tparam Columns Типы колонок
 * @param dto Описание таблицы
 * @return true, если есть auto_increment колонка, иначе false
 */
template<typename ClassType, typename... Columns>
bool has_auto_increment(const table<ClassType, Columns...>& dto)
{
    bool has_auto_inc = false;

    dto.for_each([&has_auto_inc](const auto& column) {
        column_settings settings = column.settings();
        if(has_setting(settings, column_settings::auto_increment)) {
            has_auto_inc = true;
        }
    });

    return has_auto_inc;
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
    // Генерируем хеш
    std::size_t hash_value = std::hash<std::string>()(sql);

    // Создаем уникальное имя: base_name + хеш
    std::ostringstream result;
    result << base_name << "_" << std::hex << hash_value;
    return result.str();
}

} // namespace entity_craft
