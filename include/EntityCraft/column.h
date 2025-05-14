#pragma once

#include "converter/nullcheker.h"
#include "QueryCraft/conditiongroup.h"

#include <ReflectionApi/helper/templates.h>
#include <ReflectionApi/property.h>

#include <utility>

namespace EntityCraft {

template<typename ClassType,
    typename PropertyType,
    typename Setter = reflection_api::helper::Setter_t<ClassType, PropertyType>,
    typename Getter = reflection_api::helper::ConstGetter_t<ClassType, PropertyType>>
class Column
{
public:
    Column(query_craft::column_info column_info, const reflection_api::property<ClassType, PropertyType, Setter, Getter>& reflection_property)
        : _column_info(std::move(column_info))
        , _reflection_property(reflection_property)
    {
    }

    ~Column() = default;

    Column(const Column& other)
        : _column_info(other._column_info)
        , _reflection_property(other._reflection_property)
    {
    }

    Column(Column&& other) noexcept
        : _column_info(std::move(other._column_info))
        , _reflection_property(std::move(other._reflection_property))
    {
    }

    Column& operator=(const Column& other)
    {
        if(this == &other)
            return *this;
        _column_info = other._column_info;
        _reflection_property = other._reflection_property;
        return *this;
    }

    Column& operator=(Column&& other) noexcept
    {
        if(this == &other)
            return *this;
        _column_info = std::move(other._column_info);
        _reflection_property = std::move(other._reflection_property);
        return *this;
    }

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
        return _reflection_property.converter();
    }

    auto set_converter(const std::shared_ptr<type_converter_api::type_converter<PropertyType>>& converter)
    {
        _reflection_property.set_converter(converter);
        return *this;
    }

    std::shared_ptr<NullCheker<PropertyType>> null_cheker() const
    {
        return _null_cheker;
    }

    auto set_null_cheker(const std::shared_ptr<NullCheker<PropertyType>>& null_cheker)
    {
        _null_cheker = null_cheker;
        return *this;
    }

private:
    query_craft::column_info _column_info;
    reflection_api::property<ClassType, PropertyType, Setter, Getter> _reflection_property;
    std::shared_ptr<NullCheker<PropertyType>> _null_cheker = std::make_shared<NullCheker<PropertyType>>();
};

template<typename ClassType, typename PropertyType>
auto make_column(
    std::string column_name,
    reflection_api::helper::Variable_t<ClassType, PropertyType> variable,
    const query_craft::column_settings settings = query_craft::column_settings::none)
{
    return Column<ClassType, PropertyType>(
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
    return Column<ClassType, PropertyType>(
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
    return Column<ClassType, PropertyType>(
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
    return Column<ClassType, PropertyType>(
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
    return Column<ClassType, PropertyType>(
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
    return Column<ClassType, PropertyType>(
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
    return Column<ClassType, PropertyType>(
        query_craft::column_info(
            column_name,
            settings),
        reflection_api::make_property(
            column_name,
            setter,
            getter));
}

} // namespace EntityCraft
