#pragma once

#include "ReflectionApi/property.hpp"
#include "ReflectionApi/referenceproperty.hpp"

namespace reflection_api {
namespace visitor {

/**
 * @brief Визитор для обычных проперти
 * @tparam PropertyAction Тип действия для обычной проперти
 */
template<typename PropertyAction>
class property_visitor
{
public:
    /**
     * @brief Конструктор визитора для обычных проперти
     * @param property_action Действие для обычной проперти
     */
    explicit property_visitor(const PropertyAction& property_action)
        : _property_action(property_action)
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
     */
    template<typename ClassType,
        typename PropertyType,
        typename Setter,
        typename Getter,
        typename... ReferenceProperties>
    void operator()(const reference_property<ClassType, PropertyType, Setter, Getter, ReferenceProperties...>& /*reference_property*/) const
    {
    }

private:
    /// Действие, которое сработает для обычной проперти
    PropertyAction _property_action;
};

/**
 * @brief Создание визитора для обычных проперти
 * @tparam PropertyAction Тип действия для обычной проперти
 * @param property_action Действие для обычной проперти
 */
template<typename PropertyAction>
auto make_property_visitor(const PropertyAction& property_action)
{
    return property_visitor<PropertyAction>(property_action);
}

} // namespace visitor
} // namespace reflection_api

