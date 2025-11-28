#pragma once

#include "ReflectionApi/referenceproperty.hpp"

namespace reflection_api {
namespace visitor {

/**
 * @brief Визитор для любых типов проперти
 * @tparam PropertyAction Тип действия для обычной проперти
 * @tparam ReferencePropertyAction Тип действия для ссылочной проперти
 */
template<typename PropertyAction, typename ReferencePropertyAction>
class any_property_visitor
{
public:
    /**
     * @brief Конструктор визитора для любых типов проперти
     * @param property_action Действие для обычной проперти
     * @param reference_property_action Действие для ссылочной проперти
     */
    explicit any_property_visitor(const PropertyAction& property_action, const ReferencePropertyAction& reference_property_action)
        : _property_action(property_action)
        , _reference_property_action(reference_property_action)
    {
    }

    /**
     * @brief Оператор вызова для обычной проперти
     * @tparam ClassType Тип класса
     * @tparam PropertyType Тип свойства
     * @tparam Setter Тип сеттера
     * @tparam Getter Тип геттера
     * @param property Проперти
     */
    template<typename ClassType,
        typename PropertyType,
        typename Setter,
        typename Getter>
    void operator()(const property<ClassType, PropertyType, Setter, Getter>& property) const
    {
        _property_action(property);
    }

    /**
     * @brief Оператор вызова для ссылочной проперти
     * @tparam ClassType Тип класса
     * @tparam PropertyType Тип свойства
     * @tparam Setter Тип сеттера
     * @tparam Getter Тип геттера
     * @param reference_property Ссылочная проперти
     */
    template<typename ClassType,
        typename PropertyType,
        typename Setter,
        typename Getter,
        typename... ReferenceProperties>
    void operator()(const reference_property<ClassType, PropertyType, Setter, Getter, ReferenceProperties...>& reference_property) const
    {
        _reference_property_action(reference_property);
    }

private:
    /// Действие, которое сработает для обычной проперти
    PropertyAction _property_action;
    /// Действие, которое сработает для ссылочной проперти
    ReferencePropertyAction _reference_property_action;
};

/**
 * @brief Создание визитора для любых типов проперти
 * @tparam PropertyAction Тип действия для обычной проперти
 * @tparam ReferencePropertyAction Тип действия для ссылочной проперти
 * @param property_action Действие для обычной проперти
 * @param reference_property_action Действие для ссылочной проперти
 */
template<typename PropertyAction, typename ReferencePropertyAction>
auto make_any_property_visitor(
    PropertyAction&& property_action,
    ReferencePropertyAction&& reference_property_action)
{
    return any_property_visitor<PropertyAction, ReferencePropertyAction>(
        std::forward<PropertyAction>(property_action),
        std::forward<ReferencePropertyAction>(reference_property_action));
}

} // namespace visitor
} // namespace reflection_api

