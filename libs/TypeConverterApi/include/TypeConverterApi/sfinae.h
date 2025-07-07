#pragma once

#include "void_t.h"

#include <iostream>
#include <type_traits>
#include <utility>

#ifdef USE_TYPE_QT
#    include <QString>
#endif

namespace type_converter_api {
namespace sfinae {

#ifdef USE_TYPE_QT
/// Структура для проверки наличия оператора toString
template<typename T, typename = void>
struct has_to_string : std::false_type
{
};

/// Структура для проверки наличия оператора toString
template<typename T>
struct has_to_string<T, void_t<decltype(std::declval<T>().toString())>>
    : std::true_type
{
};

/// Результат для проверки наличия оператора toString
template<typename T>
constexpr bool has_to_string_v = has_to_string<T>::value;

/// Структура для проверки наличия оператора fromString
template<typename T, typename = void>
struct has_from_string : std::false_type
{
};

/// Структура для проверки наличия оператора fromString
template<typename T>
struct has_from_string<T, void_t<decltype(T::fromString(QString()))>>
    : std::true_type
{
};

/// Результат проверки наличия оператора fromString
template<typename T>
constexpr bool has_from_string_v = has_from_string<T>::value;

/// Структура для проверки наличия оператора << для вставки в контейнер
template<typename Container, typename T, typename = void>
struct has_left_shift_container_operator : std::false_type
{
};

/// Структура для проверки наличия оператора << для вставки в контейнер
template<typename Container, typename T>
struct has_left_shift_container_operator<Container, T, void_t<decltype((std::declval<Container&>() << std::declval<T>()))>>
    : std::true_type
{
};

/// Результат проверки наличия оператора << для вставки в контейнер
template<typename Container, typename T>
constexpr bool has_left_shift_container_operator_v = has_left_shift_container_operator<Container, T>::value;
#endif

/// Структура для проверки наличия оператора <<
template<typename T, typename = void>
struct has_left_shift_operator : std::false_type
{
};

/// Структура для проверки наличия оператора <<
template<typename T>
struct has_left_shift_operator<T, void_t<decltype((std::declval<std::ostream&>() << std::declval<T>()))>>
    : std::true_type
{
};

/// Результат проверки наличия оператора <<
template<typename T>
constexpr bool has_left_shift_operator_v = has_left_shift_operator<T>::value;

/// Структура для проверки наличия оператора >>
template<typename T, typename = void>
struct has_right_shift_operator : std::false_type
{
};

/// Структура для проверки наличия оператора >>
template<typename T>
struct has_right_shift_operator<T, void_t<decltype((std::declval<std::istream&>() >> std::declval<T&>()))>>
    : std::true_type
{
};

/// Результат проверки наличия оператора >>
template<typename T>
constexpr bool has_right_shift_operator_v = has_right_shift_operator<T>::value;

/// Структура для проверки наличия оператора emplace_back
template<typename Container, typename T, typename = void>
struct has_emplace_back : std::false_type
{
};

/// Структура для проверки наличия оператора emplace_back
template<typename Container, typename T>
struct has_emplace_back<Container, T, void_t<decltype((std::declval<Container&>().emplace_back(std::declval<T>())))>>
    : std::true_type
{
};

/// Результат проверки наличия оператора emplace_back
template<typename Container, typename T>
constexpr bool has_emplace_back_v = has_emplace_back<Container, T>::value;

} // namespace sfinae
} // namespace type_converter_api