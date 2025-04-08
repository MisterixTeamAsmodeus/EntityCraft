#pragma once

#include "QueryCraft/conditiongroup.h"

#include <ReflectionApi/helper/templates.h>
#include <ReflectionApi/property.h>

#include <utility>

namespace EntityCraft {

/**
 * Макрос для автоматической генерации имени колонки
 * @param field Ссылка на колонку следующего вида - &Test::id -> id
 */
#define create_column_name(field) (strrchr(#field, ':') ? strrchr(#field, ':') + 1 : "Error"), field

template<typename ClassType,
    typename PropertyType,
    typename Setter = ReflectionApi::Helper::Setter_t<ClassType, PropertyType>,
    typename Getter = ReflectionApi::Helper::ConstGetter_t<ClassType, PropertyType>>
class Column
{
public:
    Column(QueryCraft::ColumnInfo column_info, const ReflectionApi::Property<ClassType, PropertyType, Setter, Getter>& reflection_property)
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

    ReflectionApi::Property<ClassType, PropertyType, Setter, Getter> property() const
    {
        return _reflection_property;
    }

    QueryCraft::ColumnInfo column_info() const
    {
        return _column_info;
    }

    QueryCraft::ColumnInfo& mutable_column_info()
    {
        return _column_info;
    }

    std::shared_ptr<ReflectionApi::Converter<PropertyType>> converter() const
    {
        return _reflection_property.converter();
    }

    void set_converter(const std::shared_ptr<ReflectionApi::Converter<PropertyType>>& converter)
    {
        _reflection_property.set_converter(converter);
    }

private:
    QueryCraft::ColumnInfo _column_info;
    ReflectionApi::Property<ClassType, PropertyType, Setter, Getter> _reflection_property;
};

template<typename ClassType, typename PropertyType>
auto make_column(
    std::string column_name,
    ReflectionApi::Helper::Variable_t<ClassType, PropertyType> variable,
    const QueryCraft::ColumnSettings settings = QueryCraft::ColumnSettings::NONE)
{
    return Column<ClassType, PropertyType>(
        QueryCraft::ColumnInfo(
            column_name,
            settings),
        ReflectionApi::make_property(
            column_name,
            variable));
}

template<typename ClassType, typename PropertyType>
auto make_column(
    std::string column_name,
    ReflectionApi::Helper::Setter_t<ClassType, PropertyType> setter,
    ReflectionApi::Helper::ConstGetter_t<ClassType, PropertyType> getter,
    const QueryCraft::ColumnSettings settings = QueryCraft::ColumnSettings::NONE)
{
    return Column<ClassType, PropertyType>(
        QueryCraft::ColumnInfo(
            column_name,
            settings),
        ReflectionApi::make_property(
            column_name,
            setter,
            getter));
}

template<typename ClassType, typename PropertyType>
auto make_column(
    std::string column_name,
    ReflectionApi::Helper::Setter_t<ClassType, PropertyType> setter,
    ReflectionApi::Helper::MutableGetter_t<ClassType, PropertyType> getter,
    const QueryCraft::ColumnSettings settings = QueryCraft::ColumnSettings::NONE)
{
    return Column<ClassType, PropertyType>(
        QueryCraft::ColumnInfo(
            column_name,
            settings),
        ReflectionApi::make_property(
            column_name,
            setter,
            getter));
}

template<typename ClassType, typename PropertyType>
auto make_column(
    std::string column_name,
    ReflectionApi::Helper::Setter_t<ClassType, PropertyType> setter,
    ReflectionApi::Helper::Getter_t<ClassType, PropertyType> getter,
    const QueryCraft::ColumnSettings settings = QueryCraft::ColumnSettings::NONE)
{
    return Column<ClassType, PropertyType>(
        QueryCraft::ColumnInfo(
            column_name,
            settings),
        ReflectionApi::make_property(
            column_name,
            setter,
            getter));
}

//-----------------------------Перегрузки для примитивов, где параметры в setter не по const &-----------------------------------

template<typename ClassType, typename PropertyType>
auto make_column(
    std::string column_name,
    ReflectionApi::Helper::BaseSetter_t<ClassType, PropertyType>&& setter,
    ReflectionApi::Helper::ConstGetter_t<ClassType, PropertyType>&& getter,
    const QueryCraft::ColumnSettings settings = QueryCraft::ColumnSettings::NONE)
{
    return Column<ClassType, PropertyType>(
        QueryCraft::ColumnInfo(
            column_name,
            settings),
        ReflectionApi::make_property(
            column_name,
            setter,
            getter));
}

template<typename ClassType, typename PropertyType>
auto make_column(
    std::string column_name,
    ReflectionApi::Helper::BaseSetter_t<ClassType, PropertyType> setter,
    ReflectionApi::Helper::MutableGetter_t<ClassType, PropertyType> getter,
    const QueryCraft::ColumnSettings settings = QueryCraft::ColumnSettings::NONE)
{
    return Column<ClassType, PropertyType>(
        QueryCraft::ColumnInfo(
            column_name,
            settings),
        ReflectionApi::make_property(
            column_name,
            setter,
            getter));
}

template<typename ClassType, typename PropertyType>
auto make_column(
    std::string column_name,
    ReflectionApi::Helper::BaseSetter_t<ClassType, PropertyType> setter,
    ReflectionApi::Helper::Getter_t<ClassType, PropertyType> getter,
    const QueryCraft::ColumnSettings settings = QueryCraft::ColumnSettings::NONE)
{
    return Column<ClassType, PropertyType>(
        QueryCraft::ColumnInfo(
            column_name,
            settings),
        ReflectionApi::make_property(
            column_name,
            setter,
            getter));
}

} // namespace EntityCraft
