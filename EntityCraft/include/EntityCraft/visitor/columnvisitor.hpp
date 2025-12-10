#pragma once

#include "EntityCraft/reflection/column.h"
#include "EntityCraft/reflection/referencecolumn.h"

#include <utility>

namespace entity_craft {

template<typename ClassType,
    typename PropertyType,
    typename Setter,
    typename Getter,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
class reference_column;

template<typename ClassType,
    typename PropertyType,
    typename Setter,
    typename Getter>
class column;


namespace visitor {

/**
 * @brief Визитор для обычной колонки
 * @tparam ColumnAction Тип действия для обычной колонки
 */
template<typename ColumnAction>
class column_visitor
{
public:
    /**
     * @brief Конструктор визитора для обычной колонки
     * @param property_action Действие для обычной колонки
     */
    explicit column_visitor(const ColumnAction& property_action)
        : _property_action(property_action)
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

    template<typename ClassType,
        typename PropertyType,
        typename Setter,
        typename Getter,
        typename ReferencePropertyType,
        typename... ReferenceColumns>
    void operator()(reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>&) const
    {
    }

private:
    /// Действие, которое сработает для обычной проперти
    ColumnAction _property_action;
};

/**
 * @brief Создание визитора для обычной колонки
 * @tparam ColumnAction Тип действия для обычной колонки
 * @param column_action Действие для обычной колонки
 */
template<typename ColumnAction>
auto make_column_visitor(ColumnAction&& column_action)
{
    return column_visitor<ColumnAction>(std::forward<ColumnAction>(column_action));
}

} // namespace visitor
} // namespace entity_craft
