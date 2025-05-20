#pragma once

#include "sfinae.h"

#include <memory>
#include <stdexcept>
#include <string>

namespace entity_craft {
namespace helpers {

/**
 * Шаблонная функция которая обобщает работу с типами у которых есть функция isNull
 * @tparam T тип данных который нужно проверить на NULL
 * @return Возвращает true, если объект пустой, иначе false
 */
template<typename T,
    std::enable_if_t<sfinae::has_is_null_v<T>, bool> = true>
bool is_null(const T& value, int)
{
    return value.isNull();
}

/// Специализация для численных типов
template<typename T,
    std::enable_if_t<std::is_integral<T>::value | std::is_floating_point<T>::value, bool> = true>
bool is_null(const T& value, int)
{
    return value == 0;
}

/// Специализация по умолчания
template<typename T>
bool is_null(const T&, ...)
{
    throw std::runtime_error("is_null not implemented");
}
} // namespace helpers

template<typename T>
class null_cheker
{
public:
    virtual ~null_cheker() = default;

    virtual bool is_null(const T& value) const
    {
        return helpers::is_null(value, 0);
    }
};

template<>
class null_cheker<std::string>
{
public:
    ~null_cheker() = default;

    bool is_null(const std::string& value) const
    {
        return value.empty();
    }
};

template<typename T>
class null_cheker<std::shared_ptr<T>>
{
public:
    ~null_cheker() = default;

    bool is_null(const std::shared_ptr<T>& value) const
    {
        return value == nullptr;
    }
};

template<typename T>
class null_cheker<std::unique_ptr<T>>
{
public:
    ~null_cheker() = default;

    bool is_null(const std::unique_ptr<T>& value) const
    {
        return value == nullptr;
    }
};

template<typename T>
class null_cheker<T*>
{
public:
    ~null_cheker() = default;

    bool is_null(T*& value) const
    {
        return value == nullptr;
    }
};

} // namespace entity_craft