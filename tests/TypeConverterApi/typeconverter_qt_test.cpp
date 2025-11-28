#ifdef USE_TYPE_QT

#include <TypeConverterApi/typeconverterapi.hpp>

#include <gtest/gtest.h>

#include <QByteArray>
#include <QDate>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

// Тесты для QString
TEST(TypeConverterApi, Qt_QStringConversion)
{
    type_converter_api::type_converter<QString> converter;
    
    QString value;
    converter.fill_from_string(value, "Hello, World!");
    EXPECT_EQ(value, QString::fromStdString("Hello, World!"));
    
    std::string result = converter.convert_to_string(QString("test"));
    EXPECT_EQ(result, "test");
}

TEST(TypeConverterApi, Qt_QStringEmptyString)
{
    type_converter_api::type_converter<QString> converter;
    
    QString str;
    converter.fill_from_string(str, "");
    EXPECT_TRUE(str.isEmpty());
}

TEST(TypeConverterApi, Qt_QStringUnicode)
{
    type_converter_api::type_converter<QString> converter;
    
    QString value;
    converter.fill_from_string(value, "Привет, мир!");
    EXPECT_EQ(value, QString::fromStdString("Привет, мир!"));
    
    std::string result = converter.convert_to_string(value);
    EXPECT_EQ(result, "Привет, мир!");
}

// Тесты для QByteArray
TEST(TypeConverterApi, Qt_QByteArrayConversion)
{
    type_converter_api::type_converter<QByteArray> converter;
    
    QByteArray value;
    converter.fill_from_string(value, "Hello, World!");
    EXPECT_EQ(value, QByteArray::fromStdString("Hello, World!"));
    
    std::string result = converter.convert_to_string(QByteArray("test"));
    EXPECT_EQ(result, "test");
}

TEST(TypeConverterApi, Qt_QByteArrayEmpty)
{
    type_converter_api::type_converter<QByteArray> converter;
    
    QByteArray arr;
    converter.fill_from_string(arr, "");
    EXPECT_TRUE(arr.isEmpty());
}

// Тесты для QDate
TEST(TypeConverterApi, Qt_QDateConversion)
{
    type_converter_api::type_converter<QDate> converter;
    
    QDate value;
    converter.fill_from_string(value, "2024-01-15");
    EXPECT_TRUE(value.isValid());
    EXPECT_EQ(value.year(), 2024);
    EXPECT_EQ(value.month(), 1);
    EXPECT_EQ(value.day(), 15);
    
    std::string result = converter.convert_to_string(value);
    EXPECT_EQ(result, "2024-01-15");
}

TEST(TypeConverterApi, Qt_QDateCustomFormat)
{
    type_converter_api::type_converter<QDate> converter("dd.MM.yyyy");
    
    QDate value;
    converter.fill_from_string(value, "15.01.2024");
    EXPECT_TRUE(value.isValid());
    EXPECT_EQ(value.year(), 2024);
    EXPECT_EQ(value.month(), 1);
    EXPECT_EQ(value.day(), 15);
    
    std::string result = converter.convert_to_string(value);
    EXPECT_EQ(result, "15.01.2024");
}

TEST(TypeConverterApi, Qt_QDateInvalid)
{
    type_converter_api::type_converter<QDate> converter;
    
    QDate value;
    converter.fill_from_string(value, "invalid-date");
    EXPECT_FALSE(value.isValid());
}

// Тесты для QDateTime
TEST(TypeConverterApi, Qt_QDateTimeConversion)
{
    type_converter_api::type_converter<QDateTime> converter;
    
    QDateTime value;
    converter.fill_from_string(value, "2024-01-15 14:30:45.123");
    EXPECT_TRUE(value.isValid());
    EXPECT_EQ(value.date().year(), 2024);
    EXPECT_EQ(value.date().month(), 1);
    EXPECT_EQ(value.date().day(), 15);
    EXPECT_EQ(value.time().hour(), 14);
    EXPECT_EQ(value.time().minute(), 30);
    EXPECT_EQ(value.time().second(), 45);
    
    std::string result = converter.convert_to_string(value);
    // Проверяем, что результат содержит ожидаемые части
    EXPECT_TRUE(result.find("2024-01-15") != std::string::npos);
    EXPECT_TRUE(result.find("14:30:45") != std::string::npos);
}

TEST(TypeConverterApi, Qt_QDateTimeCustomFormat)
{
    type_converter_api::type_converter<QDateTime> converter("dd.MM.yyyy HH:mm:ss");
    
    QDateTime value;
    converter.fill_from_string(value, "15.01.2024 14:30:45");
    EXPECT_TRUE(value.isValid());
    EXPECT_EQ(value.date().year(), 2024);
    EXPECT_EQ(value.date().month(), 1);
    EXPECT_EQ(value.date().day(), 15);
    
    std::string result = converter.convert_to_string(value);
    EXPECT_TRUE(result.find("15.01.2024") != std::string::npos);
    EXPECT_TRUE(result.find("14:30:45") != std::string::npos);
}

TEST(TypeConverterApi, Qt_QDateTimeInvalid)
{
    type_converter_api::type_converter<QDateTime> converter;
    
    QDateTime value;
    converter.fill_from_string(value, "invalid-datetime");
    EXPECT_FALSE(value.isValid());
}

// Тесты для QJsonObject
TEST(TypeConverterApi, Qt_QJsonObjectConversion)
{
    type_converter_api::type_converter<QJsonObject> converter;
    
    QJsonObject value;
    std::string json_str = R"({"name":"John","age":30,"city":"New York"})";
    converter.fill_from_string(value, json_str);
    
    EXPECT_TRUE(value.contains("name"));
    EXPECT_TRUE(value.contains("age"));
    EXPECT_TRUE(value.contains("city"));
    EXPECT_EQ(value["name"].toString(), "John");
    EXPECT_EQ(value["age"].toInt(), 30);
    EXPECT_EQ(value["city"].toString(), "New York");
    
    std::string result = converter.convert_to_string(value);
    // Проверяем, что результат содержит ключи
    EXPECT_TRUE(result.find("name") != std::string::npos);
    EXPECT_TRUE(result.find("John") != std::string::npos);
    EXPECT_TRUE(result.find("age") != std::string::npos);
}

TEST(TypeConverterApi, Qt_QJsonObjectEmpty)
{
    type_converter_api::type_converter<QJsonObject> converter;
    
    QJsonObject value;
    converter.fill_from_string(value, "{}");
    EXPECT_TRUE(value.isEmpty());
    
    std::string result = converter.convert_to_string(value);
    EXPECT_TRUE(result.find("{") != std::string::npos);
}

TEST(TypeConverterApi, Qt_QJsonObjectNested)
{
    type_converter_api::type_converter<QJsonObject> converter;
    
    QJsonObject value;
    std::string json_str = R"({"person":{"name":"John","age":30},"city":"New York"})";
    converter.fill_from_string(value, json_str);
    
    EXPECT_TRUE(value.contains("person"));
    EXPECT_TRUE(value.contains("city"));
    QJsonObject person = value["person"].toObject();
    EXPECT_EQ(person["name"].toString(), "John");
    EXPECT_EQ(person["age"].toInt(), 30);
    
    std::string result = converter.convert_to_string(value);
    EXPECT_TRUE(result.find("person") != std::string::npos);
    EXPECT_TRUE(result.find("name") != std::string::npos);
}

TEST(TypeConverterApi, Qt_QJsonObjectInvalid)
{
    type_converter_api::type_converter<QJsonObject> converter;
    
    QJsonObject value;
    converter.fill_from_string(value, "invalid json");
    // QJsonDocument::fromJson возвращает пустой объект при ошибке
    EXPECT_TRUE(value.isEmpty());
}

#endif // USE_TYPE_QT

