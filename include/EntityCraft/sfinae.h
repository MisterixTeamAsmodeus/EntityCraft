#pragma once

#include <type_traits>

namespace entity_craft {
namespace sfinae {

/// Структура для проверки наличия оператора isNull
template<typename T, typename = void>
struct has_is_null : std::false_type
{
};

/// Структура для проверки наличия оператора isNull
template<typename T>
struct has_is_null<T, std::void_t<decltype(std::declval<T>().isNull())>>
    : std::true_type
{
};

/// Результат проверки наличия оператора isNull
template<typename T>
constexpr bool has_is_null_v = has_is_null<T>::value;
} // namespace sfinae
} // namespace entity_craft