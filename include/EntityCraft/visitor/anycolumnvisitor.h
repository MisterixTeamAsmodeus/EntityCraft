#pragma once

#include "EntityCraft/column.h"
#include "EntityCraft/reference_column.h"

#include <utility>

namespace EntityCraft {
namespace Visitor {

template<typename ColumnAction, typename ReferenceColumnAction>
class AnyColumnVisitor
{
public:
    AnyColumnVisitor(ColumnAction property_action, ReferenceColumnAction reference_property_action)
        : _column_action(std::move(property_action))
        , _reference_column_action(std::move(reference_property_action))
    {
    }

    template<typename ClassType,
        typename PropertyType,
        typename Setter,
        typename Getter>
    void operator()(Column<ClassType, PropertyType, Setter, Getter>& column)
    {
        _column_action(column);
    }

    template<typename ClassType,
        typename PropertyType,
        typename Setter,
        typename Getter,
        typename... ReferenceColumns>
    void operator()(ReferenceColumn<ClassType, PropertyType, Setter, Getter, ReferenceColumns...>& reference_column)
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
    return AnyColumnVisitor<ColumnAction, ReferenceColumnAction>(
        std::forward<ColumnAction>(column_action),
        std::forward<ReferenceColumnAction>(reference_column_action));
}

} // namespace Visitor
} // namespace EntityCraft