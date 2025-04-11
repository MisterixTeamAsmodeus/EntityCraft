#pragma once

#include <memory>
#include <string>

namespace EntityCraft {

namespace Helpers {
/// Специализация для всех целочисленных типов
template<typename T,
    std::enable_if_t<std::is_integral<T>::value, bool> = true>
bool isNull(const T& value)
{
    return value == 0;
}

/// Специализация для всех типов с плавующей точкой
template<typename T,
    std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
bool isNull(const T& value)
{
    return value == 0;
}

/// Специализация по умолчания
/// @note При добавлении частичной специализации для других типов прописывать std::enable_if_t
template<typename T,
    std::enable_if_t<!std::is_floating_point<T>::value, bool> = true,
    std::enable_if_t<!std::is_integral<T>::value, bool> = true>
bool isNull(const T&)
{
    return false;
}
} // namespace Helpers

template<typename T>
class NullCheker
{
public:
    virtual ~NullCheker() = default;

    virtual bool isNull(const T& value) const
    {
        return Helpers::isNull(value);
    }
};

template<>
class NullCheker<std::string> final
{
public:
    ~NullCheker() = default;

    bool isNull(const std::string& value) const
    {
        return value.empty();
    }
};

template<typename T>
class NullCheker<std::shared_ptr<T>> final
{
public:
    ~NullCheker() = default;

    bool isNull(const std::shared_ptr<T>& value) const
    {
        return value == nullptr;
    }
};

template<typename T>
class NullCheker<std::unique_ptr<T>> final
{
public:
    ~NullCheker() = default;

    bool isNull(const std::unique_ptr<T>& value) const
    {
        return value == nullptr;
    }
};

} // namespace EntityCraft