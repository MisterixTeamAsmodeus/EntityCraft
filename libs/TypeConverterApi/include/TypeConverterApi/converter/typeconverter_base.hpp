#pragma once

#include "helper/sfinae.hpp"

#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>

#ifdef USE_TYPE_QT
#    include <QString>
#endif

namespace type_converter_api {

template<typename T>
class type_converter;

namespace impl {

#ifdef USE_TYPE_QT
/// Реализация для классов имеющих оператор fromString
template<typename T, std::enable_if_t<sfinae::has_from_string_v<T>, bool> = true>
void fill_from_string(T& value, const std::string& str, int)
{
    value = T::fromString(QString::fromStdString(str));
}

/// Реализация для классов имеющих оператор toString
template<typename T, std::enable_if_t<sfinae::has_to_string_v<T>, bool> = true>
std::string convert_to_string(const T& value, int)
{
    return value.toString().toStdString();
}
#endif

template<typename T, std::enable_if_t<std::is_enum<T>::value, bool> = true>
void fill_from_string(T& value, const std::string& str, int)
{
    std::underlying_type_t<T> t;
    type_converter<decltype(t)>().fill_from_string(t, str);

    value = static_cast<T>(t);
}

template<typename T, std::enable_if_t<std::is_enum<T>::value, bool> = true>
std::string convert_to_string(const T& value, int)
{
    return type_converter<std::underlying_type_t<T>>().convert_to_string(static_cast<std::underlying_type_t<T>>(value));
}

template<typename T, std::enable_if_t<sfinae::has_right_shift_operator_v<T> && !std::is_enum<T>::value, bool> = true>
void fill_from_string(T& value, const std::string& str, int)
{
    std::stringstream stream;
    stream << str;
    stream >> value;
}

template<typename T, std::enable_if_t<sfinae::has_left_shift_operator_v<T> && !std::is_enum<T>::value, bool> = true>
std::string convert_to_string(const T& value, int)
{
    std::stringstream stream;
    stream << value;

    return stream.str();
}

template<typename T>
void fill_from_string(T&, const std::string& str, ...)
{
    std::stringstream message;
    message << "fill_from_string not implemented for type " << typeid(T).name();
    if(!str.empty()) {
        message << " (input: \"" << str << "\")";
    }
    throw std::runtime_error(message.str());
}

template<typename T>
std::string convert_to_string(const T&, ...)
{
    std::stringstream message;
    message << "convert_to_string not implemented for type " << typeid(T).name();
    throw std::runtime_error(message.str());
}

} // namespace impl

/// \brief Базовый класс для конвертации типов в строки и обратно
/// \tparam T Тип для конвертации
template<typename T>
class type_converter
{
public:
    virtual ~type_converter() = default;

    /// \brief Заполняет значение из строки
    /// \param value Ссылка на значение для заполнения
    /// \param str Строка для парсинга
    virtual void fill_from_string(T& value, const std::string& str) const
    {
        impl::fill_from_string<T>(value, str, 0);
    }

    /// \brief Конвертирует значение в строку
    /// \param value Значение для конвертации
    /// \return Строковое представление значения
    virtual std::string convert_to_string(const T& value) const
    {
        return impl::convert_to_string<T>(value, 0);
    }
};

} // namespace type_converter_api
