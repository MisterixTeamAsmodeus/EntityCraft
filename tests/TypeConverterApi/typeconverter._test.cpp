#include <gtest/gtest.h>
#include <TypeConverterApi/typeconverterapi.h>

// Test for converting a string to an integer
TEST(TypeConverterTest, StringToInt)
{
    type_converter_api::type_converter<int> converter;
    int value = 0;
    converter.fill_from_string(value, "123");
    EXPECT_EQ(value, 123);
}

// Test for converting a string to a boolean
TEST(TypeConverterTest, StringToBool)
{
    const type_converter_api::type_converter<bool> converter;
    bool value = false;
    converter.fill_from_string(value, "true");
    EXPECT_TRUE(value);
}

// Test for converting an integer to a string
TEST(TypeConverterTest, IntToString)
{
    const type_converter_api::type_converter<int> converter;
    const std::string str = converter.convert_to_string(456);
    EXPECT_EQ(str, "456");
}

// Test for converting a boolean to a string
TEST(TypeConverterTest, BoolToString)
{
    type_converter_api::type_converter<bool> converter;
    std::string str = converter.convert_to_string(true);
    EXPECT_EQ(str, "true");
}

// Test for handling empty string input
TEST(TypeConverterTest, EmptyStringInput)
{
    type_converter_api::type_converter<int> converter;
    int value = 0;
    converter.fill_from_string(value, "");
    EXPECT_EQ(value, 0); // Assuming default value is 0 for empty string
}

// Test for converting a string to an enum
enum class Color
{
    RED,
    GREEN,
    BLUE
};

TEST(TypeConverterTest, StringToEnum)
{
    const type_converter_api::type_converter<Color> converter;
    Color color;
    converter.fill_from_string(color, "1");
    EXPECT_EQ(color, Color::GREEN);
}

// Test for converting an enum to a string
TEST(TypeConverterTest, EnumToString)
{
    const type_converter_api::type_converter<Color> converter;
    const std::string str = converter.convert_to_string(Color::BLUE);
    EXPECT_EQ(str, "2");
}

#ifdef USE_TYPE_QT
// Test for converting a string to QDateTime
TEST(QtTypeConverterTest, StringToQDateTime)
{
    type_converter_api::type_converter<QDateTime> converter;
    QDateTime value = QDateTime();
    converter.fill_from_string(value, "2023-10-05T14:30:00.000");
    EXPECT_EQ(value.toString("yyyy-MM-ddTHH:mm:ss.zzz"), "2023-10-05T14:30:00.000");
}

// Test for converting a QDateTime to string
TEST(QtTypeConverterTest, QDateTimeToString)
{
    type_converter_api::type_converter<QDateTime> converter;
    const std::string str = converter.convert_to_string(QDateTime::currentDateTime());
    EXPECT_TRUE(str.find("yyyy-MM-ddTHH:mm:ss.zzz") != std::string::npos);
}

// Test for converting a string to QDate
TEST(QtTypeConverterTest, StringToQDate)
{
    type_converter_api::type_converter<QDate> converter;
    QDate value = QDate();
    converter.fill_from_string(value, "2023-10-05");
    EXPECT_EQ(value.toString("yyyy-MM-dd"), "2023-10-05");
}

// Test for converting a QDate to string
TEST(QtTypeConverterTest, QDateToString)
{
    type_converter_api::type_converter<QDate> converter;
    const std::string str = converter.convert_to_string(QDate::currentDate());
    EXPECT_TRUE(str.find("yyyy-MM-dd") != std::string::npos);
}

// Test for converting a string to QString
TEST(QtTypeConverterTest, StringToQString)
{
    type_converter_api::type_converter<QString> converter;
    QString value;
    converter.fill_from_string(value, "Hello, World!");
    EXPECT_EQ(value.toStdString(), "Hello, World!");
}

// Test for converting a QString to string
TEST(QtTypeConverterTest, QStringToString)
{
    type_converter_api::type_converter<QString> converter;
    const std::string str = converter.convert_to_string(QString("Hello, World!"));
    EXPECT_EQ(str, "Hello, World!");
}

// Test for converting a string to QByteArray
TEST(QtTypeConverterTest, StringToQByteArray)
{
    type_converter_api::type_converter<QByteArray> converter;
    QByteArray value;
    converter.fill_from_string(value, "Hello, World!");
    EXPECT_EQ(value.toStdString(), "Hello, World!");
}

// Test for converting a QByteArray to string
TEST(QtTypeConverterTest, QByteArrayToString)
{
    type_converter_api::type_converter<QByteArray> converter;
    const std::string str = converter.convert_to_string(QByteArray("Hello, World!"));
    EXPECT_EQ(str, "Hello, World!");
}

// Test for converting a string to QJsonObject
TEST(QtTypeConverterTest, StringToQJsonObject)
{
    type_converter_api::type_converter<QJsonObject> converter;
    QJsonObject value;
    converter.fill_from_string(value, "{\"key\":\"value\"}");
    EXPECT_TRUE(value.contains("key"));
    EXPECT_EQ(value["key"].toString(), "value");
}

// Test for converting a QJsonObject to string
TEST(QtTypeConverterTest, QJsonObjectToString)
{
    type_converter_api::type_converter<QJsonObject> converter;
    const std::string str = converter.convert_to_string(QJsonObject({ { "key", "value" } }));
    EXPECT_TRUE(str.find("\"key\":\"value\"") != std::string::npos);
}

#endif