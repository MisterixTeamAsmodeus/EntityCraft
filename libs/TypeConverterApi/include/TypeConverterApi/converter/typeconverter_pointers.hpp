#pragma once

#include "typeconverter_base.hpp"

#include <memory>
#include <new>
#include <stdexcept>
#include <string>

namespace type_converter_api {

/// Специализация для std::shared_ptr
template<typename T>
class type_converter<std::shared_ptr<T>>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::shared_ptr<T>& value, const std::string& str) const
    {
        if(str.empty())
            return;

        if(value == nullptr)
            value = std::make_shared<T>();

        T tempValue;
        type_converter_api::type_converter<T>().fill_from_string(tempValue, str);
        *value = tempValue;
    }

    virtual std::string convert_to_string(const std::shared_ptr<T>& value) const noexcept
    {
        if(value == nullptr)
            return "";

        return type_converter_api::type_converter<T>().convert_to_string(*value);
    }
};

/// Специализация для std::unique_ptr
template<typename T>
class type_converter<std::unique_ptr<T>>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::unique_ptr<T>& value, const std::string& str) const
    {
        if(str.empty())
            return;

        if(value == nullptr)
            value = std::make_unique<T>();

        T tempValue;
        type_converter_api::type_converter<T>().fill_from_string(tempValue, str);
        *value = tempValue;
    }

    virtual std::string convert_to_string(const std::unique_ptr<T>& value) const noexcept
    {
        if(value == nullptr)
            return "";

        return type_converter_api::type_converter<T>().convert_to_string(*value);
    }
};

/// Специализация для сырых указателей T*
/// \warning Использование сырых указателей не рекомендуется.
/// Предпочтительно использовать std::shared_ptr или std::unique_ptr.
/// Эта специализация создает объект через new, но не управляет его временем жизни.
/// Пользователь должен самостоятельно управлять памятью.
template<typename T>
class type_converter<T*>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(T*& value, const std::string& str) const
    {
        if(str.empty()) {
            if(value != nullptr) {
                delete value;
                value = nullptr;
            }
            return;
        }

        if(value == nullptr) {
            value = new(std::nothrow) T();
            if(value == nullptr) {
                throw std::bad_alloc();
            }
        }

        T tempValue;
        type_converter_api::type_converter<T>().fill_from_string(tempValue, str);
        *value = tempValue;
    }

    virtual std::string convert_to_string(T* const& value) const
    {
        if(value == nullptr)
            return "";

        return type_converter_api::type_converter<T>().convert_to_string(*value);
    }
};

} // namespace type_converter_api
