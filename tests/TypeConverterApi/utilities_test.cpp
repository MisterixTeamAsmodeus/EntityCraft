#include <TypeConverterApi/typeconverterapi.hpp>

#include <string>
#include <vector>

#include <gtest/gtest.h>

// Тесты для утилит
TEST(TypeConverterApi, Utilities_ContainerToStringUtility)
{
    std::vector<int> vec = {1, 2, 3};
    std::string result = type_converter_api::container_to_string(vec);
    EXPECT_EQ(result, "1,2,3");
    
    std::string result2 = type_converter_api::container_to_string(vec, "|");
    EXPECT_EQ(result2, "1|2|3");
}

TEST(TypeConverterApi, Utilities_ContainerFromStringUtility)
{
    std::vector<int> vec;
    type_converter_api::container_from_string(vec, "1,2,3");
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[2], 3);
}

TEST(TypeConverterApi, Utilities_StringConvertUtility)
{
    std::string str = "Hello";
    std::wstring wstr = type_converter_api::string_convert<std::wstring>(str);
    EXPECT_FALSE(wstr.empty());
}

