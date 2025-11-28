#pragma once

#include "EntityCraft/column.h"
#include "EntityCraft/referencecolumn.h"

#include <utility>

namespace entity_craft {
namespace visitor {

template<typename ColumnAction>
class column_visitor
{
public:
    explicit column_visitor(ColumnAction property_action)
        : _column_action(std::move(property_action))
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
    void operator()(reference_column<ClassType, PropertyType, Setter, Getter, ReferenceColumns...>& /*reference_column*/)
    {
    }

private:
    ColumnAction _column_action;
};

template<typename ColumnAction>
auto make_column_visitor(
    ColumnAction&& column_action)
{
    return column_visitor<ColumnAction>(
        std::forward<ColumnAction>(column_action));
}

} // namespace visitor
} // namespace entity_craft