#include <gtest/gtest.h>

#include <ReflectionApi/reflectionapi.hpp>
#include <string>

// Тестовые классы для вложенных структур
struct NestedStruct
{
    int nestedValue = 0;
    std::string nestedName;
};

struct ParentStruct
{
    NestedStruct nested;
    int parentValue = 0;
};

class ParentClass
{
public:
    void setNested(const NestedStruct& n) { nested = n; }

    NestedStruct getNested() const { return nested; }

    void setParentValue(int v) { parentValue = v; }

    int getParentValue() const { return parentValue; }

private:
    NestedStruct nested;
    int parentValue = 0;
};

// Тесты для reference_property
TEST(ReflectionApi, ReferenceProperty_CreateReferencePropertyWithMemberPointer)
{
    auto nestedEntity = reflection_api::make_entity<NestedStruct>(
        reflection_api::make_property("nestedValue", &NestedStruct::nestedValue),
        reflection_api::make_property("nestedName", &NestedStruct::nestedName));

    auto refProp = reflection_api::make_reference_property(
        "nested",
        &ParentStruct::nested,
        nestedEntity);

    EXPECT_EQ(refProp.name(), "nested");

    ParentStruct obj;
    NestedStruct nested;
    nested.nestedValue = 42;
    nested.nestedName = "test";

    refProp.set_value(obj, nested);
    EXPECT_EQ(refProp.value(obj).nestedValue, 42);
    EXPECT_EQ(refProp.value(obj).nestedName, "test");
}

TEST(ReflectionApi, ReferenceProperty_WithGetterSetter)
{
    auto nestedEntity = reflection_api::make_entity<NestedStruct>(
        reflection_api::make_property("nestedValue", &NestedStruct::nestedValue),
        reflection_api::make_property("nestedName", &NestedStruct::nestedName));

    auto refProp = reflection_api::make_reference_property(
        "nested",
        &ParentClass::setNested,
        &ParentClass::getNested,
        nestedEntity);

    EXPECT_EQ(refProp.name(), "nested");

    ParentClass obj;
    NestedStruct nested;
    nested.nestedValue = 100;
    nested.nestedName = "getter_setter_test";

    refProp.set_value(obj, nested);
    EXPECT_EQ(refProp.value(obj).nestedValue, 100);
    EXPECT_EQ(refProp.value(obj).nestedName, "getter_setter_test");
}

TEST(ReflectionApi, ReferenceProperty_ReferenceEntityAccess)
{
    auto nestedEntity = reflection_api::make_entity<NestedStruct>(
        reflection_api::make_property("nestedValue", &NestedStruct::nestedValue),
        reflection_api::make_property("nestedName", &NestedStruct::nestedName));

    auto refProp = make_reference_property(
        "nested",
        &ParentStruct::nested,
        nestedEntity);

    const auto& refEntity = refProp.reference_entity();
    EXPECT_EQ(refEntity.property_count(), 2);
    EXPECT_TRUE(refEntity.has_property("nestedValue"));
    EXPECT_TRUE(refEntity.has_property("nestedName"));
}

TEST(ReflectionApi, ReferenceProperty_NestedAccess)
{
    auto nestedEntity = reflection_api::make_entity<NestedStruct>(
        reflection_api::make_property("nestedValue", &NestedStruct::nestedValue),
        reflection_api::make_property("nestedName", &NestedStruct::nestedName));

    auto refProp = make_reference_property(
        "nested",
        &ParentStruct::nested,
        nestedEntity);

    ParentStruct obj;
    NestedStruct nested;
    nested.nestedValue = 200;
    nested.nestedName = "nested_access";
    refProp.set_value(obj, nested);

    // Access nested property through reference entity
    const auto& refEntity = refProp.reference_entity();
    int nestedValue = 0;
    refEntity.get_property_value(refProp.value(obj), nestedValue, "nestedValue");
    EXPECT_EQ(nestedValue, 200);

    std::string nestedName;
    refEntity.get_property_value(refProp.value(obj), nestedName, "nestedName");
    EXPECT_EQ(nestedName, "nested_access");
}

TEST(ReflectionApi, ReferenceProperty_SetNestedProperty)
{
    auto nestedEntity = reflection_api::make_entity<NestedStruct>(
        reflection_api::make_property("nestedValue", &NestedStruct::nestedValue),
        reflection_api::make_property("nestedName", &NestedStruct::nestedName));

    auto refProp = make_reference_property(
        "nested",
        &ParentStruct::nested,
        nestedEntity);

    ParentStruct obj;

    // Set nested property through reference entity
    auto refEntity = refProp.reference_entity();
    auto tempObj = refProp.value(obj);
    refEntity.set_property_value(tempObj, 300, "nestedValue");
    refEntity.set_property_value(tempObj, std::string("set_nested"), "nestedName");
    refProp.set_value(obj, tempObj);

    EXPECT_EQ(refProp.value(obj).nestedValue, 300);
    EXPECT_EQ(refProp.value(obj).nestedName, "set_nested");
}

TEST(ReflectionApi, ReferenceProperty_InEntity)
{
    auto nestedEntity = reflection_api::make_entity<NestedStruct>(
        reflection_api::make_property("nestedValue", &NestedStruct::nestedValue),
        reflection_api::make_property("nestedName", &NestedStruct::nestedName));

    auto refProp = make_reference_property(
        "nested",
        &ParentStruct::nested,
        nestedEntity);

    auto parentEntity = reflection_api::make_entity<ParentStruct>(
        reflection_api::make_property("parentValue", &ParentStruct::parentValue),
        refProp);

    EXPECT_EQ(parentEntity.property_count(), 2);
    EXPECT_TRUE(parentEntity.has_property("nested"));
    EXPECT_TRUE(parentEntity.has_property("parentValue"));

    ParentStruct obj;
    NestedStruct nested;
    nested.nestedValue = 600;
    nested.nestedName = "in_entity";
    refProp.set_value(obj, nested);
    parentEntity.set_property_value(obj, 700, "parentValue");

    EXPECT_EQ(obj.nested.nestedValue, 600);
    EXPECT_EQ(obj.nested.nestedName, "in_entity");
    EXPECT_EQ(obj.parentValue, 700);
}

TEST(ReflectionApi, ReferenceProperty_EmptyEntity)
{
    struct EmptyNested
    {
    };

    auto emptyEntity = reflection_api::make_entity<EmptyNested>();

    struct ParentWithEmpty
    {
        EmptyNested empty;
    };

    auto refProp = reflection_api::make_reference_property(
        "empty",
        &ParentWithEmpty::empty,
        emptyEntity);

    EXPECT_EQ(refProp.reference_entity().property_count(), 0);
}

TEST(ReflectionApi, ReferenceProperty_MultipleNestedLevels)
{
    struct Level2
    {
        int level2Value = 0;
    };

    struct Level1
    {
        Level2 level2;
        int level1Value = 0;
    };

    struct Level0
    {
        Level1 level1;
        int level0Value = 0;
    };

    auto level2Entity = reflection_api::make_entity<Level2>(
        reflection_api::make_property("level2Value", &Level2::level2Value));

    auto level1Entity = reflection_api::make_entity<Level1>(
        reflection_api::make_reference_property("level2", &Level1::level2, level2Entity),
        reflection_api::make_property("level1Value", &Level1::level1Value));

    auto level0Entity = reflection_api::make_entity<Level0>(
        reflection_api::make_reference_property("level1", &Level0::level1, level1Entity),
        reflection_api::make_property("level0Value", &Level0::level0Value));

    Level0 obj;
    level0Entity.set_property_value(obj, 800, "level0Value");

    // Direct access to nested structures
    obj.level1.level1Value = 900;
    level2Entity.set_property_value(obj.level1.level2, 1000, "level2Value");

    EXPECT_EQ(obj.level0Value, 800);
    EXPECT_EQ(obj.level1.level1Value, 900);
    EXPECT_EQ(obj.level1.level2.level2Value, 1000);

    // Verify we can access through reference entities
    EXPECT_EQ(level0Entity.property_count(), 2);
    EXPECT_EQ(level1Entity.property_count(), 2);
    EXPECT_EQ(level2Entity.property_count(), 1);
}

TEST(ReflectionApi, ReferenceProperty_ConstAccess)
{
    auto nestedEntity = reflection_api::make_entity<NestedStruct>(
        reflection_api::make_property("nestedValue", &NestedStruct::nestedValue));

    auto refProp = reflection_api::make_reference_property(
        "nested",
        &ParentStruct::nested,
        nestedEntity);

    ParentStruct obj;
    NestedStruct nested;
    nested.nestedValue = 1100;
    refProp.set_value(obj, nested);

    const ParentStruct& constObj = obj;
    EXPECT_EQ(refProp.value(constObj).nestedValue, 1100);
}
