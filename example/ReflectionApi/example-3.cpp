#include <ReflectionApi/reflectionapi.h>

#include <iostream>

// Работа с данными используя ссылки на поля класса с использованием ссылочных проперти

struct Test
{
    int a = 0;
};

struct ParentItem
{
    Test t;
};

int main()

{
    using namespace reflection_api;

    // Объявление обобщённой сущности
    auto entity = make_entity<ParentItem>(make_reference_property("t", &ParentItem::t,
        make_entity<Test>(make_property("a", &Test::a))));

    ParentItem p {};

    // Получение значения поля "a" используя обобщённый доступ
    std::cout << p.t.a << " - ";
    entity.visit_property("t", visitor::make_reference_property_visitor([&p](const auto& property) {
        const auto value = property.value(p);
        auto inlineEntity = property.reference_entity();

        inlineEntity.visit_property("a", [&p, value](const auto& inline_property) {
            std::cout << inline_property.value(value);
            std::cout << " get value in inline entity with reflection"
                      << "\n";
        });
    }));

    std::cout << "\n";

    // Изменение значения поля "a" используя обобщённый доступ
    entity.visit_property("t", visitor::make_reference_property_visitor([&p](auto& property) {
        auto value = property.value(p);
        auto inlineEntity = property.reference_entity();

        inlineEntity.visit_property("a", [&p, &value](auto& inline_property) {
            std::cout << "set value in inline entity with reflection. value = " << 6 << "\n";
            inline_property.set_value(value, 6);
        });

        property.set_value(p, value);
    }));

    std::cout << "value - " << p.t.a;
}
