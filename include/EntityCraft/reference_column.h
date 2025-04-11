#pragma once

#include "column.h"
#include "relationtype.h"
#include "table.h"

namespace EntityCraft {
template<typename ClassType,
    typename PropertyType,
    typename Setter,
    typename Getter,
    typename... ReferenceColumns>
class ReferenceColumn : public Column<ClassType, PropertyType, Setter, Getter>
{
public:
    ReferenceColumn(QueryCraft::ColumnInfo column_info,
        ReflectionApi::Property<ClassType, PropertyType, Setter, Getter> reflection_property,
        Table<PropertyType, ReferenceColumns...> reference_table,
        const RelationType type = RelationType::ONE_TO_ONE)
        : Column<ClassType, PropertyType, Setter, Getter>(std::move(column_info), std::move(reflection_property))
        , _reference_table(std::move(reference_table))
        , _type(type)
    {
    }

    Table<PropertyType, ReferenceColumns...> reference_table() const
    {
        return _reference_table;
    }

    RelationType type() const
    {
        return _type;
    }

private:
    Table<PropertyType, ReferenceColumns...> _reference_table;
    RelationType _type;
};

template<typename ClassType,
    typename PropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    ReflectionApi::Helper::Variable_t<ClassType, PropertyType> variable,
    Table<PropertyType, ReferenceColumns...> reference_table,
    const QueryCraft::ColumnSettings settings = QueryCraft::ColumnSettings::NONE,
    const RelationType type = RelationType::ONE_TO_ONE)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        ReflectionApi::Helper::Setter_t<ClassType, PropertyType>,
        ReflectionApi::Helper::ConstGetter_t<ClassType, PropertyType>,
        ReferenceColumns...>(
        QueryCraft::ColumnInfo(
            column_name,
            settings),
        ReflectionApi::make_property(
            column_name,
            variable),
        std::move(reference_table),
        type);
}

} // namespace EntityCraft