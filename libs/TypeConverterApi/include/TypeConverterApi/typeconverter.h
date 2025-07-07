#pragma once

#include "sfinae.h"

#include <memory>
#include <sstream>
#include <string>

#ifdef USE_TYPE_QT
#    include <QDateTime>
#    include <QJsonDocument>
#    include <QJsonObject>
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
void fill_from_string(T&, const std::string&, ...)
{
    throw std::runtime_error("fill_from_string not implemented");
}

template<typename T>
std::string convert_to_string(const T&, ...)
{
    throw std::runtime_error("convert_to_string not implemented");
}

} // namespace impl

template<typename T>
class type_converter
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(T& value, const std::string& str) const
    {
        impl::fill_from_string<T>(value, str, 0);
    }

    virtual std::string convert_to_string(const T& value) const
    {
        return impl::convert_to_string<T>(value, 0);
    }
};

template<>
class type_converter<bool>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(bool& value, const std::string& str) const
    {
        std::stringstream stream;
        stream << str;
        stream >> std::boolalpha >> value;
    }

    virtual std::string convert_to_string(const bool& value) const
    {
        return value ? "true" : "false";
    }
};

template<>
class type_converter<std::string>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(std::string& value, const std::string& str) const
    {
        value = str;
    }

    virtual std::string convert_to_string(const std::string& value) const
    {
        return value;
    }
};

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

        std::stringstream stream;
        stream << str;
        stream >> *value;
    }

    virtual std::string convert_to_string(const std::shared_ptr<T>& value) const
    {
        if(value == nullptr)
            return "";

        std::stringstream stream;
        stream << *value;

        return stream.str();
    }
};

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

        std::stringstream stream;
        stream << str;
        stream >> *value;
    }

    virtual std::string convert_to_string(const std::unique_ptr<T>& value) const
    {
        if(value == nullptr)
            return "";

        std::stringstream stream;
        stream << *value;

        return stream.str();
    }
};

template<typename T>
class type_converter<T*>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(T*& value, const std::string& str) const
    {
        if(str.empty())
            return;

        if(value == nullptr)
            value = new T();

        std::stringstream stream;
        stream << str;
        stream >> *value;
    }

    virtual std::string convert_to_string(T*& value) const
    {
        if(value == nullptr)
            return "";

        std::stringstream stream;
        stream << *value;

        return stream.str();
    }
};

#ifdef USE_TYPE_QT

template<>
class type_converter<QDateTime>
{
    static constexpr auto mask = "yyyy-MM-ddTHH:mm:ss.zzz";

public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(QDateTime& value, const std::string& str) const
    {
        value = QDateTime::fromString(QString::fromStdString(str), mask);
    }

    virtual std::string convert_to_string(const QDateTime& value) const
    {
        return value.toString(mask).toStdString();
    }
};

template<>
class type_converter<QDate>
{
    static constexpr auto mask = "yyyy-MM-dd";

public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(QDate& value, const std::string& str) const
    {
        value = QDate::fromString(QString::fromStdString(str), mask);
    }

    virtual std::string convert_to_string(const QDate& value) const
    {
        return value.toString(mask).toStdString();
    }
};

template<>
class type_converter<QString>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(QString& value, const std::string& str) const
    {
        value = QString::fromStdString(str);
    }

    virtual std::string convert_to_string(const QString& value) const
    {
        return value.toStdString();
    }
};

template<>
class type_converter<QByteArray>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(QByteArray& value, const std::string& str) const
    {
        value = QByteArray::fromStdString(str);
    }

    virtual std::string convert_to_string(const QByteArray& value) const
    {
        return value.toStdString();
    }
};

template<>
class type_converter<QJsonObject>
{
public:
    virtual ~type_converter() = default;

    virtual void fill_from_string(QJsonObject& value, const std::string& str) const
    {
        value = QJsonDocument::fromJson(QString::fromStdString(str).toUtf8()).object();
    }

    virtual std::string convert_to_string(const QJsonObject& value) const
    {
        return QString(QJsonDocument(value).toJson(QJsonDocument::Indented)).toStdString();
    }
};

#endif

} // namespace type_converter_api