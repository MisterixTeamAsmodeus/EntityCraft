#pragma once

#include "entitycraft_global.h"

#include <ReflectionApi/helper/templates.h>
#include <ReflectionApi/property.h>

#include <utility>

namespace EntityCraft {

template<typename ClassType,
    typename PropertyType,
    typename Setter = ReflectionApi::Helper::Setter_t<ClassType, PropertyType>,
    typename Getter = ReflectionApi::Helper::ConstGetter_t<ClassType, PropertyType>>
class ENTITYCRAFT_EXPORT Column
{
public:
    Column(std::string column_name, const ReflectionApi::Property<ClassType, PropertyType, Setter, Getter>& reflection_property)
        : _column_name(std::move(column_name))
        , _reflection_property(reflection_property)
    {
    }

    ~Column() = default;

    Column(const Column& other)
        : _column_name(other._column_name)
        , _reflection_property(other._reflection_property)
    {
    }

    Column(Column&& other) noexcept
        : _column_name(std::move(other._column_name))
        , _reflection_property(std::move(other._reflection_property))
    {
    }

    Column& operator=(const Column& other)
    {
        if(this == &other)
            return *this;
        _column_name = other._column_name;
        _reflection_property = other._reflection_property;
        return *this;
    }

    Column& operator=(Column&& other) noexcept
    {
        if(this == &other)
            return *this;
        _column_name = std::move(other._column_name);
        _reflection_property = std::move(other._reflection_property);
        return *this;
    }

    std::string column_name() const
    {
        return _column_name;
    }

    ReflectionApi::Property<ClassType, PropertyType, Setter, Getter> reflection_property() const
    {
        return _reflection_property;
    }

private:
    std::string _column_name;
    ReflectionApi::Property<ClassType, PropertyType, Setter, Getter> _reflection_property;
};

template<typename ClassType, typename PropertyType>
auto ENTITYCRAFT_EXPORT make_column(
    std::string column_name,
    ReflectionApi::Helper::Variable_t<ClassType, PropertyType> variable)
{
    return Column<ClassType, PropertyType>(
        std::move(column_name),
        ReflectionApi::make_property(
            column_name,
            variable));
}

template<typename ClassType,
    typename PropertyType,
    typename Setter,
    typename Getter>
auto ENTITYCRAFT_EXPORT make_column(
    std::string column_name,
    Setter&& setter,
    Getter&& getter)
{
    return Column<ClassType, PropertyType>(
        std::move(column_name),
        ReflectionApi::make_property(
            column_name,
            std::forward<Setter>(setter),
            std::forward<Getter>(getter)));
}

} // namespace EntityCraft
