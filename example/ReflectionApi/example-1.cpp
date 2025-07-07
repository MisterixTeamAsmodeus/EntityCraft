#include <ReflectionApi/reflectionapi.h>

#include <iostream>

// Работа с данными используя ссылки на поля класса
struct Test
{
    int a = 0;
    std::string b;
};

int main()
{
    using namespace reflection_api;

    // Объявление обобщённой сущности
    auto entity = make_entity<Test>(
        make_property("a", &Test::a),
        make_property("b", &Test::b));

    Test t {};

    // Получение значения поля "a" используя обобщённый доступ
    std::cout << t.a << " - ";
    entity.visit_property("a", [&t](const auto& property) {
        std::cout << property.value(t);
        std::cout << " get value with reflection"
                  << "\n";
    });

    std::cout << "\n";

    // Изменение значения поля "a" используя обобщённый доступ
    entity.visit_property("a", [&t](auto& property) {
        std::cout << "set value with reflection. value = " << 6 << "\n";
        property.set_value(t, 6);
    });

    std::cout << "value - " << t.a;
}
