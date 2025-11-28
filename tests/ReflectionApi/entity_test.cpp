#include <ReflectionApi/reflectionapi.hpp>
#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

// Тестовые классы
struct SimpleStruct
{
    int value = 0;
    std::string name;
};

class TestClass
{
public:
    void setValue(const int v) { value = v; }
    int getValue() const { return value; }

private:
    int value = 0;
};

// Тесты для entity
TEST(ReflectionApi, Entity_CreateEntity)
{
    const auto entity = reflection_api::make_entity<SimpleStruct>(
        reflection_api::make_property("value", &SimpleStruct::value),
        reflection_api::make_property("name", &SimpleStruct::name));

    EXPECT_EQ(entity.property_count(), 2);
}

TEST(ReflectionApi, Entity_EmptyEntity)
{
    auto entity = reflection_api::make_entity<SimpleStruct>(
        reflection_api::make_property("value", &SimpleStruct::value));

    const auto obj = entity.empty_entity();
    EXPECT_EQ(obj.value, 0);
}

TEST(ReflectionApi, Entity_SetPropertyValue)
{
    const auto entity = reflection_api::make_entity<SimpleStruct>(
        reflection_api::make_property("value", &SimpleStruct::value),
        reflection_api::make_property("name", &SimpleStruct::name));

    SimpleStruct obj;
    
    entity.set_property_value(obj, 42, "value");
    EXPECT_EQ(obj.value, 42);

    std::string value = "test";
    entity.set_property_value(obj, value, "name");
    EXPECT_EQ(obj.name, "test");
}

TEST(ReflectionApi, Entity_GetPropertyValue)
{
    auto entity = reflection_api::make_entity<SimpleStruct>(
        reflection_api::make_property("value", &SimpleStruct::value),
        reflection_api::make_property("name", &SimpleStruct::name));

    SimpleStruct obj;
    obj.value = 42;
    obj.name = "test";

    int value = 0;
    entity.get_property_value(obj, value, "value");
    EXPECT_EQ(value, obj.value);

    std::string name;
    entity.get_property_value(obj, name, "name");
    EXPECT_EQ(name, obj.name);
}

TEST(ReflectionApi, Entity_GetPropertyValueConst)
{
    const auto entity = reflection_api::make_entity<SimpleStruct>(
        reflection_api::make_property("value", &SimpleStruct::value));

    SimpleStruct obj;
    obj.value = 100;

    int value = 0;
    entity.get_property_value(obj, value, "value");
    EXPECT_EQ(value, obj.value);
}

TEST(ReflectionApi, Entity_ForEach)
{
    auto entity = reflection_api::make_entity<SimpleStruct>(
        reflection_api::make_property("value", &SimpleStruct::value),
        reflection_api::make_property("name", &SimpleStruct::name));

    int count = 0;
    entity.for_each([&count](const auto& prop) {
        ++count;
        EXPECT_FALSE(prop.name().empty());
    });

    EXPECT_EQ(count, 2);
}

TEST(ReflectionApi, Entity_ForEachConst)
{
    const auto entity = reflection_api::make_entity<SimpleStruct>(
        reflection_api::make_property("value", &SimpleStruct::value));

    int count = 0;
    entity.for_each([&count](const auto&) {
        ++count;
    });

    EXPECT_EQ(count, 1);
}

TEST(ReflectionApi, Entity_PropertyCount)
{
    auto entity1 = reflection_api::make_entity<SimpleStruct>(
        reflection_api::make_property("value", &SimpleStruct::value));
    EXPECT_EQ(entity1.property_count(), 1);

    auto entity2 = reflection_api::make_entity<SimpleStruct>(
        reflection_api::make_property("value", &SimpleStruct::value),
        reflection_api::make_property("name", &SimpleStruct::name));
    EXPECT_EQ(entity2.property_count(), 2);
}

TEST(ReflectionApi, Entity_HasProperty)
{
    auto entity = reflection_api::make_entity<SimpleStruct>(
        reflection_api::make_property("value", &SimpleStruct::value),
        reflection_api::make_property("name", &SimpleStruct::name));

    EXPECT_TRUE(entity.has_property("value"));
    EXPECT_TRUE(entity.has_property("name"));
    EXPECT_FALSE(entity.has_property("nonexistent"));
}

TEST(ReflectionApi, Entity_WithGetterSetter)
{
    auto entity = reflection_api::make_entity<TestClass>(
        reflection_api::make_property("value", &TestClass::setValue, &TestClass::getValue));

    TestClass obj;
    entity.set_property_value(obj, 99, "value");
    int value = 0;
    entity.get_property_value(obj, value, "value");
    EXPECT_EQ(value, 99);
}

TEST(ReflectionApi, Entity_SetPropertyValueWithRvalue)
{
    auto entity = reflection_api::make_entity<SimpleStruct>(
        reflection_api::make_property("value", &SimpleStruct::value),
        reflection_api::make_property("name", &SimpleStruct::name));

    SimpleStruct obj;
    
    entity.set_property_value(obj, 100, "value");
    EXPECT_EQ(obj.value, 100);

    entity.set_property_value(obj, std::string("rvalue_test"), "name");
    EXPECT_EQ(obj.name, "rvalue_test");
}

TEST(ReflectionApi, Entity_EmptyName)
{
    auto entity = reflection_api::make_entity<SimpleStruct>(
        reflection_api::make_property("", &SimpleStruct::value));

    EXPECT_TRUE(entity.has_property(""));
    EXPECT_EQ(entity.property_count(), 1);

    SimpleStruct obj;
    entity.set_property_value(obj, 42, "");
    EXPECT_EQ(obj.value, 42);
}

TEST(ReflectionApi, Entity_MultiplePropertiesSameType)
{
    struct MultiIntStruct
    {
        int a = 0;
        int b = 0;
        int c = 0;
    };

    auto entity = reflection_api::make_entity<MultiIntStruct>(
        reflection_api::make_property("a", &MultiIntStruct::a),
        reflection_api::make_property("b", &MultiIntStruct::b),
        reflection_api::make_property("c", &MultiIntStruct::c));

    EXPECT_EQ(entity.property_count(), 3);

    MultiIntStruct obj;
    entity.set_property_value(obj, 1, "a");
    entity.set_property_value(obj, 2, "b");
    entity.set_property_value(obj, 3, "c");

    EXPECT_EQ(obj.a, 1);
    EXPECT_EQ(obj.b, 2);
    EXPECT_EQ(obj.c, 3);

    int value = 0;
    entity.get_property_value(obj, value, "a");
    EXPECT_EQ(value, 1);
    entity.get_property_value(obj, value, "b");
    EXPECT_EQ(value, 2);
    entity.get_property_value(obj, value, "c");
    EXPECT_EQ(value, 3);
}

TEST(ReflectionApi, Entity_ForEachWithPropertyNames)
{
    auto entity = reflection_api::make_entity<SimpleStruct>(
        reflection_api::make_property("value", &SimpleStruct::value),
        reflection_api::make_property("name", &SimpleStruct::name));

    std::vector<std::string> names;
    entity.for_each([&names](const auto& prop) {
        names.push_back(prop.name());
    });

    EXPECT_EQ(names.size(), 2);
    EXPECT_TRUE(std::find(names.begin(), names.end(), "value") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "name") != names.end());
}

TEST(ReflectionApi, Entity_SetPropertyValueNonExistent)
{
    auto entity = reflection_api::make_entity<SimpleStruct>(
        reflection_api::make_property("value", &SimpleStruct::value));

    SimpleStruct obj;
    obj.value = 10;
    
    // Setting non-existent property should not change anything
    entity.set_property_value(obj, 999, "nonexistent");
    
    // Original value should remain unchanged
    int value = 0;
    entity.get_property_value(obj, value, "value");
    EXPECT_EQ(value, 10);
}

TEST(ReflectionApi, Entity_GetPropertyValueNonExistent)
{
    auto entity = reflection_api::make_entity<SimpleStruct>(
        reflection_api::make_property("value", &SimpleStruct::value));

    SimpleStruct obj;
    obj.value = 20;
    
    int value = 100;
    // Getting non-existent property should not modify target
    entity.get_property_value(obj, value, "nonexistent");
    
    // Value should remain unchanged
    EXPECT_EQ(value, 100);
}

TEST(ReflectionApi, Entity_WithZeroProperties)
{
    struct EmptyStruct {};
    
    auto entity = reflection_api::make_entity<EmptyStruct>();
    
    EXPECT_EQ(entity.property_count(), 0);
    EXPECT_FALSE(entity.has_property("any"));
    
    int count = 0;
    entity.for_each([&count](const auto&) { ++count; });
    EXPECT_EQ(count, 0);
}

TEST(ReflectionApi, Entity_StaticEmptyEntity)
{
    auto entity = reflection_api::make_entity<SimpleStruct>(
        reflection_api::make_property("value", &SimpleStruct::value));

    auto obj = entity.empty_entity();
    EXPECT_EQ(obj.value, 0);
    EXPECT_TRUE(obj.name.empty());
}

TEST(ReflectionApi, Entity_GetPropertyNames)
{
    auto entity = reflection_api::make_entity<SimpleStruct>(
        reflection_api::make_property("value", &SimpleStruct::value),
        reflection_api::make_property("name", &SimpleStruct::name));

    auto names = entity.get_property_names();
    EXPECT_EQ(names.size(), 2);
    EXPECT_TRUE(std::find(names.begin(), names.end(), "value") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "name") != names.end());
}
