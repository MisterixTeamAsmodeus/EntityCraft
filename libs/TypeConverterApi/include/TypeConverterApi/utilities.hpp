#pragma once

#include "helper/sfinae.hpp"
#include "typeconverter.hpp"

#include <string>

namespace type_converter_api {

namespace impl {
template<typename Container, typename ValueType,
    std::enable_if_t<sfinae::has_push_back_v<Container, ValueType>, bool> = true>
void insert_item(Container& container, const ValueType& item, int)
{
    container.push_back(item);
}

template<typename Container, typename ValueType,
    std::enable_if_t<!sfinae::has_push_back_v<Container, ValueType> && sfinae::has_insert_v<Container, ValueType>, bool> = true>
void insert_item(Container& container, const ValueType& item, int)
{
    container.insert(item);
}

template<typename Container, typename ValueType,
    std::enable_if_t<!sfinae::has_push_back_v<Container, ValueType> && !sfinae::has_insert_v<Container, ValueType> && sfinae::has_emplace_back_v<Container, ValueType>, bool> = true>
void insert_item(Container& container, const ValueType& item, int)
{
    container.emplace_back(item);
}

template<typename Container, typename ValueType>
void insert_item(Container& container, const ValueType& item, ...)
{
    container.insert(container.end(), item);
}
} // namespace impl

/// \brief Свободная функция для конвертации значения в строку
/// \tparam T Тип значения для конвертации
/// \param value Значение для конвертации
/// \return Строковое представление значения
/// \example
/// \code
/// int value = 42;
/// std::string str = to_string(value); // "42"
/// \endcode
template<typename T>
std::string to_string(const T& value)
{
    return type_converter<T>().convert_to_string(value);
}

/// \brief Свободная функция для заполнения значения из строки
/// \tparam T Тип значения для заполнения
/// \param value Ссылка на значение для заполнения
/// \param str Строка для парсинга
/// \throws std::invalid_argument если строка не может быть преобразована
/// \throws std::out_of_range если значение выходит за границы типа
/// \example
/// \code
/// int value;
/// from_string(value, "42"); // value = 42
/// \endcode
template<typename T>
void from_string(T& value, const std::string& str)
{
    type_converter<T>().fill_from_string(value, str);
}

/// \brief Свободная функция для создания значения из строки
/// \tparam T Тип значения для создания
/// \param str Строка для парсинга
/// \return Созданное значение
/// \throws std::invalid_argument если строка не может быть преобразована
/// \throws std::out_of_range если значение выходит за границы типа
/// \example
/// \code
/// int value = from_string<int>("42"); // value = 42
/// \endcode
template<typename T>
T from_string(const std::string& str)
{
    T value {};
    type_converter<T>().fill_from_string(value, str);
    return value;
}

/// \brief Утилита для массовой конвертации контейнера в строку
/// \tparam Container Тип контейнера
/// \param container Контейнер для конвертации
/// \param delimiter Разделитель между элементами (по умолчанию ",")
/// \return Строковое представление контейнера
/// \example
/// \code
/// std::vector<int> vec = {1, 2, 3};
/// std::string str = container_to_string(vec); // "1,2,3"
/// std::string str2 = container_to_string(vec, "|"); // "1|2|3"
/// \endcode
template<typename Container>
std::string container_to_string(const Container& container, const std::string& delimiter = ",")
{
    if(container.empty())
        return "";

    using ValueType = typename Container::value_type;
    type_converter<ValueType> converter;

    std::string result;
    bool first = true;
    for(const auto& item : container) {
        if(!first)
            result += delimiter;
        result += converter.convert_to_string(item);
        first = false;
    }
    return result;
}

/// \brief Утилита для заполнения контейнера из строки
/// \tparam Container Тип контейнера
/// \param container Контейнер для заполнения
/// \param str Строка для парсинга
/// \param delimiter Разделитель между элементами (по умолчанию ",")
/// \throws std::invalid_argument если строка не может быть преобразована
/// \example
/// \code
/// std::vector<int> vec;
/// container_from_string(vec, "1,2,3"); // vec = {1, 2, 3}
/// \endcode
template<typename Container>
void container_from_string(Container& container, const std::string& str, const std::string& delimiter = ",")
{
    if(sfinae::has_clear_v<Container>) {
        container.clear();
    }
    if(str.empty())
        return;

    using ValueType = typename Container::value_type;
    type_converter<ValueType> converter;

    std::size_t start = 0;
    std::size_t end = str.find(delimiter);

    while(end != std::string::npos) {
        std::string item_str = str.substr(start, end - start);
        if(!item_str.empty()) {
            ValueType item;
            converter.fill_from_string(item, item_str);
            impl::insert_item(container, item, 0);
        }
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }

    std::string item_str = str.substr(start);
    if(!item_str.empty()) {
        ValueType item;
        converter.fill_from_string(item, item_str);
        impl::insert_item(container, item, 0);
    }
}

/// \brief Утилита для конвертации между различными строковыми типами
/// \tparam TargetString Тип целевой строки
/// \tparam SourceString Тип исходной строки
/// \param source Исходная строка
/// \return Конвертированная строка
/// \example
/// \code
/// std::string str = "Hello";
/// std::wstring wstr = string_convert<std::wstring>(str);
/// \endcode
template<typename TargetString, typename SourceString>
TargetString string_convert(const SourceString& source)
{
    type_converter<TargetString> converter;
    std::string intermediate = type_converter<SourceString>().convert_to_string(source);
    TargetString result;
    converter.fill_from_string(result, intermediate);
    return result;
}

} // namespace type_converter_api
