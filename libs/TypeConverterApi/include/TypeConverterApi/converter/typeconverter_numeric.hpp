#pragma once

#include "typeconverter_base.hpp"

#include <limits>
#include <stdexcept>
#include <string>

namespace type_converter_api {

/// Специализация для int
template<>
class type_converter<int>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(int& value, const std::string& str) const
    {
        try {
            std::size_t pos = 0;
            long long result = std::stoll(str, &pos);
            if(pos != str.length()) {
                throw std::invalid_argument("Invalid integer: " + str);
            }
            if(result < std::numeric_limits<int>::min() || result > std::numeric_limits<int>::max()) {
                throw std::out_of_range("Integer out of range: " + str);
            }
            value = static_cast<int>(result);
        } catch(const std::invalid_argument&) {
            throw;
        } catch(const std::out_of_range&) {
            throw;
        }
    }

    virtual std::string convert_to_string(const int& value) const noexcept
    {
        return std::to_string(value);
    }
};

/// Специализация для long
template<>
class type_converter<long>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(long& value, const std::string& str) const
    {
        try {
            std::size_t pos = 0;
            value = std::stol(str, &pos);
            if(pos != str.length()) {
                throw std::invalid_argument("Invalid long: " + str);
            }
        } catch(const std::invalid_argument&) {
            throw;
        } catch(const std::out_of_range&) {
            throw;
        }
    }

    virtual std::string convert_to_string(const long& value) const noexcept
    {
        return std::to_string(value);
    }
};

/// Специализация для long long
template<>
class type_converter<long long>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(long long& value, const std::string& str) const
    {
        try {
            std::size_t pos = 0;
            value = std::stoll(str, &pos);
            if(pos != str.length()) {
                throw std::invalid_argument("Invalid long long: " + str);
            }
        } catch(const std::invalid_argument&) {
            throw;
        } catch(const std::out_of_range&) {
            throw;
        }
    }

    virtual std::string convert_to_string(const long long& value) const noexcept
    {
        return std::to_string(value);
    }
};

/// Специализация для unsigned int
template<>
class type_converter<unsigned int>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(unsigned int& value, const std::string& str) const
    {
        try {
            std::size_t pos = 0;
            unsigned long long result = std::stoull(str, &pos);
            if(pos != str.length()) {
                throw std::invalid_argument("Invalid unsigned integer: " + str);
            }
            if(result > std::numeric_limits<unsigned int>::max()) {
                throw std::out_of_range("Unsigned integer out of range: " + str);
            }
            value = static_cast<unsigned int>(result);
        } catch(const std::invalid_argument&) {
            throw;
        } catch(const std::out_of_range&) {
            throw;
        }
    }

    virtual std::string convert_to_string(const unsigned int& value) const noexcept
    {
        return std::to_string(value);
    }
};

/// Специализация для unsigned long
template<>
class type_converter<unsigned long>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(unsigned long& value, const std::string& str) const
    {
        try {
            std::size_t pos = 0;
            value = std::stoul(str, &pos);
            if(pos != str.length()) {
                throw std::invalid_argument("Invalid unsigned long: " + str);
            }
        } catch(const std::invalid_argument&) {
            throw;
        } catch(const std::out_of_range&) {
            throw;
        }
    }

    virtual std::string convert_to_string(const unsigned long& value) const noexcept
    {
        return std::to_string(value);
    }
};

/// Специализация для unsigned long long
template<>
class type_converter<unsigned long long>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(unsigned long long& value, const std::string& str) const
    {
        try {
            std::size_t pos = 0;
            value = std::stoull(str, &pos);
            if(pos != str.length()) {
                throw std::invalid_argument("Invalid unsigned long long: " + str);
            }
        } catch(const std::invalid_argument&) {
            throw;
        } catch(const std::out_of_range&) {
            throw;
        }
    }

    virtual std::string convert_to_string(const unsigned long long& value) const noexcept
    {
        return std::to_string(value);
    }
};

/// Специализация для float
template<>
class type_converter<float>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(float& value, const std::string& str) const
    {
        try {
            std::size_t pos = 0;
            double result = std::stod(str, &pos);
            if(pos != str.length()) {
                throw std::invalid_argument("Invalid float: " + str);
            }
            value = static_cast<float>(result);
        } catch(const std::invalid_argument&) {
            throw;
        } catch(const std::out_of_range&) {
            throw;
        }
    }

    virtual std::string convert_to_string(const float& value) const noexcept
    {
        return std::to_string(value);
    }
};

/// Специализация для double
template<>
class type_converter<double>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(double& value, const std::string& str) const
    {
        try {
            std::size_t pos = 0;
            value = std::stod(str, &pos);
            if(pos != str.length()) {
                throw std::invalid_argument("Invalid double: " + str);
            }
        } catch(const std::invalid_argument&) {
            throw;
        } catch(const std::out_of_range&) {
            throw;
        }
    }

    virtual std::string convert_to_string(const double& value) const noexcept
    {
        return std::to_string(value);
    }
};

/// Специализация для long double
template<>
class type_converter<long double>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(long double& value, const std::string& str) const
    {
        try {
            std::size_t pos = 0;
            value = std::stold(str, &pos);
            if(pos != str.length()) {
                throw std::invalid_argument("Invalid long double: " + str);
            }
        } catch(const std::invalid_argument&) {
            throw;
        } catch(const std::out_of_range&) {
            throw;
        }
    }

    virtual std::string convert_to_string(const long double& value) const noexcept
    {
        return std::to_string(value);
    }
};

/// Специализация для short
template<>
class type_converter<short>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(short& value, const std::string& str) const
    {
        try {
            std::size_t pos = 0;
            long result = std::stol(str, &pos);
            if(pos != str.length()) {
                throw std::invalid_argument("Invalid short: " + str);
            }
            if(result < std::numeric_limits<short>::min() || result > std::numeric_limits<short>::max()) {
                throw std::out_of_range("Short out of range: " + str);
            }
            value = static_cast<short>(result);
        } catch(const std::invalid_argument&) {
            throw;
        } catch(const std::out_of_range&) {
            throw;
        }
    }

    virtual std::string convert_to_string(const short& value) const noexcept
    {
        return std::to_string(value);
    }
};

/// Специализация для unsigned short
template<>
class type_converter<unsigned short>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(unsigned short& value, const std::string& str) const
    {
        try {
            std::size_t pos = 0;
            unsigned long result = std::stoul(str, &pos);
            if(pos != str.length()) {
                throw std::invalid_argument("Invalid unsigned short: " + str);
            }
            if(result > std::numeric_limits<unsigned short>::max()) {
                throw std::out_of_range("Unsigned short out of range: " + str);
            }
            value = static_cast<unsigned short>(result);
        } catch(const std::invalid_argument&) {
            throw;
        } catch(const std::out_of_range&) {
            throw;
        }
    }

    virtual std::string convert_to_string(const unsigned short& value) const noexcept
    {
        return std::to_string(value);
    }
};

/// Специализация для char
template<>
class type_converter<char>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(char& value, const std::string& str) const
    {
        if(str.length() != 1) {
            throw std::invalid_argument("Invalid char: string must be exactly one character");
        }
        value = str[0];
    }

    virtual std::string convert_to_string(const char& value) const noexcept
    {
        return std::string(1, value);
    }
};

/// Специализация для unsigned char
template<>
class type_converter<unsigned char>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(unsigned char& value, const std::string& str) const
    {
        try {
            std::size_t pos = 0;
            unsigned long result = std::stoul(str, &pos);
            if(pos != str.length()) {
                throw std::invalid_argument("Invalid unsigned char: " + str);
            }
            if(result > std::numeric_limits<unsigned char>::max()) {
                throw std::out_of_range("Unsigned char out of range: " + str);
            }
            value = static_cast<unsigned char>(result);
        } catch(const std::invalid_argument&) {
            throw;
        } catch(const std::out_of_range&) {
            throw;
        }
    }

    virtual std::string convert_to_string(const unsigned char& value) const noexcept
    {
        return std::to_string(static_cast<unsigned int>(value));
    }
};

} // namespace type_converter_api
