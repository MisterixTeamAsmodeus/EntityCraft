#include <ReflectionApi/reflectionapi.h>

#include <iostream>

// Работа с данными используя ссылки на get и set
class Test
{
public:
    int get_a() const
    {
        return a;
    }

    void set_a(const int a)
    {
        this->a = a;
    }

private:
    int a = 0;
};

int main()
{
    using namespace reflection_api;

    // Объявление обобщённой сущности
    auto entity = make_entity<Test>(make_property("a", &Test::set_a, &Test::get_a));

    Test t {};

    // Получение значения поля "a" используя обобщённый доступ
    std::cout << t.get_a() << " - ";
    entity.visit_property("a", [&t](const auto& property) {
        std::cout << property.value(t);
        std::cout << " get value with reflection"
                  << "\n";
    });

    std::cout << "\n";

    // Изменение значения поля "a" используя обобщённый доступ
    entity.visit_property("a", [&t](auto& property) {
        std::cout << "set value with reflection value = " << 6 << "\n";
        property.set_value(t, 6);
    });
    std::cout << "value - " << t.get_a();
}
