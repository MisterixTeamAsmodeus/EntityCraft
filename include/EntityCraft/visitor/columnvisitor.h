#pragma once

#include "EntityCraft/column.h"
#include "EntityCraft/reference_column.h"

#include <utility>

namespace EntityCraft {
namespace Visitor {

template<typename ColumnAction>
class ColumnVisitor
{
public:
    explicit ColumnVisitor(ColumnAction property_action)
        : _column_action(std::move(property_action))
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
    void operator()(ReferenceColumn<ClassType, PropertyType, Setter, Getter, ReferenceColumns...>& /*reference_column*/)
    {
    }

private:
    ColumnAction _column_action;
};

template<typename ColumnAction>
auto make_column_visitor(
    ColumnAction&& column_action)
{
    return ColumnVisitor<ColumnAction>(
        std::forward<ColumnAction>(column_action));
}

} // namespace Visitor
} // namespace EntityCraft