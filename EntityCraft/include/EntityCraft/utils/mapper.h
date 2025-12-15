#pragma once

#include "EntityCraft/reflection/column.h"
#include "EntityCraft/reflection/relationtype.h"
#include "EntityCraft/reflection/table.h"
#include "EntityCraft/visitor/referencecolumnvisitor.hpp"

#include <DatabaseAdapter/model/queryresult.hpp>
#include <string>
#include <TypeConverterApi/typeconverter.hpp>
#include <vector>

namespace entity_craft {
/**
 * @brief Пре-декларация функции маппинга строки с учетом зависимостей.
 *
 * Необходимо, чтобы использовать функцию в других шаблонах (например,
 * в map_result_to_entities) до её полной реализации.
 */
template<typename ClassType, typename... Columns>
ClassType map_row_to_entity_with_dependencies(const table<ClassType, Columns...>& dto,
    const database_adapter::query_result::row& row);

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
ClassType map_row_to_entity(const table<ClassType, Columns...>& dto,
    const database_adapter::query_result::row& row,
    bool contains_by_alias = false)
{
    ClassType entity = dto.empty_entity();

    // Используем column_visitor, чтобы обрабатывать только обычные колонки,
    // а не reference_column. reference_column обрабатываются отдельно
    // в map_row_to_entity_with_dependencies.
    dto.for_each(visitor::make_column_visitor([&entity, &row, &dto, contains_by_alias](const auto& column) {
        auto it = contains_by_alias ? row.find(dto.column_alias(column.name())) : row.find(column.name());

        if(it != row.end()) {
            auto db_value = it->second;

            if(db_value != NULL_VALUE && !db_value.empty()) {
                // Конвертация значения из строки в тип свойства
                column.from_string(entity, db_value);
            }
        }
    }));

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
std::vector<std::string> map_entity_to_params(table<ClassType, Columns...>& dto,
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
std::vector<ClassType> map_result_to_entities(const table<ClassType, Columns...>& dto, const database_adapter::query_result& result)
{
    std::vector<ClassType> entities;
    entities.reserve(result.size());

    for(const auto& row : result.data()) {
        // Используем маппинг с учётом зависимостей, чтобы корректно
        // заполнять связи (one_to_many, one_to_one и т.п.) при работе
        // с результатом JOIN.
        entities.push_back(map_row_to_entity_with_dependencies(dto, row));
    }

    return entities;
}

template<typename ClassType,
    typename ReferencePropertyType,
    typename... Columns>
void map_reference_column_value(
    ClassType& entity,
    const table<ClassType, Columns...>& dto,
    std::string&& reference_column_name,
    const ReferencePropertyType& dependent_entity,
    const relation_type relation)
{
    switch(relation) {
        case relation_type::one_to_one:
        case relation_type::one_to_one_inverted:
        case relation_type::many_to_one: {
            // Для one_to_one и many_to_one устанавливаем одну сущность
            dto.set_property_value(entity, dependent_entity, reference_column_name);
            break;
        }

        case relation_type::one_to_many: {
            // Для one_to_many нужно добавить сущность в коллекцию
            dto.append_property_value(entity, dependent_entity, reference_column_name);
            break;
        }
    }
}

/**
 * @brief Предварительное объявление функции маппинга строки с учетом зависимостей
 *
 * Необходимо для использования в extract_dependent_entities, чтобы поддерживать
 * рекурсивный маппинг зависимостей (например, User -> Orders -> OrderItems).
 */
template<typename ClassType, typename... Columns>
ClassType map_row_to_entity_with_dependencies(const table<ClassType, Columns...>& dto,
    const database_adapter::query_result::row& row);

/**
 * @brief Извлекает зависимые сущности из результата JOIN
 * @tparam ReferencePropertyType Тип связанной сущности
 * @tparam ReferenceColumns Типы колонок связанной таблицы
 * @param main_pk_column Имя колонки первичного ключа основной таблицы
 * @param main_fk_column Имя колонки внешнего ключа основной таблицы
 * @param dep_pk_column Имя колонки первичного ключа зависимой таблицы
 * @param dep_fk_column Имя колонки внешнего ключа зависимой таблицы
 * @param ref_column Ссылочная колонка
 * @param row Строка результата JOIN запроса
 */
template<typename ClassType, typename PropertyType, typename Setter, typename Getter, typename ReferencePropertyType, typename... ReferenceColumns>
ReferencePropertyType extract_dependent_entities(
    std::string&& main_pk_column,
    std::string&& main_fk_column,
    std::string&& dep_pk_column,
    std::string&& dep_fk_column,
    const reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>& ref_column,
    const database_adapter::query_result::row& row)
{
    auto ref_table = ref_column.reference_table();

    switch(ref_column.type()) {
        case relation_type::many_to_one:
        case relation_type::one_to_one: {
            auto main_fk_column_it = row.find(main_fk_column);
            auto dep_pk_column_it = row.find(dep_pk_column);

            // Если колонка не найдена или значения не совпадают, возвращаем пустую сущность
            if(main_fk_column_it == row.end() || dep_pk_column_it == row.end() || main_fk_column_it->second != dep_pk_column_it->second) {
                return ref_table.empty_entity();
            }
            break;
        }

        case relation_type::one_to_one_inverted:
        case relation_type::one_to_many:
            // Внешний ключ в зависимой таблице ссылается на первичный ключ основной таблицы
            // main_pk_column = dep_fk_column
            auto main_pk_column_it = row.find(main_pk_column);
            auto dep_fk_column_it = row.find(dep_fk_column);

            // Если колонка не найдена или значения не совпадают, возвращаем пустую сущность
            if(main_pk_column_it == row.end() || dep_fk_column_it == row.end() || main_pk_column_it->second != dep_fk_column_it->second) {
                return ref_table.empty_entity();
            }
            break;
    }

    // Маппим строку в зависимую сущность, используя алиасы и учитывая её собственные зависимости
    return map_row_to_entity_with_dependencies(ref_table, row);
}

/**
 * @brief Маппинг строки с учетом зависимостей из JOIN результата
 * @tparam ClassType Тип класса сущности
 * @tparam Columns Типы колонок
 * @param dto Описание таблицы с метаданными
 * @param row Строка результата JOIN запроса из БД
 * @return Объект сущности с заполненными данными и зависимостями
 */
template<typename ClassType, typename... Columns>
ClassType map_row_to_entity_with_dependencies(const table<ClassType, Columns...>& dto,
    const database_adapter::query_result::row& row)
{
    // Сначала маппим основную сущность
    ClassType entity = map_row_to_entity(dto, row, true);

    // Затем обрабатываем все reference_column
    dto.for_each(visitor::make_reference_column_visitor([&entity, &dto, &row](const auto& ref_column) {
        auto ref_table = ref_column.reference_table();
        relation_type relation = ref_column.type();

        std::string main_pk_alias = dto.column_alias(dto.primary_key_column_name());
        std::string main_fk_alias = dto.column_alias(ref_column.name());
        std::string dep_pk_alias = ref_table.column_alias(ref_table.primary_key_column_name());
        std::string dep_fk_alias= ref_table.column_alias(ref_column.name());

        // Извлекаем зависимую сущность из строки JOIN с учётом её собственных зависимостей
        auto dependent_entity = extract_dependent_entities(
            std::move(main_pk_alias),
            std::move(main_fk_alias),
            std::move(dep_pk_alias),
            std::move(dep_fk_alias),
            ref_column,
            row);

        // Сравнение сущностей по значению требует перегрузки операторов сравнения.
        // Чтобы избежать неявных зависимостей и сохранить универсальность, проверяем,
        // что первичный ключ извлечённой сущности отличается от первичного ключа
        // "пустой" сущности. Таким образом, мы определяем, что зависимость действительно
        // существует в текущей строке результата JOIN.
        const auto empty_pk_value = ref_table.primary_key_column_value(ref_table.empty_entity());
        const auto dependent_pk_value = ref_table.primary_key_column_value(dependent_entity);
        if(dependent_pk_value != empty_pk_value) {
            map_reference_column_value(entity, dto, ref_column.name(), dependent_entity, relation);
        }
    }));

    return entity;
}

template<typename ClassType, typename... Columns>
std::vector<ClassType> merge_entities(const std::vector<ClassType>& entities, table<ClassType, Columns...>& dto)
{
    if(entities.empty()) {
        return {};
    }

    std::unordered_map<std::string, std::vector<ClassType>> mapped_entities;

    for(const auto& entity : entities) {
        auto primary_key_value = dto.primary_key_column_value(entity);
        if(mapped_entities.find(primary_key_value) == mapped_entities.end()) {
            mapped_entities.insert({primary_key_value, {entity}});
        } else {
            mapped_entities[primary_key_value].push_back(entity);
        }
    }

    std::vector<ClassType> merged_entities;
    for(const auto& pair : mapped_entities) {
        auto merged = pair.second.front();
        dto.for_each(visitor::make_reference_column_visitor([&pair, &merged, dto](const auto& ref_column) {
            auto reference_table = ref_column.reference_table();
            if(ref_column.type() == relation_type::one_to_many) {
                std::vector<typename std::decay_t<decltype(reference_table)>::class_type> dependent_entities;
                for(const auto& entity : pair.second) {
                    auto ref_value = ref_column.value(entity);
                    type_converter_api::container_converter<decltype(dependent_entities)>().convert_to_target(dependent_entities, ref_value);
                }

                auto merge_dependent = merge_entities(dependent_entities, reference_table);

                dto.set_property_value(merged, type_converter_api::container_converter<decltype(dependent_entities)>()
                    .template convert<decltype(ref_column.value(merged))>(merge_dependent), ref_column.name());
            }
        }));

        merged_entities.push_back(merged);
    }
    return merged_entities;
}

} // namespace entity_craft
