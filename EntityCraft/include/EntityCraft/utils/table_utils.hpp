#pragma once

#include "TypeConverterApi/helper/sfinae.hpp"
#include <ReflectionApi/helper/tuplealgoritm.hpp>

namespace entity_craft {
namespace helper {
namespace impl {

template<typename ClassType, typename Property, typename TargetType,
    std::enable_if_t<type_converter_api::sfinae::has_push_back_v<Property, TargetType> || type_converter_api::sfinae::has_insert_v<Property, TargetType> || type_converter_api::sfinae::has_emplace_back_v<Property, TargetType>, bool> = true>
constexpr void append_value_impl(ClassType&& obj, Property&& property, TargetType&& item)
{
    property.append_value(std::forward<ClassType>(obj), std::forward<TargetType>(item));
}

// Fallback overload for non-matching types: does nothing.
template<typename ClassType, typename Property, typename TargetType,
    std::enable_if_t<
        !(type_converter_api::sfinae::has_push_back_v<Property, TargetType> || type_converter_api::sfinae::has_insert_v<Property, TargetType> || type_converter_api::sfinae::has_emplace_back_v<Property, TargetType>),
        bool> = true>
constexpr void append_value_impl(ClassType&&, Property&&, TargetType&&)
{
    // Intentionally does nothing for non-matching types
}
} // namespace impl

template<typename ClassType, typename Tuple, typename TargetType, typename Predicate>
constexpr void append_value(Tuple&& tuple, ClassType&& obj, TargetType&& item, Predicate&& predicate)
{
    reflection_api::helper::for_each(std::forward<Tuple>(tuple), [&predicate, &item, &obj](auto&& value) {
        if(predicate(std::forward<decltype(value)>(value))) {
            impl::append_value_impl(std::forward<ClassType>(obj), std::forward<decltype(value)>(value), std::forward<TargetType>(item));
        }
    });
}

} // namespace helper
} // namespace entity_craft