#pragma once

#include "nullcheker.h"
#include "QueryCraft/conditiongroup.h"

#include <ReflectionApi/helper/templates.h>
#include <ReflectionApi/property.h>

#include <utility>

namespace entity_craft {

template<typename ClassType,
    typename PropertyType,
    typename Setter = reflection_api::helper::Setter_t<ClassType, PropertyType>,
    typename Getter = reflection_api::helper::ConstGetter_t<ClassType, PropertyType>>
class column
{
public:
    column(query_craft::column_info column_info, const reflection_api::property<ClassType, PropertyType, Setter, Getter>& reflection_property)
        : _column_info(std::move(column_info))
        , _reflection_property(reflection_property)
    {
    }

    ~column() = default;

    column(const column& other) = default;
    column(column&& other) noexcept = default;
    column& operator=(const column& other) = default;
    column& operator=(column&& other) noexcept = default;

    reflection_api::property<ClassType, PropertyType, Setter, Getter> property() const
    {
        return _reflection_property;
    }

    query_craft::column_info column_info() const
    {
        return _column_info;
    }

    query_craft::column_info& mutable_column_info()
    {
        return _column_info;
    }

    std::shared_ptr<type_converter_api::type_converter<PropertyType>> converter() const
    {
        return _reflection_property.property_converter();
    }

    auto set_converter(const std::shared_ptr<type_converter_api::type_converter<PropertyType>>& converter)
    {
        _reflection_property.set_converter(converter);
        return *this;
    }

    std::shared_ptr<entity_craft::null_cheker<PropertyType>> null_cheker() const
    {
        return _null_cheker;
    }

    auto set_null_cheker(const std::shared_ptr<entity_craft::null_cheker<PropertyType>>& null_cheker)
    {
        _null_cheker = null_cheker;
        return *this;
    }

private:
    query_craft::column_info _column_info;
    reflection_api::property<ClassType, PropertyType, Setter, Getter> _reflection_property;
    std::shared_ptr<entity_craft::null_cheker<PropertyType>> _null_cheker = std::make_shared<entity_craft::null_cheker<PropertyType>>();
};

template<typename ClassType, typename PropertyType>
auto make_column(
    std::string column_name,
    reflection_api::helper::Variable_t<ClassType, PropertyType> variable,
    const query_craft::column_settings settings = query_craft::column_settings::none)
{
    return column<ClassType, PropertyType>(
        query_craft::column_info(
            column_name,
            settings),
        reflection_api::make_property(
            column_name,
            variable));
}

template<typename ClassType, typename PropertyType>
auto make_column(
    std::string column_name,
    reflection_api::helper::Setter_t<ClassType, PropertyType> setter,
    reflection_api::helper::ConstGetter_t<ClassType, PropertyType> getter,
    const query_craft::column_settings settings = query_craft::column_settings::none)
{
    return column<ClassType, PropertyType>(
        query_craft::column_info(
            column_name,
            settings),
        reflection_api::make_property(
            column_name,
            setter,
            getter));
}

template<typename ClassType, typename PropertyType>
auto make_column(
    std::string column_name,
    reflection_api::helper::Setter_t<ClassType, PropertyType> setter,
    reflection_api::helper::MutableGetter_t<ClassType, PropertyType> getter,
    const query_craft::column_settings settings = query_craft::column_settings::none)
{
    return column<ClassType, PropertyType>(
        query_craft::column_info(
            column_name,
            settings),
        reflection_api::make_property(
            column_name,
            setter,
            getter));
}

template<typename ClassType, typename PropertyType>
auto make_column(
    std::string column_name,
    reflection_api::helper::Setter_t<ClassType, PropertyType> setter,
    reflection_api::helper::Getter_t<ClassType, PropertyType> getter,
    const query_craft::column_settings settings = query_craft::column_settings::none)
{
    return column<ClassType, PropertyType>(
        query_craft::column_info(
            column_name,
            settings),
        reflection_api::make_property(
            column_name,
            setter,
            getter));
}

//-----------------------------Перегрузки для примитивов, где параметры в setter не по const &-----------------------------------

template<typename ClassType, typename PropertyType>
auto make_column(
    std::string column_name,
    reflection_api::helper::BaseSetter_t<ClassType, PropertyType>&& setter,
    reflection_api::helper::ConstGetter_t<ClassType, PropertyType>&& getter,
    const query_craft::column_settings settings = query_craft::column_settings::none)
{
    return column<ClassType, PropertyType>(
        query_craft::column_info(
            column_name,
            settings),
        reflection_api::make_property(
            column_name,
            setter,
            getter));
}

template<typename ClassType, typename PropertyType>
auto make_column(
    std::string column_name,
    reflection_api::helper::BaseSetter_t<ClassType, PropertyType> setter,
    reflection_api::helper::MutableGetter_t<ClassType, PropertyType> getter,
    const query_craft::column_settings settings = query_craft::column_settings::none)
{
    return column<ClassType, PropertyType>(
        query_craft::column_info(
            column_name,
            settings),
        reflection_api::make_property(
            column_name,
            setter,
            getter));
}

template<typename ClassType, typename PropertyType>
auto make_column(
    std::string column_name,
    reflection_api::helper::BaseSetter_t<ClassType, PropertyType> setter,
    reflection_api::helper::Getter_t<ClassType, PropertyType> getter,
    const query_craft::column_settings settings = query_craft::column_settings::none)
{
    return column<ClassType, PropertyType>(
        query_craft::column_info(
            column_name,
            settings),
        reflection_api::make_property(
            column_name,
            setter,
            getter));
}

} // namespace entity_craft
