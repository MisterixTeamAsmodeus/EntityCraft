#pragma once

#include "EntityCraft/column.h"
#include "EntityCraft/referencecolumn.h"

#include <utility>

namespace entity_craft {
namespace visitor {

template<typename ColumnAction, typename ReferenceColumnAction>
class any_column_visitor
{
public:
    any_column_visitor(ColumnAction property_action, ReferenceColumnAction reference_property_action)
        : _column_action(std::move(property_action))
        , _reference_column_action(std::move(reference_property_action))
    {
    }

    template<typename ClassType,
        typename PropertyType,
        typename Setter,
        typename Getter>
    void operator()(column<ClassType, PropertyType, Setter, Getter>& column)
    {
        _column_action(column);
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
    ColumnAction _column_action;
    ReferenceColumnAction _reference_column_action;
};

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
} // namespace entity_craft