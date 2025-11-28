#pragma once

#include "ReflectionApi/referenceproperty.hpp"

namespace reflection_api {
namespace visitor {

/**
 * @brief Визитор для ссылочных типов проперти
 * @tparam ReferencePropertyAction Тип действия для ссылочной проперти
 */
template<typename ReferencePropertyAction>
class reference_property_visitor
{
public:
    /**
     * @brief Конструктор визитора для ссылочных типов проперти
     * @param reference_property_action Действие для ссылочной проперти
     */
    explicit reference_property_visitor(const ReferencePropertyAction& reference_property_action)
        : _reference_property_action(reference_property_action)
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
    void operator()(const property<ClassType, PropertyType, Setter, Getter>& /*property*/)
    {
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
    void operator()(const reference_property<ClassType, PropertyType, Setter, Getter, ReferenceProperties...>& reference_property)
    {
        _reference_property_action(reference_property);
    }

private:
    /// Действие которое сработает для ссылочной проперти
    ReferencePropertyAction _reference_property_action;
};

/**
 * @brief Создание визитора для ссылочных типов проперти
 * @tparam ReferencePropertyAction Тип действия для ссылочной проперти
 * @param reference_property_action Действие для ссылочной проперти
 */
template<typename ReferencePropertyAction>
auto make_reference_property_visitor(ReferencePropertyAction&& reference_property_action)
{
    return reference_property_visitor<ReferencePropertyAction>(std::forward<ReferencePropertyAction>(reference_property_action));
}

} // namespace visitor
} // namespace reflection_api

