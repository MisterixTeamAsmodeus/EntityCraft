#pragma once

#include "EntityCraft/reflection/column.h"
#include "EntityCraft/reflection/referencecolumn.h"

#include <utility>

namespace entity_craft {
namespace visitor {

/**
 * @brief Визитор для ссылочной колонки
 * @tparam ReferenceColumnAction Тип действия для ссылочной колонки
 */
template<typename ReferenceColumnAction>
class reference_column_visitor
{
public:
    /**
     * @brief Конструктор визитора для ссылочной колонки
     * @param reference_property_action Действие для ссылочной колонки
     */
    explicit reference_column_visitor(const ReferenceColumnAction& reference_property_action)
        : _reference_property_action(reference_property_action)
    {
    }

    template<typename ClassType,
        typename PropertyType,
        typename Setter,
        typename Getter>
    void operator()(column<ClassType, PropertyType, Setter, Getter>&) const
    {
    }

    /**
     * @brief Оператор вызова для ссылочной колонки
     * @tparam ClassType Тип класса
     * @tparam PropertyType Тип свойства
     * @tparam Setter Тип сеттера
     * @tparam Getter Тип геттера
     * @tparam ReferencePropertyType Тип связанного свойства
     * @tparam ReferenceColumns Типы колонок связанной таблицы
     * @param reference_column Ссылочная колонка
     */
    template<typename ClassType,
        typename PropertyType,
        typename Setter,
        typename Getter,
        typename ReferencePropertyType,
        typename... ReferenceColumns>
    void operator()(reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>& reference_column) const
    {
        _reference_property_action(reference_column);
    }

private:
    /// Действие, которое сработает для ссылочной проперти
    ReferenceColumnAction _reference_property_action;
};

/**
 * @brief Создание визитора для ссылочной колонки
 * @tparam ReferenceColumnAction Тип действия для ссылочной колонки
 * @param reference_column_action Действие для ссылочной колонки
 */
template<typename ReferenceColumnAction>
auto make_reference_column_visitor(ReferenceColumnAction&& reference_column_action)
{
    return reference_column_visitor<ReferenceColumnAction>(std::forward<ReferenceColumnAction>(reference_column_action));
}

} // namespace visitor
} // namespace reflection_api

