#pragma once

#include "EntityCraft/reflection/referencecolumn.h"
#include "EntityCraft/reflection/relationtype.h"
#include "EntityCraft/reflection/table.h"
#include "EntityCraft/visitor/referencecolumnvisitor.hpp"

#include <QueryCraft/builder/dsl.h>
#include <QueryCraft/builder/select_builder.h>

#include <string>
#include <vector>

namespace entity_craft {

/**
 * @brief Информация о зависимости для построения JOIN
 */
struct dependency_info
{
    /// Имя колонки первичного ключа основной таблицы
    std::string main_pk_column;
    /// Имя колонки внешнего ключа основной таблицы
    std::string main_fk_column;
    /// Имя колонки внешнего ключа зависимой таблицы
    std::string dep_fk_column;
    /// Имя колонки первичного ключа зависимой таблицы
    std::string dep_pk_column;

    /// Имя зависимой таблицы
    std::string dep_table_name;
    /// Схема зависимой таблицы
    std::string dep_table_scheme;
    /// Тип связи между основной и зависимой таблицами
    relation_type relation;
};

/**
 * @brief Создает условие JOIN на основе типа связи и информации о ключах
 * @param dependency_info Имя колонки первичного ключа основной таблицы
 * @return Выражение для условия ON в JOIN
 */
inline query_craft::ast::expression create_join_condition(const dependency_info& dependency_info)
{
    using namespace query_craft::dsl;

    switch(dependency_info.relation) {
        case relation_type::many_to_one:
        case relation_type::one_to_one:
            // Внешний ключ в исходной таблице ссылается на первичный ключ зависимой таблицы
            // main_fk_column = dep_pk_column
            return col(dependency_info.main_fk_column) == col(dependency_info.dep_pk_column);

        case relation_type::one_to_one_inverted:
        case relation_type::one_to_many:
            // Внешний ключ в зависимой таблице ссылается на первичный ключ основной таблицы
            // main_pk_column = dep_fk_column
            return col(dependency_info.main_pk_column) == col(dependency_info.dep_fk_column);

        default:
            throw std::invalid_argument("Invalid relation type");
    }

    return {};
}

/**
 * @brief Добавляет колонки таблицы в SELECT запрос с алиасами
 * @tparam ClassType Тип класса сущности
 * @tparam Columns Типы колонок
 * @param builder Билдер SELECT запроса
 * @param dto Описание таблицы
 */
template<typename ClassType, typename... Columns>
void add_table_columns_to_select(query_craft::dsl::select_builder& builder, const table<ClassType, Columns...>& dto)
{
    std::vector<query_craft::ast::expression> columns;
    for(const auto& column_name : dto.columns_name()) {
        columns.push_back(query_craft::dsl::col(column_name, dto.column_alias(column_name)));
    }
    builder.columns(columns);
}

/**
 * @brief Собирает информацию о зависимостях из таблицы и добавляет колонки таблицы в SELECT
 * @tparam ClassType Тип класса сущности
 * @tparam Columns Типы колонок
 * @param builder Билдер SELECT запроса
 * @param dto Описание таблицы
 * @return Вектор информации о зависимостях
 */
template<typename ClassType, typename... Columns>
std::vector<dependency_info> collect_dependencies(query_craft::dsl::select_builder& builder, const table<ClassType, Columns...>& dto)
{
    std::vector<dependency_info> dependencies;

    dto.for_each(visitor::make_reference_column_visitor([&dependencies, &dto, &builder](const auto& ref_column) {
        dependency_info info;
        info.relation = ref_column.type();
        info.main_pk_column = dto.primary_key_column_name();
        // Получаем информацию о связанной таблице
        auto ref_table = ref_column.reference_table();

        info.dep_table_name = ref_table.table_name();
        info.dep_table_scheme = ref_table.scheme();

        add_table_columns_to_select(builder, ref_table);

        switch(info.relation) {
            case relation_type::many_to_one:
            case relation_type::one_to_one: {
                info.main_fk_column = ref_column.name();
                info.dep_pk_column = ref_table.primary_key_column_name();
                break;
            }

            case relation_type::one_to_one_inverted:
            case relation_type::one_to_many: {
                info.main_pk_column = dto.primary_key_column_name();
                info.dep_fk_column = ref_column.name();
                break;
            }
        }

        dependencies.push_back(info);

        auto inline_dependencies = collect_dependencies(builder, ref_table);
        dependencies.insert(dependencies.end(), inline_dependencies.begin(), inline_dependencies.end());
    }));

    return dependencies;
}

/**
 * @brief Рекурсивно добавляет JOIN для зависимостей в SELECT запрос
 * @tparam ClassType Тип класса сущности
 * @tparam Columns Типы колонок
 * @param builder Билдер SELECT запроса
 * @param dto Описание таблицы
 */
template<typename ClassType, typename... Columns>
void add_joins_for_dependencies(query_craft::dsl::select_builder& builder, const table<ClassType, Columns...>& dto)
{
    for(const auto& dep_info : collect_dependencies(builder, dto)) {
        // Создаем условие JOIN
        auto join_condition = create_join_condition(dep_info);

        // Добавляем JOIN
        // QueryCraft компилятор обработает их правильно
        builder.join(query_craft::ast::join_type::left, dep_info.dep_table_name, join_condition, dep_info.dep_table_scheme);
    }
}

/**
 * @brief Строит SELECT запрос с JOIN для загрузки всех зависимостей
 * @tparam ClassType Тип класса сущности
 * @tparam Columns Типы колонок
 * @param dto Описание таблицы
 * @return Билдер SELECT запроса с добавленными JOIN
 */
template<typename ClassType, typename... Columns>
query_craft::dsl::select_builder build_select_with_joins(const table<ClassType, Columns...>& dto)
{
    using namespace query_craft::dsl;

    select_builder builder;

    // Добавляем FROM с алиасом
    builder.from(dto.table_name(), dto.scheme());

    // Добавляем колонки основной таблицы в SELECT
    add_table_columns_to_select(builder, dto);
    // Собираем зависимости и добавляем JOIN
    add_joins_for_dependencies(builder, dto);

    return builder;
}

/**
 * @brief Строит SELECT запрос с JOIN для загрузки всех зависимостей с условием WHERE
 * @tparam ClassType Тип класса сущности
 * @tparam Columns Типы колонок
 * @param dto Описание таблицы
 * @param condition Условие WHERE
 * @return Билдер SELECT запроса с добавленными JOIN
 */
template<typename ClassType, typename... Columns>
query_craft::dsl::select_builder build_select_with_joins(const table<ClassType, Columns...>& dto,const query_craft::ast::expression& condition)
{
    auto builder = build_select_with_joins(dto);
    builder.where(condition);
    return builder;
}
} // namespace entity_craft
