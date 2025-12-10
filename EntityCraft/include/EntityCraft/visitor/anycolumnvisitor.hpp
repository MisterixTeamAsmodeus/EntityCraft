#pragma once

#include "EntityCraft/reflection/column.h"
#include "EntityCraft/reflection/referencecolumn.h"

#include <utility>

namespace entity_craft {
namespace visitor {

/**
 * @brief Визитор для любых типов колонок
 * @tparam ColumnAction Тип действия для обычной колонки
 * @tparam ReferenceColumnAction Тип действия для ссылочной колонки
 */
template<typename ColumnAction, typename ReferenceColumnAction>
class any_column_visitor
{
public:
    /**
     * @brief Конструктор визитора для любых типов колонок
     * @param property_action Действие для обычной колонки
     * @param reference_property_action Действие для ссылочной колонки
     */
    explicit any_column_visitor(const ColumnAction& property_action, const ReferenceColumnAction& reference_property_action)
        : _property_action(property_action)
        , _reference_property_action(reference_property_action)
    {
    }

    /**
     * @brief Оператор вызова для обычной колонки
     * @tparam ClassType Тип класса
     * @tparam PropertyType Тип свойства
     * @tparam Setter Тип сеттера
     * @tparam Getter Тип геттера
     * @param column Колонка
     */
    template<typename ClassType,
        typename PropertyType,
        typename Setter,
        typename Getter>
    void operator()(column<ClassType, PropertyType, Setter, Getter>& column) const
    {
        _property_action(column);
    }

    /**
     * @brief Оператор вызова для ссылочной колонки
     * @tparam ClassType Тип класса
     * @tparam PropertyType Тип свойства
     * @tparam Setter Тип сеттера
     * @tparam Getter Тип геттера
     * @param reference_column Ссылочная колонка
     */
    template<typename ClassType,
        typename PropertyType,
        typename Setter,
        typename Getter,
        typename... ReferenceProperties>
    void operator()(reference_column<ClassType, PropertyType, Setter, Getter, ReferenceProperties...>& reference_column) const
    {
        _reference_property_action(reference_column);
    }

private:
    /// Действие, которое сработает для обычной проперти
    ColumnAction _property_action;
    /// Действие, которое сработает для ссылочной проперти
    ReferenceColumnAction _reference_property_action;
};

/**
 * @brief Создание визитора для любых типов колонки
 * @tparam ColumnAction Тип действия для обычной колонки
 * @tparam ReferenceColumnAction Тип действия для ссылочной колонки
 * @param column_action Действие для обычной колонки
 * @param reference_column_action Действие для ссылочной колонки
 */
template<typename ColumnAction, typename ReferenceColumnAction>
auto make_any_column_visitor(
    ColumnAction&& column_action,
    ReferenceColumnAction&& reference_column_action)
{
    return any_column_visitor<ColumnAction, ReferenceColumnAction>(
        std::forward<ColumnAction>(column_action),
        std::forward<ReferenceColumnAction>(reference_column_action));
}

} // namespace visitor
} // namespace reflection_api

