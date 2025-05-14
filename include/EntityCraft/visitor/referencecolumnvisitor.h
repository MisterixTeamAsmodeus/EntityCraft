#pragma once

#include "EntityCraft/column.h"
#include "EntityCraft/referencecolumn.h"

#include <utility>

namespace entity_craft {
namespace visitor {

template<typename ReferenceColumnAction>
class reference_column_visitor
{
public:
    explicit reference_column_visitor(ReferenceColumnAction reference_property_action)
        : _reference_column_action(std::move(reference_property_action))
    {
    }

    template<typename ClassType,
        typename PropertyType,
        typename Setter,
        typename Getter>
    void operator()(column<ClassType, PropertyType, Setter, Getter>& /*column*/)
    {
    }

    template<typename ClassType,
        typename PropertyType,
        typename Setter,
        typename Getter,
        typename... ReferenceColumns>
    void operator()(reference_column<ClassType, PropertyType, Setter, Getter, ReferenceColumns...>& reference_column)
    {
        _reference_column_action(reference_column);
    }

private:
    ReferenceColumnAction _reference_column_action;
};

template<typename ReferenceColumnAction>
auto make_reference_column_visitor(ReferenceColumnAction&& reference_column_action)
{
    return reference_column_visitor<ReferenceColumnAction>(
        std::forward<ReferenceColumnAction>(reference_column_action));
}

} // namespace visitor
} // namespace entity_craft