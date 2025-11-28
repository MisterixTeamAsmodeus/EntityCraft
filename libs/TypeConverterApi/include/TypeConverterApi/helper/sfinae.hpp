#pragma once

#include "void_t.hpp"

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
#endif

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

/// Структура для проверки наличия метода push_back
template<typename Container, typename T, typename = void>
struct has_push_back : std::false_type
{
};

/// Структура для проверки наличия метода push_back
template<typename Container, typename T>
struct has_push_back<Container, T, void_t<decltype((std::declval<Container&>().push_back(std::declval<T>())))>>
    : std::true_type
{
};

/// Результат проверки наличия метода push_back
template<typename Container, typename T>
constexpr bool has_push_back_v = has_push_back<Container, T>::value;

/// Структура для проверки наличия метода insert
template<typename Container, typename T, typename = void>
struct has_insert : std::false_type
{
};

/// Структура для проверки наличия метода insert
template<typename Container, typename T>
struct has_insert<Container, T, void_t<decltype((std::declval<Container&>().insert(std::declval<T>())))>>
    : std::true_type
{
};

/// Результат проверки наличия метода insert
template<typename Container, typename T>
constexpr bool has_insert_v = has_insert<Container, T>::value;

/// Структура для проверки наличия метода clear
template<typename Container, typename = void>
struct has_clear : std::false_type
{
};

/// Структура для проверки наличия метода clear
template<typename Container>
struct has_clear<Container, void_t<decltype(std::declval<Container&>().clear())>>
    : std::true_type
{
};

/// Результат проверки наличия метода clear
template<typename Container>
constexpr bool has_clear_v = has_clear<Container>::value;

/// \brief Проверка наличия метода reserve для оптимизации
template<typename Container, typename = void>
struct has_reserve : std::false_type
{
};

template<typename Container>
struct has_reserve<Container, void_t<decltype(std::declval<Container&>().reserve(std::size_t {}))>>
    : std::true_type
{
};

template<typename Container>
constexpr bool has_reserve_v = has_reserve<Container>::value;

/// \brief Проверка, является ли тип числовым (арифметическим)
template<typename T>
struct is_numeric : std::integral_constant<bool,
                        std::is_arithmetic<T>::value && !std::is_same<T, bool>::value && !std::is_same<T, char>::value>
{
};

template<typename T>
constexpr bool is_numeric_v = is_numeric<T>::value;

} // namespace sfinae
} // namespace type_converter_api
