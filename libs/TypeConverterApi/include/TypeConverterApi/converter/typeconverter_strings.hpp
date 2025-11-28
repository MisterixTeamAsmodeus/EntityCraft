#pragma once

#include "typeconverter_base.hpp"

#include <codecvt>
#include <locale>
#include <string>

namespace type_converter_api {

/// Специализация для bool
template<>
class type_converter<bool>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(bool& value, const std::string& str) const
    {
        if(str == "true" || str == "1" || str == "True" || str == "TRUE") {
            value = true;
        } else if(str == "false" || str == "0" || str == "False" || str == "FALSE") {
            value = false;
        } else {
            throw std::invalid_argument("Invalid boolean string: " + str);
        }
    }

    virtual std::string convert_to_string(const bool& value) const noexcept
    {
        return value ? "true" : "false";
    }
};

/// Специализация для std::string
template<>
class type_converter<std::string>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::string& value, const std::string& str) const noexcept
    {
        value = str;
    }

    virtual std::string convert_to_string(const std::string& value) const noexcept
    {
        return value;
    }
};

/// Специализация для std::wstring
template<>
class type_converter<std::wstring>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::wstring& value, const std::string& str) const
    {
        // Конвертация из UTF-8 в wstring
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        value = converter.from_bytes(str);
    }

    virtual std::string convert_to_string(const std::wstring& value) const
    {
        // Конвертация из wstring в UTF-8
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(value);
    }
};

/// Специализация для std::u16string
template<>
class type_converter<std::u16string>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::u16string& value, const std::string& str) const
    {
        // Конвертация из UTF-8 в UTF-16
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
        value = converter.from_bytes(str);
    }

    virtual std::string convert_to_string(const std::u16string& value) const
    {
        // Конвертация из UTF-16 в UTF-8
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
        return converter.to_bytes(value);
    }
};

/// Специализация для std::u32string
template<>
class type_converter<std::u32string>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::u32string& value, const std::string& str) const
    {
        // Конвертация из UTF-8 в UTF-32
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
        value = converter.from_bytes(str);
    }

    virtual std::string convert_to_string(const std::u32string& value) const
    {
        // Конвертация из UTF-32 в UTF-8
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
        return converter.to_bytes(value);
    }
};

} // namespace type_converter_api
