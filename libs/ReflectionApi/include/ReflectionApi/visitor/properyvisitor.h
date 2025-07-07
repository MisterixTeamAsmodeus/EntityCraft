#pragma once

#include "ReflectionApi/property.h"
#include "ReflectionApi/referenceproperty.h"

namespace reflection_api {
namespace visitor {

/**
 * Визитор для обычных проперти
 */
template<typename PropertyAction>
class property_visitor
{
public:
    explicit property_visitor(const PropertyAction& property_action)
        : _property_action(property_action)
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
    void operator()(reference_property<ClassType, PropertyType, ReferenceColumnType, Setter, Getter, ReferenceProperties...>& /*reference_property*/)
    {
    }

private:
    /// Действие которое сработает для обычной проперти
    PropertyAction _property_action;
};

template<typename PropertyAction>
auto make_property_visitor(PropertyAction&& property_action)
{
    return property_visitor<PropertyAction>(std::forward<PropertyAction>(property_action));
}

} // namespace visitor
} // namespace reflection_api