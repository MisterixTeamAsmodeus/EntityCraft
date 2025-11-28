#include <TypeConverterApi/typeconverterapi.hpp>

#include <algorithm>
#include <deque>
#include <list>
#include <set>
#include <string>
#include <vector>

#include <gtest/gtest.h>

// Тесты для container_converter

// Тесты конвертации между последовательными контейнерами с одинаковыми типами элементов
TEST(TypeConverterApi, ContainerConverter_VectorToVector)
{
    std::vector<int> source = {1, 2, 3, 4, 5};
    std::vector<int> target;
    
    type_converter_api::container_converter<std::vector<int>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 5);
    EXPECT_EQ(target[0], 1);
    EXPECT_EQ(target[4], 5);
}

TEST(TypeConverterApi, ContainerConverter_VectorToList)
{
    std::vector<int> source = {1, 2, 3};
    std::list<int> target;
    
    type_converter_api::container_converter<std::list<int>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 3);
    auto it = target.begin();
    EXPECT_EQ(*it++, 1);
    EXPECT_EQ(*it++, 2);
    EXPECT_EQ(*it++, 3);
}

TEST(TypeConverterApi, ContainerConverter_VectorToDeque)
{
    std::vector<int> source = {10, 20, 30};
    std::deque<int> target;
    
    type_converter_api::container_converter<std::deque<int>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 3);
    EXPECT_EQ(target[0], 10);
    EXPECT_EQ(target[1], 20);
    EXPECT_EQ(target[2], 30);
}

TEST(TypeConverterApi, ContainerConverter_ListToVector)
{
    std::list<int> source = {5, 6, 7};
    std::vector<int> target;
    
    type_converter_api::container_converter<std::vector<int>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 3);
    EXPECT_EQ(target[0], 5);
    EXPECT_EQ(target[1], 6);
    EXPECT_EQ(target[2], 7);
}

TEST(TypeConverterApi, ContainerConverter_DequeToVector)
{
    std::deque<int> source = {100, 200, 300};
    std::vector<int> target;
    
    type_converter_api::container_converter<std::vector<int>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 3);
    EXPECT_EQ(target[0], 100);
    EXPECT_EQ(target[1], 200);
    EXPECT_EQ(target[2], 300);
}

// Тесты конвертации с изменением типов элементов
TEST(TypeConverterApi, ContainerConverter_VectorIntToVectorLong)
{
    std::vector<int> source = {1, 2, 3, 4, 5};
    std::vector<long> target;
    
    type_converter_api::container_converter<std::vector<long>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 5);
    EXPECT_EQ(target[0], 1L);
    EXPECT_EQ(target[4], 5L);
}

TEST(TypeConverterApi, ContainerConverter_VectorIntToListLong)
{
    std::vector<int> source = {10, 20, 30};
    std::list<long> target;
    
    type_converter_api::container_converter<std::list<long>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 3);
    auto it = target.begin();
    EXPECT_EQ(*it++, 10L);
    EXPECT_EQ(*it++, 20L);
    EXPECT_EQ(*it++, 30L);
}

TEST(TypeConverterApi, ContainerConverter_VectorIntToVectorString)
{
    std::vector<int> source = {42, 100, 999};
    std::vector<std::string> target;
    
    type_converter_api::container_converter<std::vector<std::string>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 3);
    EXPECT_EQ(target[0], "42");
    EXPECT_EQ(target[1], "100");
    EXPECT_EQ(target[2], "999");
}

TEST(TypeConverterApi, ContainerConverter_VectorStringToVectorInt)
{
    std::vector<std::string> source = {"1", "2", "3"};
    std::vector<int> target;
    
    type_converter_api::container_converter<std::vector<int>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 3);
    EXPECT_EQ(target[0], 1);
    EXPECT_EQ(target[1], 2);
    EXPECT_EQ(target[2], 3);
}

// Тесты для ассоциативных контейнеров
TEST(TypeConverterApi, ContainerConverter_VectorToSet)
{
    std::vector<int> source = {3, 1, 2, 1, 3}; // Дубликаты для проверки
    std::set<int> target;
    
    type_converter_api::container_converter<std::set<int>> converter;
    converter.convert_to_target(target, source);
    
    // Set должен содержать уникальные элементы
    EXPECT_EQ(target.size(), 3);
    EXPECT_NE(target.find(1), target.end());
    EXPECT_NE(target.find(2), target.end());
    EXPECT_NE(target.find(3), target.end());
}

TEST(TypeConverterApi, ContainerConverter_ListToSet)
{
    std::list<int> source = {5, 3, 1, 4, 2};
    std::set<int> target;
    
    type_converter_api::container_converter<std::set<int>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 5);
    EXPECT_NE(target.find(1), target.end());
    EXPECT_NE(target.find(5), target.end());
}

// Тесты для метода convert()
TEST(TypeConverterApi, ContainerConverter_ConvertMethod_VectorToList)
{
    std::vector<int> source = {1, 2, 3};
    
    type_converter_api::container_converter<std::list<int>> converter;
    auto target = converter.convert(source);
    
    EXPECT_EQ(target.size(), 3);
    auto it = target.begin();
    EXPECT_EQ(*it++, 1);
    EXPECT_EQ(*it++, 2);
    EXPECT_EQ(*it++, 3);
}

TEST(TypeConverterApi, ContainerConverter_ConvertMethod_VectorIntToVectorLong)
{
    std::vector<int> source = {100, 200, 300};
    
    type_converter_api::container_converter<std::vector<long>> converter;
    auto target = converter.convert(source);
    
    EXPECT_EQ(target.size(), 3);
    EXPECT_EQ(target[0], 100L);
    EXPECT_EQ(target[1], 200L);
    EXPECT_EQ(target[2], 300L);
}

// Тесты для пустых контейнеров
TEST(TypeConverterApi, ContainerConverter_EmptyVectorToVector)
{
    std::vector<int> source;
    std::vector<int> target;
    
    type_converter_api::container_converter<std::vector<int>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_TRUE(target.empty());
}

TEST(TypeConverterApi, ContainerConverter_EmptyVectorToList)
{
    std::vector<int> source;
    std::list<int> target;
    
    type_converter_api::container_converter<std::list<int>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_TRUE(target.empty());
}

TEST(TypeConverterApi, ContainerConverter_EmptyVectorToSet)
{
    std::vector<int> source;
    std::set<int> target;
    
    type_converter_api::container_converter<std::set<int>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_TRUE(target.empty());
}

// Тесты для проверки очистки целевого контейнера
TEST(TypeConverterApi, ContainerConverter_TargetContainerCleared)
{
    std::vector<int> source1 = {1, 2, 3};
    std::vector<int> source2 = {4, 5};
    std::vector<int> target = {99, 98, 97}; // Предзаполненный контейнер
    
    type_converter_api::container_converter<std::vector<int>> converter;
    converter.convert_to_target(target, source1);
    
    EXPECT_EQ(target.size(), 3);
    EXPECT_EQ(target[0], 1);
    
    converter.convert_to_target(target, source2);
    
    EXPECT_EQ(target.size(), 2);
    EXPECT_EQ(target[0], 4);
    EXPECT_EQ(target[1], 5);
}

// Тесты для больших контейнеров
TEST(TypeConverterApi, ContainerConverter_LargeContainer)
{
    std::vector<int> source;
    for(int i = 0; i < 1000; ++i)
    {
        source.push_back(i);
    }
    
    std::vector<int> target;
    type_converter_api::container_converter<std::vector<int>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 1000);
    EXPECT_EQ(target[0], 0);
    EXPECT_EQ(target[999], 999);
}

// Тесты для строковых контейнеров
TEST(TypeConverterApi, ContainerConverter_VectorStringToListString)
{
    std::vector<std::string> source = {"hello", "world", "test"};
    std::list<std::string> target;
    
    type_converter_api::container_converter<std::list<std::string>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 3);
    auto it = target.begin();
    EXPECT_EQ(*it++, "hello");
    EXPECT_EQ(*it++, "world");
    EXPECT_EQ(*it++, "test");
}

// Тесты для конвертации с разными числовыми типами
TEST(TypeConverterApi, ContainerConverter_VectorIntToVectorDouble)
{
    std::vector<int> source = {1, 2, 3};
    std::vector<double> target;
    
    type_converter_api::container_converter<std::vector<double>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 3);
    EXPECT_DOUBLE_EQ(target[0], 1.0);
    EXPECT_DOUBLE_EQ(target[1], 2.0);
    EXPECT_DOUBLE_EQ(target[2], 3.0);
}

TEST(TypeConverterApi, ContainerConverter_VectorDoubleToVectorInt)
{
    std::vector<double> source = {1.5, 2.7, 3.9};
    std::vector<int> target;
    
    type_converter_api::container_converter<std::vector<int>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 3);
    EXPECT_EQ(target[0], 1); // Округление вниз
    EXPECT_EQ(target[1], 2);
    EXPECT_EQ(target[2], 3);
}

// Тест для проверки работы с различными исходными контейнерами
TEST(TypeConverterApi, ContainerConverter_SetToVector)
{
    std::set<int> source = {3, 1, 2};
    std::vector<int> target;
    
    type_converter_api::container_converter<std::vector<int>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 3);
    // Порядок может быть разным, но все элементы должны присутствовать
    EXPECT_NE(std::find(target.begin(), target.end(), 1), target.end());
    EXPECT_NE(std::find(target.begin(), target.end(), 2), target.end());
    EXPECT_NE(std::find(target.begin(), target.end(), 3), target.end());
}

// Тесты для граничных случаев
TEST(TypeConverterApi, ContainerConverter_SingleElement)
{
    std::vector<int> source = {42};
    std::list<int> target;
    
    type_converter_api::container_converter<std::list<int>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 1);
    EXPECT_EQ(*target.begin(), 42);
}

TEST(TypeConverterApi, ContainerConverter_NegativeNumbers)
{
    std::vector<int> source = {-1, -2, -3, 0, 1, 2};
    std::vector<int> target;
    
    type_converter_api::container_converter<std::vector<int>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 6);
    EXPECT_EQ(target[0], -1);
    EXPECT_EQ(target[3], 0);
    EXPECT_EQ(target[5], 2);
}

TEST(TypeConverterApi, ContainerConverter_NegativeNumbersTypeConversion)
{
    std::vector<int> source = {-10, -20, 30};
    std::vector<long> target;
    
    type_converter_api::container_converter<std::vector<long>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 3);
    EXPECT_EQ(target[0], -10L);
    EXPECT_EQ(target[1], -20L);
    EXPECT_EQ(target[2], 30L);
}

// Тесты для различных комбинаций исходных контейнеров
TEST(TypeConverterApi, ContainerConverter_DequeToList)
{
    std::deque<int> source = {7, 8, 9};
    std::list<int> target;
    
    type_converter_api::container_converter<std::list<int>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 3);
    auto it = target.begin();
    EXPECT_EQ(*it++, 7);
    EXPECT_EQ(*it++, 8);
    EXPECT_EQ(*it++, 9);
}

TEST(TypeConverterApi, ContainerConverter_ListToDeque)
{
    std::list<int> source = {11, 12, 13};
    std::deque<int> target;
    
    type_converter_api::container_converter<std::deque<int>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 3);
    EXPECT_EQ(target[0], 11);
    EXPECT_EQ(target[1], 12);
    EXPECT_EQ(target[2], 13);
}

TEST(TypeConverterApi, ContainerConverter_DequeToSet)
{
    std::deque<int> source = {5, 3, 5, 1, 3}; // С дубликатами
    std::set<int> target;
    
    type_converter_api::container_converter<std::set<int>> converter;
    converter.convert_to_target(target, source);
    
    EXPECT_EQ(target.size(), 3); // Дубликаты должны быть удалены
    EXPECT_NE(target.find(1), target.end());
    EXPECT_NE(target.find(3), target.end());
    EXPECT_NE(target.find(5), target.end());
}

// Тест для проверки конвертации с изменением типа через метод convert()
TEST(TypeConverterApi, ContainerConverter_ConvertMethod_ListToVector)
{
    std::list<int> source = {50, 60, 70};
    
    type_converter_api::container_converter<std::vector<int>> converter;
    auto target = converter.convert(source);
    
    EXPECT_EQ(target.size(), 3);
    EXPECT_EQ(target[0], 50);
    EXPECT_EQ(target[1], 60);
    EXPECT_EQ(target[2], 70);
}

TEST(TypeConverterApi, ContainerConverter_ConvertMethod_DequeToVector)
{
    std::deque<int> source = {100, 200};
    
    type_converter_api::container_converter<std::vector<int>> converter;
    auto target = converter.convert(source);
    
    EXPECT_EQ(target.size(), 2);
    EXPECT_EQ(target[0], 100);
    EXPECT_EQ(target[1], 200);
}

// Тест для проверки конвертации с изменением типа через метод convert()
TEST(TypeConverterApi, ContainerConverter_ConvertMethod_TypeConversion)
{
    std::vector<int> source = {1000, 2000, 3000};
    
    type_converter_api::container_converter<std::list<long>> converter;
    auto target = converter.convert(source);
    
    EXPECT_EQ(target.size(), 3);
    auto it = target.begin();
    EXPECT_EQ(*it++, 1000L);
    EXPECT_EQ(*it++, 2000L);
    EXPECT_EQ(*it++, 3000L);
}

