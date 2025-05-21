#pragma once

#include "cascadetype.h"
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
        const relation_type type = relation_type::one_to_one,
        const cascade_type cascade = cascade_type::all)
        : column<ClassType, PropertyType, Setter, Getter>(std::move(column_info), std::move(reflection_property))
        , _reference_table(std::move(reference_table))
        , _type(type)
        , _cascade(cascade)
    {
    }

    reference_column(const reference_column& other) = default;
    reference_column(reference_column&& other) noexcept = default;
    reference_column& operator=(const reference_column& other) = default;
    reference_column& operator=(reference_column&& other) noexcept = default;

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

    bool has_cascade(const cascade_type cascade) const
    {
        auto t = (_cascade | cascade);
        return t == _cascade;
    }

    cascade_type cascade() const
    {
        return _cascade;
    }

private:
    table<ReferencePropertyType, ReferenceColumns...> _reference_table;
    relation_type _type;
    cascade_type _cascade;

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
    const relation_type type = relation_type::one_to_one,
    const cascade_type cascade = cascade_type::all)
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
        type,
        cascade);
}

template<typename ClassType,
    typename PropertyType,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
auto make_reference_column(
    std::string column_name,
    reflection_api::helper::Variable_t<ClassType, PropertyType> variable,
    table<ReferencePropertyType, ReferenceColumns...> reference_table,
    const relation_type type,
    const cascade_type cascade = cascade_type::all)
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
        type,
        cascade);
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
    const relation_type type = relation_type::one_to_one,
    const cascade_type cascade = cascade_type::all)
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
        type,
        cascade);
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
    const relation_type type,
    const cascade_type cascade = cascade_type::all)
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
        type,
        cascade);
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
    const relation_type type = relation_type::one_to_one,
    const cascade_type cascade = cascade_type::all)
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
        type,
        cascade);
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
    const relation_type type,
    const cascade_type cascade = cascade_type::all)
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
        type,
        cascade);
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
    const relation_type type = relation_type::one_to_one,
    const cascade_type cascade = cascade_type::all)
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
        type,
        cascade);
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
    const relation_type type,
    const cascade_type cascade = cascade_type::all)
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
        type,
        cascade);
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
    const relation_type type = relation_type::one_to_one,
    const cascade_type cascade = cascade_type::all)
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
        type,
        cascade);
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
    const relation_type type,
    const cascade_type cascade = cascade_type::all)
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
        type,
        cascade);
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
    const relation_type type = relation_type::one_to_one,
    const cascade_type cascade = cascade_type::all)
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
        type,
        cascade);
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
    const relation_type type,
    const cascade_type cascade = cascade_type::all)
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
        type,
        cascade);
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
    const relation_type type = relation_type::one_to_one,
    const cascade_type cascade = cascade_type::all)
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
        type,
        cascade);
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
    const relation_type type,
    const cascade_type cascade = cascade_type::all)
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
        type,
        cascade);
}

} // namespace entity_craft