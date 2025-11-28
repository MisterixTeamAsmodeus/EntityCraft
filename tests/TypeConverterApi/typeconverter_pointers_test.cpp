#include <TypeConverterApi/typeconverterapi.hpp>

#include <memory>

#include <gtest/gtest.h>

// Тесты для умных указателей
TEST(TypeConverterApi, Pointers_SharedPtrConversion)
{
    type_converter_api::type_converter<std::shared_ptr<int>> converter;
    
    std::shared_ptr<int> ptr;
    converter.fill_from_string(ptr, "42");
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
    
    std::string result = converter.convert_to_string(ptr);
    EXPECT_EQ(result, "42");
    
    std::shared_ptr<int> null_ptr = nullptr;
    std::string null_result = converter.convert_to_string(null_ptr);
    EXPECT_EQ(null_result, "");
}

TEST(TypeConverterApi, Pointers_UniquePtrConversion)
{
    type_converter_api::type_converter<std::unique_ptr<int>> converter;
    
    std::unique_ptr<int> ptr;
    converter.fill_from_string(ptr, "42");
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
    
    std::string result = converter.convert_to_string(ptr);
    EXPECT_EQ(result, "42");
}

TEST(TypeConverterApi, Pointers_PtrConversion)
{
    type_converter_api::type_converter<int*> converter;
    
    int* ptr = nullptr;
    converter.fill_from_string(ptr, "42");
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
    
    std::string result = converter.convert_to_string(ptr);
    EXPECT_EQ(result, "42");
    delete ptr;
}