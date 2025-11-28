#pragma once

#include "typeconverter_base.hpp"

#include <string>

#ifdef USE_TYPE_QT
#    include <QByteArray>
#    include <QDate>
#    include <QDateTime>
#    include <QJsonDocument>
#    include <QJsonObject>
#    include <QString>

namespace type_converter_api {

/// Специализация для QDateTime
template<>
class type_converter<QDateTime>
{
    static constexpr auto default_mask = "yyyy-MM-dd HH:mm:ss.zzz";

public:
    explicit type_converter(std::string mask = default_mask)
        : mask(std::move(mask))
    {
    }

    virtual ~type_converter() = default;

    virtual void fill_from_string(QDateTime& value, const std::string& str) const
    {
        value = QDateTime::fromString(QString::fromStdString(str), mask.c_str());
    }

    virtual std::string convert_to_string(const QDateTime& value) const
    {
        return value.toString(mask.c_str()).toStdString();
    }

private:
    std::string mask;
};

/// Специализация для QDate
template<>
class type_converter<QDate>
{
    static constexpr auto default_mask = "yyyy-MM-dd";

public:
    explicit type_converter(std::string mask = default_mask)
        : mask(std::move(mask))
    {
    }

    virtual ~type_converter() = default;

    virtual void fill_from_string(QDate& value, const std::string& str) const
    {
        value = QDate::fromString(QString::fromStdString(str), mask.c_str());
    }

    virtual std::string convert_to_string(const QDate& value) const
    {
        return value.toString(mask.c_str()).toStdString();
    }

private:
    std::string mask;
};

/// Специализация для QString
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

/// Специализация для QByteArray
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

/// Специализация для QJsonObject
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

} // namespace type_converter_api

#endif // USE_TYPE_QT
