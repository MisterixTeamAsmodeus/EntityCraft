#pragma once

#include "tuplealgoritm.hpp"

namespace reflection_api {
namespace helper {
namespace impl {
template<typename ClassType, typename Property, typename TargetType,
    std::enable_if_t<
        std::is_same<typename std::decay_t<Property>::class_type, std::decay_t<ClassType>>::value && std::is_same<typename std::decay_t<Property>::property_type, std::decay_t<TargetType>>::value,
        int>
    = 0>
constexpr void get_value_impl(ClassType&& obj, Property&& property, TargetType& target)
{
    target = property.value(std::forward<ClassType>(obj));
}

// Fallback overload for non-matching types: does nothing.
template<typename ClassType, typename Property, typename TargetType,
    std::enable_if_t<
        !(std::is_same<typename std::decay_t<Property>::class_type, std::decay_t<ClassType>>::value && std::is_same<typename std::decay_t<Property>::property_type, std::decay_t<TargetType>>::value),
        int>
    = 0>
constexpr void get_value_impl(ClassType&&, Property&&, TargetType&)
{
    // Intentionally does nothing for non-matching types
}

template<typename ClassType, typename Property, typename TargetType,
    std::enable_if_t<
        std::is_same<typename std::decay_t<Property>::class_type, std::decay_t<ClassType>>::value && std::is_same<typename std::decay_t<Property>::property_type, std::decay_t<TargetType>>::value,
        int>
    = 0>
constexpr void set_value_impl(ClassType&& obj, Property&& property, TargetType&& target)
{
    property.set_value(std::forward<ClassType>(obj), std::forward<TargetType>(target));
}

// Fallback overload for non-matching types: does nothing.
template<typename ClassType, typename Property, typename TargetType,
    std::enable_if_t<
        !(std::is_same<typename std::decay_t<Property>::class_type, std::decay_t<ClassType>>::value && std::is_same<typename std::decay_t<Property>::property_type, std::decay_t<TargetType>>::value),
        int>
    = 0>
constexpr void set_value_impl(ClassType&&, Property&&, TargetType&&)
{
    // Intentionally does nothing for non-matching types
}
} // namespace impl

template<typename ClassType, typename Tuple, typename TargetType, typename Predicate>
constexpr void get_value(Tuple&& tuple, ClassType&& obj, TargetType& targetValue, Predicate&& predicate)
{
    for_each(std::forward<Tuple>(tuple), [&predicate, &targetValue, &obj](auto&& value) {
        if(predicate(std::forward<decltype(value)>(value))) {
            impl::get_value_impl(std::forward<ClassType>(obj), std::forward<decltype(value)>(value), targetValue);
        }
    });
}

template<typename ClassType, typename Tuple, typename TargetType, typename Predicate>
constexpr void set_value(Tuple&& tuple, ClassType&& obj, TargetType&& targetValue, Predicate&& predicate)
{
    for_each(std::forward<Tuple>(tuple), [&predicate, &targetValue, &obj](auto&& value) {
        if(predicate(std::forward<decltype(value)>(value))) {
            impl::set_value_impl(std::forward<ClassType>(obj), std::forward<decltype(value)>(value), targetValue);
        }
    });
}
} // namespace helper
} // namespace reflection_api