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

            if(db_value == NULL_VALUE || db_value.empty()) {
                auto null_checker = column.null_checker();
                if(null_checker != nullptr) {
                    auto default_value = column.empty_property();
                    column.set_value(entity, default_value);
                }
            } else {
                // Конвертация значения из строки в тип свойства
                using PropertyType = typename std::decay_t<decltype(column)>::property_type;
                PropertyType property_value = column.empty_property();

                auto converter = column.property_converter();
                if(converter != nullptr) {
                    converter->fill_from_string(property_value, db_value);
                } else {
                    // Fallback на стандартный конвертер
                    type_converter_api::type_converter<PropertyType> default_converter;
                    default_converter.fill_from_string(property_value, db_value);
                }

                column.set_value(entity, property_value);
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
 * @param include_primary_key Флаг, указывающий включать ли primary key в параметры
 * @return Вектор строковых значений параметров в порядке колонок
 */
template<typename ClassType, typename... Columns>
std::vector<std::string> map_entity_to_params(const table<ClassType, Columns...>& dto,
    const ClassType& entity,
    bool include_primary_key = true)
{
    std::vector<std::string> params;

    dto.for_each([&entity, &params, include_primary_key](const auto& column) {
        // Пропускаем primary key, если указано
        if(!include_primary_key) {
            column_settings settings = column.settings();
            if(has_setting(settings, column_settings::primary_key) || has_setting(settings, column_settings::auto_increment)) {
                return;
            }
        }

        // Получаем значение свойства
        using PropertyType = typename std::decay_t<decltype(column)>::property_type;
        PropertyType property_value = column.value(entity);

        // Проверяем на NULL
        auto null_checker = column.null_checker();
        if(null_checker != nullptr && null_checker->is_null(property_value)) {
            const std::string null_value_str = "NULL";
            params.push_back(null_value_str);
        } else {
            // Конвертируем значение в строку
            std::string string_value;
            auto converter = column.property_converter();
            if(converter != nullptr) {
                string_value = converter->convert_to_string(property_value);
            } else {
                // Fallback на стандартный конвертер
                type_converter_api::type_converter<PropertyType> default_converter;
                string_value = default_converter.convert_to_string(property_value);
            }
            params.push_back(string_value);
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
