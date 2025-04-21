#pragma once

#include "EntityCraft/column.h"
#include "EntityCraft/reference_column.h"

#include <utility>

namespace EntityCraft {
namespace Visitor {

template<typename ReferenceColumnAction>
class ReferenceColumnVisitor
{
public:
    explicit ReferenceColumnVisitor(ReferenceColumnAction reference_property_action)
        : _reference_column_action(std::move(reference_property_action))
    {
    }

    template<typename ClassType,
        typename PropertyType,
        typename Setter,
        typename Getter>
    void operator()(Column<ClassType, PropertyType, Setter, Getter>& /*column*/)
    {
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
    ReferenceColumnAction _reference_column_action;
};

template<typename ReferenceColumnAction>
auto make_reference_column_visitor(ReferenceColumnAction&& reference_column_action)
{
    return ReferenceColumnVisitor<ReferenceColumnAction>(
        std::forward<ReferenceColumnAction>(reference_column_action));
}

} // namespace Visitor
} // namespace EntityCraft