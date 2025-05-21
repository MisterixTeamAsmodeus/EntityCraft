#pragma once

#include "TypeConverterApi/void_t.h"

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
struct has_is_null<T, type_converter_api::sfinae::void_t<decltype(std::declval<T>().isNull())>>
    : std::true_type
{
};

/// Результат проверки наличия оператора isNull
template<typename T>
constexpr bool has_is_null_v = has_is_null<T>::value;

/// Структура для проверки наличия функций begin()
template<typename T, typename = void>
struct has_begin : std::false_type
{
};

/// Структура для проверки наличия функций begin()
template<typename T>
struct has_begin<T, type_converter_api::sfinae::void_t<decltype(std::declval<T>().begin())>>
    : std::true_type
{
};

/// Результат проверки наличия функций begin()
template<typename T>
constexpr bool has_begin_v = has_begin<T>::value;

/// Структура для проверки наличия функций end()
template<typename T, typename = void>
struct has_end : std::false_type
{
};

/// Структура для проверки наличия функций end()
template<typename T>
struct has_end<T, type_converter_api::sfinae::void_t<decltype(std::declval<T>().end())>>
    : std::true_type
{
};

/// Результат проверки наличия функций  end()
template<typename T>
constexpr bool has_end_v = has_end<T>::value;

template<typename T>
constexpr bool is_iterable_v = has_begin_v<T> && has_end_v<T>;
} // namespace sfinae
} // namespace entity_craft