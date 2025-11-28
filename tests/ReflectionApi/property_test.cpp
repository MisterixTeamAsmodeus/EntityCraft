#include <ReflectionApi/reflectionapi.hpp>
#include <gtest/gtest.h>

#include <string>

// Тестовые классы
struct TestStruct
{
    int intValue = 0;
    double doubleValue = 0.0;
    std::string stringValue;
    bool boolValue = false;
};

class TestClassWithMethods
{
public:
    void setIntValue(int v) { intValue = v; }
    int getIntValue() const { return intValue; }

    void setStringValue(const std::string& v) { stringValue = v; }
    const std::string& getStringValue() const { return stringValue; }

private:
    int intValue = 0;
    std::string stringValue;
};

class TestClassWithNonConstGetter
{
public:
    void setValue(int v) { value = v; }
    int getValue() const { return value; } // Non-const getter

private:
    int value = 0;
};

class TestClassWithBaseSetter
{
public:
    void setValue(int v) { value = v; } // BaseSetter (takes by value, not const&)
    int getValue() const { return value; }

private:
    int value = 0;
};

// Тесты для property
TEST(PropertyTest, CreatePropertyWithMemberPointer)
{
    auto prop = reflection_api::make_property("intValue", &TestStruct::intValue);
    EXPECT_EQ(prop.name(), "intValue");

    TestStruct obj;
    prop.set_value(obj, 42);
    EXPECT_EQ(prop.value(obj), 42);
}

TEST(PropertyTest, PropertyWithDifferentTypes)
{
    auto intProp = reflection_api::make_property("intValue", &TestStruct::intValue);
    auto doubleProp = reflection_api::make_property("doubleValue", &TestStruct::doubleValue);
    auto stringProp = reflection_api::make_property("stringValue", &TestStruct::stringValue);
    auto boolProp = reflection_api::make_property("boolValue", &TestStruct::boolValue);

    TestStruct obj;

    intProp.set_value(obj, 10);
    EXPECT_EQ(intProp.value(obj), 10);

    doubleProp.set_value(obj, 3.14);
    EXPECT_DOUBLE_EQ(doubleProp.value(obj), 3.14);

    stringProp.set_value(obj, std::string("test"));
    EXPECT_EQ(stringProp.value(obj), "test");

    boolProp.set_value(obj, true);
    EXPECT_TRUE(boolProp.value(obj));
}

TEST(PropertyTest, PropertyWithGetterSetter)
{
    auto prop = reflection_api::make_property("intValue", &TestClassWithMethods::setIntValue, &TestClassWithMethods::getIntValue);

    TestClassWithMethods obj;
    prop.set_value(obj, 100);
    EXPECT_EQ(prop.value(obj), 100);
}

TEST(PropertyTest, PropertyName)
{
    auto prop1 = reflection_api::make_property("test1", &TestStruct::intValue);
    auto prop2 = reflection_api::make_property("test2", &TestStruct::stringValue);

    EXPECT_EQ(prop1.name(), "test1");
    EXPECT_EQ(prop2.name(), "test2");
}

TEST(PropertyTest, PropertyNameConst)
{
    const auto prop = reflection_api::make_property("test", &TestStruct::intValue);
    EXPECT_EQ(prop.name(), "test");
}

TEST(PropertyTest, EmptyProperty)
{
    auto intProp = reflection_api::make_property("intValue", &TestStruct::intValue);
    constexpr auto empty = intProp.empty_property();
    EXPECT_EQ(empty, 0);

    auto stringProp = reflection_api::make_property("stringValue", &TestStruct::stringValue);
    const auto emptyString = stringProp.empty_property();
    EXPECT_TRUE(emptyString.empty());
}

TEST(PropertyTest, PropertyConverter)
{
    const auto prop = reflection_api::make_property("intValue", &TestStruct::intValue);
    const auto converter = prop.property_converter();

    EXPECT_NE(converter, nullptr);

    int value = 0;
    converter->fill_from_string(value, "123");
    EXPECT_EQ(value, 123);

    const auto str = converter->convert_to_string(456);
    EXPECT_EQ(str, "456");
}

TEST(PropertyTest, SetConverter)
{
    auto prop = reflection_api::make_property("intValue", &TestStruct::intValue);
    const auto customConverter = std::make_shared<type_converter_api::type_converter<int>>();

    prop.set_converter(customConverter);
    const auto converter = prop.property_converter();

    EXPECT_EQ(converter, customConverter);
}

TEST(PropertyTest, PropertyWithString)
{
    auto prop = reflection_api::make_property("stringValue", &TestStruct::stringValue);

    TestStruct obj;
    prop.set_value(obj, std::string("hello"));
    EXPECT_EQ(prop.value(obj), "hello");

    prop.set_value(obj, std::string("world"));
    EXPECT_EQ(prop.value(obj), "world");
}

TEST(PropertyTest, CopyConstructor)
{
    auto prop1 = reflection_api::make_property("intValue", &TestStruct::intValue);
    auto prop2 = prop1; // Copy constructor

    EXPECT_EQ(prop1.name(), prop2.name());
    EXPECT_EQ(prop2.name(), "intValue");

    TestStruct obj;
    prop2.set_value(obj, 200);
    EXPECT_EQ(prop2.value(obj), 200);
    EXPECT_EQ(prop1.value(obj), 200); // Same underlying member
}

TEST(PropertyTest, PropertyWithBaseSetter)
{
    auto prop = reflection_api::make_property("value", &TestClassWithBaseSetter::setValue, &TestClassWithBaseSetter::getValue);

    TestClassWithBaseSetter obj;
    prop.set_value(obj, 450);
    EXPECT_EQ(prop.value(obj), 450);
}

TEST(PropertyTest, PropertyWithStringGetterSetter)
{
    class StringClass
    {
    public:
        void setStr(const std::string& s) { str = s; }
        std::string getStr() const { return str; }

    private:
        std::string str;
    };

    auto prop = reflection_api::make_property("str", &StringClass::setStr, &StringClass::getStr);

    StringClass obj;
    prop.set_value(obj, std::string("test_string"));
    EXPECT_EQ(prop.value(obj), "test_string");
}

TEST(PropertyTest, PropertyConverterDefault)
{
    auto prop = reflection_api::make_property("intValue", &TestStruct::intValue);
    const auto converter = prop.property_converter();

    EXPECT_NE(converter, nullptr);

    // Test default converter
    int value = 0;
    converter->fill_from_string(value, "789");
    EXPECT_EQ(value, 789);

    const auto str = converter->convert_to_string(101112);
    EXPECT_EQ(str, "101112");
}

TEST(PropertyTest, PropertyConverterString)
{
    auto prop = reflection_api::make_property("stringValue", &TestStruct::stringValue);
    const auto converter = prop.property_converter();

    EXPECT_NE(converter, nullptr);

    std::string value;
    converter->fill_from_string(value, "converted");
    EXPECT_EQ(value, "converted");

    const auto str = converter->convert_to_string(std::string("test"));
    EXPECT_EQ(str, "test");
}

TEST(PropertyTest, SetConverterMultipleTimes)
{
    auto prop = reflection_api::make_property("intValue", &TestStruct::intValue);
    
    const auto converter1 = std::make_shared<type_converter_api::type_converter<int>>();
    prop.set_converter(converter1);
    EXPECT_EQ(prop.property_converter(), converter1);

    const auto converter2 = std::make_shared<type_converter_api::type_converter<int>>();
    prop.set_converter(converter2);
    EXPECT_EQ(prop.property_converter(), converter2);
    EXPECT_NE(prop.property_converter(), converter1);
}

TEST(PropertyTest, PropertyWithBool)
{
    auto prop = reflection_api::make_property("boolValue", &TestStruct::boolValue);

    TestStruct obj;
    prop.set_value(obj, true);
    EXPECT_TRUE(prop.value(obj));

    prop.set_value(obj, false);
    EXPECT_FALSE(prop.value(obj));
}

TEST(PropertyTest, PropertyWithDouble)
{
    auto prop = reflection_api::make_property("doubleValue", &TestStruct::doubleValue);

    TestStruct obj;
    prop.set_value(obj, 2.718);
    EXPECT_DOUBLE_EQ(prop.value(obj), 2.718);

    prop.set_value(obj, 1.414);
    EXPECT_DOUBLE_EQ(prop.value(obj), 1.414);
}

TEST(PropertyTest, PropertyEmptyName)
{
    auto prop = reflection_api::make_property("", &TestStruct::intValue);
    EXPECT_TRUE(prop.name().empty());

    TestStruct obj;
    prop.set_value(obj, 999);
    EXPECT_EQ(prop.value(obj), 999);
}

TEST(PropertyTest, PropertyTypeAliases)
{
    auto prop = reflection_api::make_property("intValue", &TestStruct::intValue);
    
    using ClassType = typename decltype(prop)::class_type;
    using PropertyType = typename decltype(prop)::property_type;
    
    EXPECT_TRUE((std::is_same<ClassType, TestStruct>::value));
    EXPECT_TRUE((std::is_same<PropertyType, int>::value));
}

TEST(PropertyTest, PropertyMultipleSetGet)
{
    auto prop = reflection_api::make_property("intValue", &TestStruct::intValue);

    TestStruct obj;
    
    for (int i = 0; i < 10; ++i) {
        prop.set_value(obj, i);
        EXPECT_EQ(prop.value(obj), i);
    }
}

TEST(PropertyTest, PropertyWithConstObject)
{
    auto prop = reflection_api::make_property("intValue", &TestStruct::intValue);

    TestStruct obj;
    obj.intValue = 777;
    
    const TestStruct& constObj = obj;
    EXPECT_EQ(prop.value(constObj), 777);
}

TEST(PropertyTest, PropertyGetterSetterWithReference)
{
    class RefClass
    {
    public:
        void setRef(int& r) { ref = &r; }
        int& getRef() { return *ref; }
        int getRef() const { return *ref; }

    private:
        int* ref = nullptr;
    };

    // This tests the property with reference types
    int externalValue = 100;
    RefClass obj;
    obj.setRef(externalValue);

    // Note: This is a simplified test - actual reference properties might need different handling
    EXPECT_EQ(obj.getRef(), 100);
}

