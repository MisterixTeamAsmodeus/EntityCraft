#pragma once

#include "column.h"
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
    ReferenceColumn(query_craft::column_info column_info,
        reflection_api::property<ClassType, PropertyType, Setter, Getter> reflection_property,
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

    auto set_inserter(const type_converter_api::container_converter<PropertyType, ReferencePropertyType>& inserter)
    {
        _inserter = inserter;
        return *this;
    }

private:
    Table<ReferencePropertyType, ReferenceColumns...> _reference_table;
    RelationType _type;

    type_converter_api::container_converter<PropertyType, ReferencePropertyType> _inserter;
};

template<typename ClassType,
    typename PropertyType,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    reflection_api::helper::Variable_t<ClassType, PropertyType> variable,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const query_craft::column_settings settings = query_craft::column_settings::none,
    const RelationType type = RelationType::ONE_TO_ONE)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        reflection_api::helper::Setter_t<ClassType, PropertyType>,
        reflection_api::helper::ConstGetter_t<ClassType, PropertyType>,
        ReferencePropertyType,
        ReferenceColumns...>(
        query_craft::column_info(
            column_name,
            settings),
        reflection_api::make_property(
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
    reflection_api::helper::Variable_t<ClassType, PropertyType> variable,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const RelationType type)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        reflection_api::helper::Setter_t<ClassType, PropertyType>,
        reflection_api::helper::ConstGetter_t<ClassType, PropertyType>,
        ReferencePropertyType,
        ReferenceColumns...>(
        query_craft::column_info(
            column_name,
            query_craft::column_settings::none),
        reflection_api::make_property(
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
    reflection_api::helper::Setter_t<ClassType, PropertyType> setter,
    reflection_api::helper::ConstGetter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const query_craft::column_settings settings = query_craft::column_settings::none,
    const RelationType type = RelationType::ONE_TO_ONE)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        settings,
        reflection_api::make_property(
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
    reflection_api::helper::Setter_t<ClassType, PropertyType> setter,
    reflection_api::helper::ConstGetter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const RelationType type)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        query_craft::column_info(
            column_name,
            query_craft::column_settings::none),
        reflection_api::make_property(
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
    reflection_api::helper::Setter_t<ClassType, PropertyType> setter,
    reflection_api::helper::MutableGetter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const query_craft::column_settings settings = query_craft::column_settings::none,
    const RelationType type = RelationType::ONE_TO_ONE)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        settings,
        reflection_api::make_property(
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
    reflection_api::helper::Setter_t<ClassType, PropertyType> setter,
    reflection_api::helper::MutableGetter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const RelationType type)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        query_craft::column_info(
            column_name,
            query_craft::column_settings::none),
        reflection_api::make_property(
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
    reflection_api::helper::Setter_t<ClassType, PropertyType> setter,
    reflection_api::helper::Getter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const query_craft::column_settings settings = query_craft::column_settings::none,
    const RelationType type = RelationType::ONE_TO_ONE)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        settings,
        reflection_api::make_property(
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
    reflection_api::helper::Setter_t<ClassType, PropertyType> setter,
    reflection_api::helper::Getter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const RelationType type)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        query_craft::column_info(
            column_name,
            query_craft::column_settings::none),
        reflection_api::make_property(
            column_name,
            setter,
            getter),
        std::move(reference_table),
        type);
}

//-----------------------------Перегрузки для примитивов, где параметры в setter не по const &-----------------------------------

template<typename ClassType,
    typename PropertyType,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    reflection_api::helper::BaseSetter_t<ClassType, PropertyType> setter,
    reflection_api::helper::ConstGetter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const query_craft::column_settings settings = query_craft::column_settings::none,
    const RelationType type = RelationType::ONE_TO_ONE)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        settings,
        reflection_api::make_property(
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
    reflection_api::helper::BaseSetter_t<ClassType, PropertyType> setter,
    reflection_api::helper::ConstGetter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const RelationType type)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        query_craft::column_info(
            column_name,
            query_craft::column_settings::none),
        reflection_api::make_property(
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
    reflection_api::helper::BaseSetter_t<ClassType, PropertyType> setter,
    reflection_api::helper::MutableGetter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const query_craft::column_settings settings = query_craft::column_settings::none,
    const RelationType type = RelationType::ONE_TO_ONE)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        settings,
        reflection_api::make_property(
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
    reflection_api::helper::BaseSetter_t<ClassType, PropertyType> setter,
    reflection_api::helper::MutableGetter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const RelationType type)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        query_craft::column_info(
            column_name,
            query_craft::column_settings::none),
        reflection_api::make_property(
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
    reflection_api::helper::BaseSetter_t<ClassType, PropertyType> setter,
    reflection_api::helper::Getter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const query_craft::column_settings settings = query_craft::column_settings::none,
    const RelationType type = RelationType::ONE_TO_ONE)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        settings,
        reflection_api::make_property(
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
    reflection_api::helper::BaseSetter_t<ClassType, PropertyType> setter,
    reflection_api::helper::Getter_t<ClassType, PropertyType> getter,
    Table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const RelationType type)
{
    return ReferenceColumn<ClassType,
        PropertyType,
        decltype(setter),
        decltype(getter),
        ReferencePropertyType,
        ReferenceColumns...>(
        query_craft::column_info(
            column_name,
            query_craft::column_settings::none),
        reflection_api::make_property(
            column_name,
            setter,
            getter),
        std::move(reference_table),
        type);
}

} // namespace EntityCraft