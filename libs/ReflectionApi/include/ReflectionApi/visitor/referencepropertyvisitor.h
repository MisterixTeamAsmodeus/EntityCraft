#pragma once

#include "ReflectionApi/referenceproperty.h"

namespace reflection_api {
namespace visitor {

/**
 * Визитор для ссылочных типов проперти
 */
template<typename ReferencePropertyAction>
class reference_property_visitor
{
public:
    explicit reference_property_visitor(const ReferencePropertyAction& reference_property_action)
        : _reference_property_action(reference_property_action)
    {
    }

    template<typename ClassType,
        typename PropertyType,
        typename Setter,
        typename Getter>
    void operator()(property<ClassType, PropertyType, Setter, Getter>& /*property*/)
    {
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
    /// Действие которое сработает для ссылочной проперти
    ReferencePropertyAction _reference_property_action;
};

template<typename ReferencePropertyAction>
auto make_reference_property_visitor(ReferencePropertyAction&& reference_property_action)
{
    return reference_property_visitor<ReferencePropertyAction>(std::forward<ReferencePropertyAction>(reference_property_action));
}

} // namespace visitor
} // namespace reflection_api