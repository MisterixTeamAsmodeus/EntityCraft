#pragma once

#include "column.h"
#include "converter/relationinserter.h"
#include "relationtype.h"
#include "table.h"

namespace EntityCraft {
template<typename ClassType,
    typename PropertyType,
    typename Setter,
    typename Getter,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
class ReferenceColumn : public Column<ClassType, PropertyType, Setter, Getter>
{
public:
    static PropertyType empty_property()
    {
        return PropertyType();
    }

public:
    ReferenceColumn(QueryCraft::ColumnInfo column_info,
        ReflectionApi::Property<ClassType, PropertyType, Setter, Getter> reflection_property,
        Table<ReferencePropertyType, ReferenceColumns...> reference_table,
        const RelationType type = RelationType::ONE_TO_ONE)
        : Column<ClassType, PropertyType, Setter, Getter>(std::move(column_info), std::move(reflection_property))
        , _reference_table(std::move(reference_table))
        , _type(type)
    {
    }

    auto reference_table() const
    {
        return _reference_table;
    }

    RelationType type() const
    {
        return _type;
    }

    auto inserter() const
    {
        return _inserter;
    }

    auto set_inserter(const RelationInserter<PropertyType, ReferencePropertyType>& inserter)
    {
        _inserter = inserter;
        return *this;
    }

private:
    Table<ReferencePropertyType, ReferenceColumns...> _reference_table;
    RelationType _type;

    RelationInserter<PropertyType, ReferencePropertyType> _inserter;
};

template<typename ClassType,
    typename PropertyType,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    ReflectionApi::Helper::Variable_t<ClassType, PropertyType> variable,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const QueryCraft::ColumnSettings settings = QueryCraft::ColumnSettings::NONE,
    const RelationType type = RelationType::ONE_TO_ONE)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        ReflectionApi::Helper::Setter_t<ClassType, PropertyType>,
        ReflectionApi::Helper::ConstGetter_t<ClassType, PropertyType>,
        ReferencePropertyType,
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

template<typename ClassType,
    typename PropertyType,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    ReflectionApi::Helper::Variable_t<ClassType, PropertyType> variable,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const RelationType type)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        ReflectionApi::Helper::Setter_t<ClassType, PropertyType>,
        ReflectionApi::Helper::ConstGetter_t<ClassType, PropertyType>,
        ReferencePropertyType,
        ReferenceColumns...>(
        QueryCraft::ColumnInfo(
            column_name,
            QueryCraft::ColumnSettings::NONE),
        ReflectionApi::make_property(
            column_name,
            variable),
        std::move(reference_table),
        type);
}

template<typename ClassType,
    typename PropertyType,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    ReflectionApi::Helper::Setter_t<ClassType, PropertyType> setter,
    ReflectionApi::Helper::ConstGetter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const QueryCraft::ColumnSettings settings = QueryCraft::ColumnSettings::NONE,
    const RelationType type = RelationType::ONE_TO_ONE)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        settings,
        ReflectionApi::make_property(
            column_name,
            setter,
            getter),
        std::move(reference_table),
        type);
}

template<typename ClassType,
    typename PropertyType,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    ReflectionApi::Helper::Setter_t<ClassType, PropertyType> setter,
    ReflectionApi::Helper::ConstGetter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const RelationType type)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        QueryCraft::ColumnInfo(
            column_name,
            QueryCraft::ColumnSettings::NONE),
        ReflectionApi::make_property(
            column_name,
            setter,
            getter),
        std::move(reference_table),
        type);
}

template<typename ClassType,
    typename PropertyType,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    ReflectionApi::Helper::Setter_t<ClassType, PropertyType> setter,
    ReflectionApi::Helper::MutableGetter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const QueryCraft::ColumnSettings settings = QueryCraft::ColumnSettings::NONE,
    const RelationType type = RelationType::ONE_TO_ONE)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        settings,
        ReflectionApi::make_property(
            column_name,
            setter,
            getter),
        std::move(reference_table),
        type);
}

template<typename ClassType,
    typename PropertyType,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    ReflectionApi::Helper::Setter_t<ClassType, PropertyType> setter,
    ReflectionApi::Helper::ConstGetter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const RelationType type)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        QueryCraft::ColumnInfo(
            column_name,
            QueryCraft::ColumnSettings::NONE),
        ReflectionApi::make_property(
            column_name,
            setter,
            getter),
        std::move(reference_table),
        type);
}

template<typename ClassType,
    typename PropertyType,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    ReflectionApi::Helper::Setter_t<ClassType, PropertyType> setter,
    ReflectionApi::Helper::Getter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const QueryCraft::ColumnSettings settings = QueryCraft::ColumnSettings::NONE,
    const RelationType type = RelationType::ONE_TO_ONE)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        settings,
        ReflectionApi::make_property(
            column_name,
            setter,
            getter),
        std::move(reference_table),
        type);
}

template<typename ClassType,
    typename PropertyType,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    ReflectionApi::Helper::Setter_t<ClassType, PropertyType> setter,
    ReflectionApi::Helper::Getter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const RelationType type)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        QueryCraft::ColumnInfo(
            column_name,
            QueryCraft::ColumnSettings::NONE),
        ReflectionApi::make_property(
            column_name,
            setter,
            getter),
        std::move(reference_table),
        type);
}

template<typename ClassType,
    typename PropertyType,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    ReflectionApi::Helper::BaseSetter_t<ClassType, PropertyType> setter,
    ReflectionApi::Helper::ConstGetter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const QueryCraft::ColumnSettings settings = QueryCraft::ColumnSettings::NONE,
    const RelationType type = RelationType::ONE_TO_ONE)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        settings,
        ReflectionApi::make_property(
            column_name,
            setter,
            getter),
        std::move(reference_table),
        type);
}

template<typename ClassType,
    typename PropertyType,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    ReflectionApi::Helper::BaseSetter_t<ClassType, PropertyType> setter,
    ReflectionApi::Helper::ConstGetter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const RelationType type)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        QueryCraft::ColumnInfo(
            column_name,
            QueryCraft::ColumnSettings::NONE),
        ReflectionApi::make_property(
            column_name,
            setter,
            getter),
        std::move(reference_table),
        type);
}

template<typename ClassType,
    typename PropertyType,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    ReflectionApi::Helper::BaseSetter_t<ClassType, PropertyType> setter,
    ReflectionApi::Helper::MutableGetter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const QueryCraft::ColumnSettings settings = QueryCraft::ColumnSettings::NONE,
    const RelationType type = RelationType::ONE_TO_ONE)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        settings,
        ReflectionApi::make_property(
            column_name,
            setter,
            getter),
        std::move(reference_table),
        type);
}

template<typename ClassType,
    typename PropertyType,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    ReflectionApi::Helper::BaseSetter_t<ClassType, PropertyType> setter,
    ReflectionApi::Helper::ConstGetter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const RelationType type)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        QueryCraft::ColumnInfo(
            column_name,
            QueryCraft::ColumnSettings::NONE),
        ReflectionApi::make_property(
            column_name,
            setter,
            getter),
        std::move(reference_table),
        type);
}

template<typename ClassType,
    typename PropertyType,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    ReflectionApi::Helper::BaseSetter_t<ClassType, PropertyType> setter,
    ReflectionApi::Helper::Getter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const QueryCraft::ColumnSettings settings = QueryCraft::ColumnSettings::NONE,
    const RelationType type = RelationType::ONE_TO_ONE)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        settings,
        ReflectionApi::make_property(
            column_name,
            setter,
            getter),
        std::move(reference_table),
        type);
}

template<typename ClassType,
    typename PropertyType,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    ReflectionApi::Helper::BaseSetter_t<ClassType, PropertyType> setter,
    ReflectionApi::Helper::Getter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const RelationType type)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        QueryCraft::ColumnInfo(
            column_name,
            QueryCraft::ColumnSettings::NONE),
        ReflectionApi::make_property(
            column_name,
            setter,
            getter),
        std::move(reference_table),
        type);
}

} // namespace EntityCraft