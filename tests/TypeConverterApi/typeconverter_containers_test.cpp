#include <TypeConverterApi/typeconverterapi.hpp>

#include <array>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#if __cplusplus >= 201703L
#include <optional>
#include <string_view>
#include <variant>
#include <any>
#endif

#include <gtest/gtest.h>

// Тесты для контейнеров
TEST(TypeConverterApi, Containers_VectorConversion)
{
    type_converter_api::type_converter<std::vector<int>> converter;
    
    std::vector<int> vec;
    converter.fill_from_string(vec, "1,2,3,4,5");
    EXPECT_EQ(vec.size(), 5);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[4], 5);
    
    std::string result = converter.convert_to_string(vec);
    EXPECT_EQ(result, "1,2,3,4,5");
}

TEST(TypeConverterApi, Containers_ArrayConversion)
{
    type_converter_api::type_converter<std::array<int, 3>> converter;
    
    std::array<int, 3> arr;
    converter.fill_from_string(arr, "10,20,30");
    EXPECT_EQ(arr[0], 10);
    EXPECT_EQ(arr[1], 20);
    EXPECT_EQ(arr[2], 30);
    
    std::string result = converter.convert_to_string(arr);
    EXPECT_EQ(result, "10,20,30");
}

TEST(TypeConverterApi, Containers_DequeConversion)
{
    type_converter_api::type_converter<std::deque<int>> converter;
    
    std::deque<int> deq;
    converter.fill_from_string(deq, "1,2,3");
    EXPECT_EQ(deq.size(), 3);
    EXPECT_EQ(deq[0], 1);
    
    std::string result = converter.convert_to_string(deq);
    EXPECT_EQ(result, "1,2,3");
}

TEST(TypeConverterApi, Containers_ListConversion)
{
    type_converter_api::type_converter<std::list<int>> converter;
    
    std::list<int> lst;
    converter.fill_from_string(lst, "1,2,3");
    EXPECT_EQ(lst.size(), 3);
    
    std::string result = converter.convert_to_string(lst);
    EXPECT_EQ(result, "1,2,3");
}

TEST(TypeConverterApi, Containers_SetConversion)
{
    type_converter_api::type_converter<std::set<int>> converter;
    
    std::set<int> s;
    converter.fill_from_string(s, "3,1,2");
    EXPECT_EQ(s.size(), 3);
    
    std::string result = converter.convert_to_string(s);
    // Порядок может быть отсортирован
    EXPECT_TRUE(result.find("1") != std::string::npos);
    EXPECT_TRUE(result.find("2") != std::string::npos);
    EXPECT_TRUE(result.find("3") != std::string::npos);
}

TEST(TypeConverterApi, Containers_MapConversion)
{
    type_converter_api::type_converter<std::map<std::string, int>> converter;
    
    std::map<std::string, int> m;
    converter.fill_from_string(m, "one:1,two:2,three:3");
    EXPECT_EQ(m.size(), 3);
    EXPECT_EQ(m["one"], 1);
    EXPECT_EQ(m["two"], 2);
    EXPECT_EQ(m["three"], 3);
    
    std::string result = converter.convert_to_string(m);
    EXPECT_TRUE(result.find("one:1") != std::string::npos);
    EXPECT_TRUE(result.find("two:2") != std::string::npos);
}

TEST(TypeConverterApi, Containers_UnorderedMapConversion)
{
    type_converter_api::type_converter<std::unordered_map<std::string, int>> converter;
    
    std::unordered_map<std::string, int> m;
    converter.fill_from_string(m, "a:1,b:2");
    EXPECT_EQ(m.size(), 2);
    EXPECT_EQ(m["a"], 1);
    EXPECT_EQ(m["b"], 2);
}

TEST(TypeConverterApi, Containers_PairConversion)
{
    type_converter_api::type_converter<std::pair<int, std::string>> converter;
    
    std::pair<int, std::string> p;
    converter.fill_from_string(p, "42:answer");
    EXPECT_EQ(p.first, 42);
    EXPECT_EQ(p.second, "answer");
    
    std::string result = converter.convert_to_string(p);
    EXPECT_EQ(result, "42:answer");
}

TEST(TypeConverterApi, Containers_EmptyVector)
{
    type_converter_api::type_converter<std::vector<int>> converter;
    
    std::vector<int> vec;
    converter.fill_from_string(vec, "");
    EXPECT_TRUE(vec.empty());
    
    std::vector<int> empty_vec;
    std::string result = converter.convert_to_string(empty_vec);
    EXPECT_TRUE(result.empty());
}

TEST(TypeConverterApi, Containers_InvalidFormatError)
{
    type_converter_api::type_converter<std::pair<int, int>> converter;
    std::pair<int, int> p;
    
    EXPECT_THROW(converter.fill_from_string(p, "invalid"), std::invalid_argument);
}

TEST(TypeConverterApi, Containers_MapInvalidFormatError)
{
    type_converter_api::type_converter<std::map<std::string, int>> converter;
    std::map<std::string, int> m;
    
    EXPECT_THROW(converter.fill_from_string(m, "invalid_format"), std::invalid_argument);
}

#if __cplusplus >= 201703L

// Тесты для C++17 типов

TEST(TypeConverterApi, Containers_Optional_WithValue)
{
    type_converter_api::type_converter<std::optional<int>> converter;
    
    std::optional<int> opt;
    converter.fill_from_string(opt, "42");
    EXPECT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, 42);
    
    std::string result = converter.convert_to_string(opt);
    EXPECT_EQ(result, "42");
}

TEST(TypeConverterApi, Containers_Optional_Null)
{
    type_converter_api::type_converter<std::optional<int>> converter;
    
    std::optional<int> opt;
    converter.fill_from_string(opt, "null");
    EXPECT_FALSE(opt.has_value());
    
    std::string result = converter.convert_to_string(opt);
    EXPECT_EQ(result, "null");
}

TEST(TypeConverterApi, Containers_Optional_Nullopt)
{
    type_converter_api::type_converter<std::optional<int>> converter;
    
    std::optional<int> opt;
    converter.fill_from_string(opt, "nullopt");
    EXPECT_FALSE(opt.has_value());
}

TEST(TypeConverterApi, Containers_Optional_None)
{
    type_converter_api::type_converter<std::optional<int>> converter;
    
    std::optional<int> opt;
    converter.fill_from_string(opt, "none");
    EXPECT_FALSE(opt.has_value());
}

TEST(TypeConverterApi, Containers_Optional_EmptyString)
{
    type_converter_api::type_converter<std::optional<int>> converter;
    
    std::optional<int> opt;
    converter.fill_from_string(opt, "");
    EXPECT_FALSE(opt.has_value());
}

TEST(TypeConverterApi, Containers_Optional_StringType)
{
    type_converter_api::type_converter<std::optional<std::string>> converter;
    
    std::optional<std::string> opt;
    converter.fill_from_string(opt, "hello");
    EXPECT_TRUE(opt.has_value());
    EXPECT_EQ(*opt, "hello");
    
    std::string result = converter.convert_to_string(opt);
    EXPECT_EQ(result, "hello");
}

TEST(TypeConverterApi, Containers_Optional_StringTypeNull)
{
    type_converter_api::type_converter<std::optional<std::string>> converter;
    
    std::optional<std::string> opt = std::make_optional<std::string>("test");
    std::string result = converter.convert_to_string(opt);
    EXPECT_EQ(result, "test");
    
    opt = std::nullopt;
    result = converter.convert_to_string(opt);
    EXPECT_EQ(result, "null");
}

TEST(TypeConverterApi, Containers_StringView_ConvertToString)
{
    type_converter_api::type_converter<std::string_view> converter;
    
    std::string_view sv = "test_string";
    std::string result = converter.convert_to_string(sv);
    EXPECT_EQ(result, "test_string");
}

TEST(TypeConverterApi, Containers_StringView_FillFromStringThrows)
{
    type_converter_api::type_converter<std::string_view> converter;
    
    std::string_view sv;
    EXPECT_THROW(converter.fill_from_string(sv, "test"), std::runtime_error);
}

TEST(TypeConverterApi, Containers_Variant_TwoTypes_WithIndex)
{
    type_converter_api::type_converter<std::variant<int, std::string>> converter;
    
    std::variant<int, std::string> var;
    converter.fill_from_string(var, "0:42");
    EXPECT_EQ(var.index(), 0);
    EXPECT_EQ(std::get<0>(var), 42);
    
    std::string result = converter.convert_to_string(var);
    EXPECT_EQ(result, "0:42");
}

TEST(TypeConverterApi, Containers_Variant_TwoTypes_String)
{
    type_converter_api::type_converter<std::variant<int, std::string>> converter;
    
    std::variant<int, std::string> var;
    converter.fill_from_string(var, "1:hello");
    EXPECT_EQ(var.index(), 1);
    EXPECT_EQ(std::get<1>(var), "hello");
    
    std::string result = converter.convert_to_string(var);
    EXPECT_EQ(result, "1:hello");
}

TEST(TypeConverterApi, Containers_Variant_TwoTypes_AutoDetect)
{
    type_converter_api::type_converter<std::variant<int, std::string>> converter;
    
    std::variant<int, std::string> var;
    // Пробуем сначала int
    converter.fill_from_string(var, "123");
    EXPECT_EQ(var.index(), 0);
    EXPECT_EQ(std::get<0>(var), 123);
    
    // Пробуем строку (не число)
    converter.fill_from_string(var, "abc");
    EXPECT_EQ(var.index(), 1);
    EXPECT_EQ(std::get<1>(var), "abc");
}

TEST(TypeConverterApi, Containers_Variant_ThreeTypes)
{
    type_converter_api::type_converter<std::variant<int, double, std::string>> converter;
    
    std::variant<int, double, std::string> var;
    converter.fill_from_string(var, "0:42");
    EXPECT_EQ(var.index(), 0);
    EXPECT_EQ(std::get<0>(var), 42);
    
    converter.fill_from_string(var, "1:3.14");
    EXPECT_EQ(var.index(), 1);
    EXPECT_DOUBLE_EQ(std::get<1>(var), 3.14);
    
    converter.fill_from_string(var, "2:test");
    EXPECT_EQ(var.index(), 2);
    EXPECT_EQ(std::get<2>(var), "test");
}

TEST(TypeConverterApi, Containers_Variant_InvalidIndex)
{
    type_converter_api::type_converter<std::variant<int, std::string>> converter;
    
    std::variant<int, std::string> var;
    EXPECT_THROW(converter.fill_from_string(var, "2:invalid"), std::invalid_argument);
}

TEST(TypeConverterApi, Containers_Optional_Vector)
{
    type_converter_api::type_converter<std::optional<std::vector<int>>> converter;
    
    std::optional<std::vector<int>> opt;
    converter.fill_from_string(opt, "1,2,3");
    EXPECT_TRUE(opt.has_value());
    EXPECT_EQ(opt->size(), 3);
    EXPECT_EQ((*opt)[0], 1);
    EXPECT_EQ((*opt)[2], 3);
    
    std::string result = converter.convert_to_string(opt);
    EXPECT_EQ(result, "1,2,3");
}

TEST(TypeConverterApi, Containers_Optional_VectorNull)
{
    type_converter_api::type_converter<std::optional<std::vector<int>>> converter;
    
    std::optional<std::vector<int>> opt = std::nullopt;
    std::string result = converter.convert_to_string(opt);
    EXPECT_EQ(result, "null");
}

#endif // __cplusplus >= 201703L

