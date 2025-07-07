#pragma once

#include "ReflectionApi/referenceproperty.h"

namespace reflection_api {
namespace visitor {

/**
 * Визитор для любых типов проперти
 */
template<typename PropertyAction, typename ReferencePropertyAction>
class any_property_visitor
{
public:
    explicit any_property_visitor(const PropertyAction& property_action, const ReferencePropertyAction& reference_property_action)
        : _property_action(property_action)
        , _reference_property_action(reference_property_action)
    {
    }

    template<typename ClassType,
        typename PropertyType,
        typename Setter,
        typename Getter>
    void operator()(property<ClassType, PropertyType, Setter, Getter>& property)
    {
        _property_action(property);
    }

    template<typename ClassType,
        typename PropertyType,
        typename ReferenceColumnType,
        typename Setter,
        typename Getter,
        typename... ReferenceProperties>
    void operator()(reference_property<ClassType, PropertyType, ReferenceColumnType, Setter, Getter, ReferenceProperties...>& reference_property)
    {
        _reference_property_action(reference_property);
    }

private:
    /// Действие которое сработает для обычной проперти
    PropertyAction _property_action;
    /// Действие которое сработает для ссылочной проперти
    ReferencePropertyAction _reference_property_action;
};

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