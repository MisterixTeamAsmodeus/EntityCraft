#pragma once

#include "column.h"
#include "relationtype.h"
#include "table.h"

namespace entity_craft {
template<typename ClassType,
    typename PropertyType,
    typename Setter,
    typename Getter,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
class reference_column : public column<ClassType, PropertyType, Setter, Getter>
{
public:
    static PropertyType empty_property()
    {
        return PropertyType();
    }

public:
    reference_column(query_craft::column_info column_info,
        reflection_api::property<ClassType, PropertyType, Setter, Getter> reflection_property,
        table<ReferencePropertyType, ReferenceColumns...> reference_table,
        const relation_type type = relation_type::one_to_one)
        : column<ClassType, PropertyType, Setter, Getter>(std::move(column_info), std::move(reflection_property))
        , _reference_table(std::move(reference_table))
        , _type(type)
    {
    }

    auto reference_table() const
    {
        return _reference_table;
    }

    relation_type type() const
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
    table<ReferencePropertyType, ReferenceColumns...> _reference_table;
    relation_type _type;

    type_converter_api::container_converter<PropertyType, ReferencePropertyType> _inserter;
};

template<typename ClassType,
    typename PropertyType,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    reflection_api::helper::Variable_t<ClassType, PropertyType> variable,
    table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const query_craft::column_settings settings = query_craft::column_settings::none,
    const relation_type type = relation_type::one_to_one)
{
    return reference_column<ClassType,
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
    table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const relation_type type)
{
    return reference_column<ClassType,
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
    table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const query_craft::column_settings settings = query_craft::column_settings::none,
    const relation_type type = relation_type::one_to_one)
{
    return reference_column<ClassType,
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
    table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const relation_type type)
{
    return reference_column<ClassType,
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
    table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const query_craft::column_settings settings = query_craft::column_settings::none,
    const relation_type type = relation_type::one_to_one)
{
    return reference_column<ClassType,
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
    table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const relation_type type)
{
    return reference_column<ClassType,
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
    table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const query_craft::column_settings settings = query_craft::column_settings::none,
    const relation_type type = relation_type::one_to_one)
{
    return reference_column<ClassType,
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
    table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const relation_type type)
{
    return reference_column<ClassType,
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
    table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const query_craft::column_settings settings = query_craft::column_settings::none,
    const relation_type type = relation_type::one_to_one)
{
    return reference_column<ClassType,
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
    table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const relation_type type)
{
    return reference_column<ClassType,
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
    table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const query_craft::column_settings settings = query_craft::column_settings::none,
    const relation_type type = relation_type::one_to_one)
{
    return reference_column<ClassType,
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
    table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const relation_type type)
{
    return reference_column<ClassType,
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
    table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const query_craft::column_settings settings = query_craft::column_settings::none,
    const relation_type type = relation_type::one_to_one)
{
    return reference_column<ClassType,
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
    table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const relation_type type)
{
    return reference_column<ClassType,
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

} // namespace entity_craft