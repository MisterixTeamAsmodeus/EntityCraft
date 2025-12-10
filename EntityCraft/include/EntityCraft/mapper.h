#pragma once

#include "column.h"
#include "referencecolumn.h"
#include "table.h"

#include <DatabaseAdapter/model/queryresult.hpp>
#include <optional>
#include <ReflectionApi/entity.hpp>
#include <string>
#include <TypeConverterApi/typeconverter.hpp>
#include <vector>

namespace entity_craft {

/**
 * @brief Преобразует строку результата запроса в объект сущности
 * @tparam ClassType Тип класса сущности
 * @tparam Columns Типы колонок
 * @param dto Описание таблицы с метаданными
 * @param row Строка результата запроса из БД
 * @param contains_by_alias Флаг поиска колонки по алиасу или по имени
 * @return Объект сущности с заполненными данными
 */
template<typename ClassType, typename... Columns>
ClassType map_row_to_entity(const table<ClassType, Columns...>& dto, const database_adapter::query_result::row& row, bool contains_by_alias = false)
{
    ClassType entity = dto.empty_entity();

    dto.for_each([&entity, &row, &dto, contains_by_alias](const auto& column) {
        auto it = contains_by_alias ? row.find(dto.column_alias(column.name())) : row.find(column.name());

        if(it != row.end()) {
            auto db_value = it->second;

            if(db_value != NULL_VALUE && !db_value.empty()) {
                // Конвертация значения из строки в тип свойства
                column.from_string(entity, db_value);
            }
        }
    });

    return entity;
}

/**
 * @brief Преобразует объект сущности в параметры для prepared statement
 * @tparam ClassType Тип класса сущности
 * @tparam Columns Типы колонок
 * @param dto Описание таблицы с метаданными
 * @param entity Объект сущности
 * @param include_auto_increment Флаг, указывающий включать ли auto increment в параметры
 * @return Вектор строковых значений параметров в порядке колонок
 */
template<typename ClassType, typename... Columns>
std::vector<std::string> map_entity_to_params(const table<ClassType, Columns...>& dto,
    const ClassType& entity,
    bool include_auto_increment = true)
{
    std::vector<std::string> params;

    dto.for_each([&entity, &params, include_auto_increment](const auto& column) {
        // Пропускаем auto increment, если указано
        if(!include_auto_increment) {
            auto settings = column.settings();
            if(has_setting(settings, column_settings::auto_increment)) {
                return;
            }
        }

        if(column.is_null_value(entity)) {
            params.emplace_back(NULL_VALUE);
        } else {
            params.push_back(column.to_string(entity));
        }
    });

    return params;
}

/**
 * @brief Преобразует несколько строк результата в вектор объектов
 * @tparam ClassType Тип класса сущности
 * @tparam Columns Типы колонок
 * @param dto Описание таблицы с метаданными
 * @param result Результат запроса из БД
 * @return Вектор объектов сущностей
 */
template<typename ClassType, typename... Columns>
std::vector<ClassType> map_result_to_entities(const table<ClassType, Columns...>& dto,
    const database_adapter::query_result& result)
{
    std::vector<ClassType> entities;
    entities.reserve(result.size());

    for(const auto& row : result.data()) {
        entities.push_back(map_row_to_entity(dto, row, true));
    }

    return entities;
}

} // namespace entity_craft
