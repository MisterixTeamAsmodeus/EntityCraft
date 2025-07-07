#include <ReflectionApi/reflectionapi.h>

#include <iostream>
#include <string>

// Реализация рекурсивного обхода по всем проперти в сущности

struct Test
{
    int a = 5;
    std::string b = "inline test";
    bool c = false;
};

struct ParentItem
{
    int a = 10;
    std::string b = "test";
    Test t {};
};

template<typename ClassType, typename Entity, typename PropertyAction>
void visit_all_property(ClassType&& visitor, Entity&& entity, PropertyAction&& property_action)
{
    using namespace reflection_api;

    entity.for_each(visitor::make_any_property_visitor(
        [&](const auto& property) {
            property_action(std::forward<ClassType>(visitor), std::forward<decltype(property)>(property));
        },
        [&](const auto& reference_property) {
            visit_all_property(reference_property.value(std::forward<ClassType>(visitor)), reference_property.reference_entity(), std::forward<PropertyAction>(property_action));
        }));
}

int main()
{
    using namespace reflection_api;

    // Объявление обобщённой сущности
    const auto inline_entity = make_entity<Test>(
        make_property("inline_a", &Test::a),
        make_property("inline_b", &Test::b),
        make_property("inline_c", &Test::c));

    auto entity = make_entity<ParentItem>(
        make_property("a", &ParentItem::a),
        make_property("b", &ParentItem::b),
        make_reference_property("t", &ParentItem::t, inline_entity));

    ParentItem p {};

    std::cout << "visit property without recursive"
              << "\n";
    entity.for_each([](const auto& property) {
        std::cout << "property name - " << property.name() << "\n";
    });

    std::cout << "\n";

    std::cout << "visit property with recursive"
              << "\n";
    visit_all_property(p, entity, [](const auto& visitor, const auto& property) {
        std::cout << "property name - " << property.name() << " property value - " << property.value(visitor) << "\n";
    });
}
