#pragma once

#include "column.h"
#include "relationtype.h"
#include "table.h"

#include <ReflectionApi/helper/templates.hpp>
#include <TypeConverterApi/containerconverter.hpp>

namespace entity_craft {
template<typename ClassType,
    typename PropertyType,
    typename Setter,
    typename Getter,
    typename ReferencePropertyType,
    typename... ReferenceColumns>
class reference_column final : public column<ClassType, PropertyType, Setter, Getter>
{
public:
    static PropertyType empty_property();

public:
    explicit reference_column(const std::string& name,
        reflection_api::helper::Variable_t<ClassType, PropertyType> variable,
        relation_type type,
        column_settings settings = column_settings::empty) noexcept;

    explicit reference_column(const std::string& name,
        Setter setter,
        Getter getter,
        relation_type type,
        column_settings settings = column_settings::empty) noexcept;

    reference_column(const reference_column& other) = default;
    reference_column(reference_column&& other) noexcept = default;
    reference_column& operator=(const reference_column& other) = default;
    reference_column& operator=(reference_column&& other) noexcept = default;

    table<ReferencePropertyType, ReferenceColumns...> reference_table() const;

    relation_type type() const;

    type_converter_api::container_converter<PropertyType, ReferencePropertyType> inserter() const;

    reference_column set_inserter(const type_converter_api::container_converter<PropertyType, ReferencePropertyType>& inserter);

private:
    table<ReferencePropertyType, ReferenceColumns...> _reference_table;
    relation_type _type;

    type_converter_api::container_converter<PropertyType, ReferencePropertyType> _inserter;
};

template<typename ClassType, typename PropertyType, typename Setter, typename Getter, typename ReferencePropertyType, typename... ReferenceColumns> PropertyType reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>::empty_property()
{
    return PropertyType();
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter, typename ReferencePropertyType, typename... ReferenceColumns> reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>::reference_column(const std::string& name, const reflection_api::helper::Variable_t<ClassType, PropertyType> variable, const relation_type type, const column_settings settings) noexcept
    : column<ClassType, PropertyType, Setter, Getter>(name, variable, settings)
    , _type(type)
{
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter, typename ReferencePropertyType, typename... ReferenceColumns> reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>::reference_column(const std::string& name, Setter setter, Getter getter, const relation_type type, const column_settings settings) noexcept
    : column<ClassType, PropertyType, Setter, Getter>(name, setter, getter, settings)
    , _type(type)
{
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter, typename ReferencePropertyType, typename... ReferenceColumns> table<ReferencePropertyType, ReferenceColumns...> reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>::reference_table() const
{
    return _reference_table;
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter, typename ReferencePropertyType, typename... ReferenceColumns> relation_type reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>::type() const
{
    return _type;
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter, typename ReferencePropertyType, typename... ReferenceColumns> type_converter_api::container_converter<PropertyType, ReferencePropertyType> reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>::inserter() const
{
    return _inserter;
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter, typename ReferencePropertyType, typename... ReferenceColumns> reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...> reference_column<ClassType, PropertyType, Setter, Getter, ReferencePropertyType, ReferenceColumns...>::set_inserter(const type_converter_api::container_converter<PropertyType, ReferencePropertyType>& inserter)
{
    _inserter = inserter;
    return *this;
}

} // namespace entity_craft