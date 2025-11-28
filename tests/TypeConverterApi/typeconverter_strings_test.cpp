#include <TypeConverterApi/typeconverterapi.hpp>

#include <string>

#include <gtest/gtest.h>

TEST(TypeConverterApi, String_StringConversion)
{
    type_converter_api::type_converter<std::string> converter;
    
    std::string value;
    converter.fill_from_string(value, "Hello, World!");
    EXPECT_EQ(value, "Hello, World!");
    
    std::string result = converter.convert_to_string("test");
    EXPECT_EQ(result, "test");
}

TEST(TypeConverterApi, String_EmptyString)
{
    type_converter_api::type_converter<std::string> converter;
    
    std::string str;
    converter.fill_from_string(str, "");
    EXPECT_TRUE(str.empty());
}

TEST(TypeConverterApi, String_WStringConversion)
{
    type_converter_api::type_converter<std::wstring> converter;
    
    std::wstring wstr;
    converter.fill_from_string(wstr, "Hello");
    EXPECT_FALSE(wstr.empty());
    
    std::string result = converter.convert_to_string(wstr);
    EXPECT_FALSE(result.empty());
}

TEST(TypeConverterApi, String_u16StringConversion)
{
    type_converter_api::type_converter<std::u16string> converter;
    
    std::u16string u16str;
    converter.fill_from_string(u16str, "Hello");
    EXPECT_FALSE(u16str.empty());
    
    std::string result = converter.convert_to_string(u16str);
    EXPECT_FALSE(result.empty());
}

TEST(TypeConverterApi, String_u32StringConversion) {
    type_converter_api::type_converter<std::u32string> converter;
    
    std::u32string u32str;
    converter.fill_from_string(u32str, "Hello");
    EXPECT_FALSE(u32str.empty());
    
    std::string result = converter.convert_to_string(u32str);
    EXPECT_FALSE(result.empty());
}