#include <TypeConverterApi/typeconverterapi.hpp>

#include <gtest/gtest.h>

// Тесты для числовых типов

TEST(TypeConverterApi, Numeric_IntConversion)
{
    type_converter_api::type_converter<int> converter;

    int value = 0;
    converter.fill_from_string(value, "42");
    EXPECT_EQ(value, 42);

    std::string result = converter.convert_to_string(123);
    EXPECT_EQ(result, "123");
}

TEST(TypeConverterApi, Numeric_IntUtilities)
{
    int value = 42;
    std::string str = type_converter_api::to_string(value);
    EXPECT_EQ(str, "42");
    
    int parsed = type_converter_api::from_string<int>("123");
    EXPECT_EQ(parsed, 123);
    
    int parsed2;
    type_converter_api::from_string(parsed2, "456");
    EXPECT_EQ(parsed2, 456);
}

TEST(TypeConverterApi, Numeric_IntErrorHandling)
{
    type_converter_api::type_converter<int> converter;
    int value = 0;
    
    EXPECT_THROW(converter.fill_from_string(value, "not_a_number"), std::invalid_argument);
    EXPECT_THROW(converter.fill_from_string(value, "123abc"), std::invalid_argument);
}

TEST(TypeConverterApi, Numeric_FloatConversion)
{
    type_converter_api::type_converter<float> converter;
    
    float value = 0.0f;
    converter.fill_from_string(value, "3.14");
    EXPECT_FLOAT_EQ(value, 3.14f);
    
    std::string result = converter.convert_to_string(2.5f);
    EXPECT_TRUE(result.find("2.5") != std::string::npos);
}

TEST(TypeConverterApi, Numeric_DoubleConversion)
{
    type_converter_api::type_converter<double> converter;
    
    double value = 0.0;
    converter.fill_from_string(value, "3.14159");
    EXPECT_DOUBLE_EQ(value, 3.14159);
    
    std::string result = converter.convert_to_string(2.71828);
    EXPECT_TRUE(result.find("2.71828") != std::string::npos);
}

TEST(TypeConverterApi, Numeric_BoolConversion)
{
    type_converter_api::type_converter<bool> converter;
    
    bool value = false;
    converter.fill_from_string(value, "true");
    EXPECT_TRUE(value);
    
    converter.fill_from_string(value, "false");
    EXPECT_FALSE(value);
    
    converter.fill_from_string(value, "1");
    EXPECT_TRUE(value);
    
    converter.fill_from_string(value, "0");
    EXPECT_FALSE(value);
    
    std::string result = converter.convert_to_string(true);
    EXPECT_EQ(result, "true");
    
    result = converter.convert_to_string(false);
    EXPECT_EQ(result, "false");
}

TEST(TypeConverterApi, Numeric_BoolErrorHandling)
{
    type_converter_api::type_converter<bool> converter;
    bool value = false;
    
    EXPECT_THROW(converter.fill_from_string(value, "maybe"), std::invalid_argument);
    EXPECT_THROW(converter.fill_from_string(value, "2"), std::invalid_argument);
}

TEST(TypeConverterApi, Numeric_OutOfRangeError)
{
    type_converter_api::type_converter<short> converter;
    short value = 0;
    
    // Попытка преобразовать значение, выходящее за границы short
    EXPECT_THROW(converter.fill_from_string(value, "99999"), std::out_of_range);
}

// Тесты для enum (через базовый тип)
enum class TestEnum : int
{
    Value1 = 1,
    Value2 = 2
};

TEST(TypeConverterApi, Numeric_EnumConversion)
{
    type_converter_api::type_converter<TestEnum> converter;
    
    TestEnum value = TestEnum::Value1;
    converter.fill_from_string(value, "2");
    EXPECT_EQ(value, TestEnum::Value2);
    
    std::string result = converter.convert_to_string(TestEnum::Value1);
    EXPECT_EQ(result, "1");
}

