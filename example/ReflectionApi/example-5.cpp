/**
 * ПРИМЕР 5: Продвинутое использование визиторов
 * 
 * Этот пример демонстрирует все возможности визиторов:
 * 
 * 1. ТИПЫ ВИЗИТОРОВ
 *    - property_visitor - обработка только обычных свойств
 *    - reference_property_visitor - обработка только ссылочных свойств
 *    - any_property_visitor - обработка всех типов свойств
 * 
 * 2. ПРАКТИЧЕСКИЕ ПРИМЕРЫ
 *    - Разделение обработки обычных и ссылочных свойств
 *    - Рекурсивная обработка вложенных объектов
 *    - Сериализация с поддержкой вложенных структур
 *    - Валидация данных
 * 
 * 3. КОМБИНИРОВАНИЕ ВИЗИТОРОВ
 *    - Использование нескольких визиторов одновременно
 *    - Условная обработка свойств
 */

#include <functional>
#include <iomanip>
#include <iostream>
#include <ReflectionApi/reflectionapi.hpp>
#include <sstream>
#include <string>
#include <vector>

// ============================================================
// СТРУКТУРЫ ДЛЯ ДЕМОНСТРАЦИИ
// ============================================================

struct Serializer
{
    template<typename T, std::enable_if_t<std::is_same_v<T, int>, int> = 0>
    void serialize(T&& value) const noexcept
    {
        std::cout << value;
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, double>, int> = 0>
    void serialize(T&& value) const noexcept
    {
        std::cout << std::fixed << std::setprecision(2) << value;
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, bool>, int> = 0>
    void serialize(T&& value) const noexcept
    {
        std::cout << (value ? "true" : "false");
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, std::string>, int> = 0>
    void serialize(T&& value) const noexcept
    {
        std::cout << "'" << value << "'";
    }
};

struct Validator
{
    template<typename T, std::enable_if_t<std::is_same_v<T, int>, int> = 0>
    void validate(T&& value) const noexcept
    {
        if (value <= 0) {
            throw std::invalid_argument("value must be positive");
        }
    }   
    template<typename T, std::enable_if_t<std::is_same_v<T, double>, int> = 0>
    void validate(T&& value) const noexcept
    {
        if (value < 0) {
            throw std::invalid_argument("value must be positive");
        }
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, std::string>, int> = 0>
    void validate(T&& value) const noexcept
    {
        if (value.empty()) {
            throw std::invalid_argument("value must be not empty");
        }
    }
};

// Вложенная структура - контактная информация
struct ContactInfo
{
    std::string email;
    std::string phone;
};

// Основная структура - сотрудник
struct Employee
{
    int id = 0;
    std::string name;
    ContactInfo contact;  // Вложенный объект
    double salary = 0.0;
};

int main()
{
    std::cout << "========================================\n";
    std::cout << "ПРИМЕР 5: Продвинутое использование визиторов\n";
    std::cout << "========================================\n\n";

    // ============================================================
    // ШАГ 1: СОЗДАНИЕ ENTITY С ОБЫЧНЫМИ И ССЫЛОЧНЫМИ СВОЙСТВАМИ
    // ============================================================
    
    std::cout << "ШАГ 1: Создание entity с обычными и ссылочными свойствами\n";
    std::cout << "---------------------------------------------------------\n";
    
    // Создаем entity для вложенной структуры
    auto contactEntity = reflection_api::make_entity<ContactInfo>(
        reflection_api::make_property("email", &ContactInfo::email),
        reflection_api::make_property("phone", &ContactInfo::phone)
    );

    // Создаем entity для Employee с комбинацией свойств
    auto employeeEntity = make_entity<Employee>(
        reflection_api::make_property("id", &Employee::id),                    // Обычное свойство
        reflection_api::make_property("name", &Employee::name),                 // Обычное свойство
        reflection_api::make_reference_property("contact", &Employee::contact, contactEntity),  // Ссылочное свойство
        reflection_api::make_property("salary", &Employee::salary)              // Обычное свойство
    );

    std::cout << "Создана entity для Employee с " 
              << employeeEntity.property_count() << " свойствами\n";
    std::cout << "  (3 обычных + 1 ссылочное)\n\n";

    // ============================================================
    // ШАГ 2: ИСПОЛЬЗОВАНИЕ property_visitor
    // ============================================================
    //
    // property_visitor обрабатывает только обычные свойства,
    // игнорируя reference_property
    
    std::cout << "ШАГ 2: Использование property_visitor\n";
    std::cout << "-------------------------------------\n";
    
    // Создаем визитор для обычных свойств
    // Этот визитор будет вызываться только для обычных property
    auto propVisitor = reflection_api::visitor::make_property_visitor([](const auto& prop) {
        std::cout << "  [Обычное свойство] '" << prop.name() << "'\n";
    });

    std::cout << "Обработка только обычных свойств:\n";
    employeeEntity.for_each([&propVisitor](auto& prop) {
        propVisitor(prop);
    });
    std::cout << "\n";

    // ============================================================
    // ШАГ 3: ИСПОЛЬЗОВАНИЕ reference_property_visitor
    // ============================================================
    //
    // reference_property_visitor обрабатывает только ссылочные свойства,
    // игнорируя обычные property
    
    std::cout << "ШАГ 3: Использование reference_property_visitor\n";
    std::cout << "------------------------------------------------\n";
    
    // Создаем визитор для ссылочных свойств
    // Этот визитор будет вызываться только для reference_property
    auto refPropVisitor = reflection_api::visitor::make_reference_property_visitor([](const auto& refProp) {
        std::cout << "  [Ссылочное свойство] '" << refProp.name() << "'\n";
        std::cout << "    Содержит " << refProp.reference_entity().property_count() 
                  << " вложенных свойств\n";
    });

    std::cout << "Обработка только ссылочных свойств:\n";
    employeeEntity.for_each([&refPropVisitor](auto& prop) {
        refPropVisitor(prop);
    });
    std::cout << "\n";

    // ============================================================
    // ШАГ 4: ИСПОЛЬЗОВАНИЕ any_property_visitor
    // ============================================================
    //
    // any_property_visitor позволяет обрабатывать все типы свойств,
    // разделяя логику для обычных и ссылочных
    
    std::cout << "ШАГ 4: Использование any_property_visitor\n";
    std::cout << "------------------------------------------\n";
    
    // Создаем визитор для всех типов свойств
    // Первый параметр - обработчик для обычных свойств
    // Второй параметр - обработчик для ссылочных свойств
    auto anyVisitor = reflection_api::visitor::make_any_property_visitor(
        // Обработчик для обычных свойств
        [](const auto& prop) {
            std::cout << "  [Обычное] '" << prop.name() << "'\n";
        },
        // Обработчик для ссылочных свойств
        [](const auto& refProp) {
            std::cout << "  [Ссылочное] '" << refProp.name() << "'\n";
        }
    );

    std::cout << "Обработка всех типов свойств:\n";
    employeeEntity.for_each([&anyVisitor](auto& prop) {
        anyVisitor(prop);
    });
    std::cout << "\n";

    // ============================================================
    // ШАГ 5: ПРАКТИЧЕСКИЙ ПРИМЕР - СЕРИАЛИЗАЦИЯ С ВЛОЖЕННЫМИ ОБЪЕКТАМИ
    // ============================================================
    //
    // Используя визиторы, можно реализовать сериализацию,
    // которая правильно обрабатывает вложенные объекты
    
    std::cout << "ШАГ 5: Практический пример - сериализация с вложенными объектами\n";
    std::cout << "----------------------------------------------------------------\n";
    
    // Создаем объект Employee с данными
    Employee employee;
    employee.id = 1;
    employee.name = "Иван Петров";
    employee.contact.email = "ivan@example.com";
    employee.contact.phone = "+7-999-123-45-67";
    employee.salary = 100000.0;

    // Функция для сериализации обычного свойства
    auto serializeProperty = [&employee](const auto& prop) {
        auto value = prop.value(employee);
        std::cout << "    \"" << prop.name() << "\": ";
        
        Serializer serializer;
        serializer.serialize(std::forward<decltype(value)>(value));
    };

    // Функция для сериализации ссылочного свойства
    auto serializeRefProperty = [&employee](const auto& refProp) {
        std::cout << "    \"" << refProp.name() << "\": {\n";
        
        // Получаем вложенный объект
        auto nestedObj = refProp.value(employee);
        auto nestedEntity = refProp.reference_entity();
        
        // Сериализуем свойства вложенного объекта
        bool first = true;
        nestedEntity.for_each([&nestedObj, &first](const auto& nestedProp) {
            if (!first) {
                std::cout << ",\n";
            }
            first = false;
            
            auto value = nestedProp.value(nestedObj);
            std::cout << "      \"" << nestedProp.name() << "\": ";
            
            Serializer serializer;
            serializer.serialize(std::forward<decltype(value)>(value));
        });
        
        std::cout << "\n    }";
    };

    // Создаем визитор для сериализации
    auto serializeVisitor = reflection_api::visitor::make_any_property_visitor(
        serializeProperty,
        serializeRefProperty
    );

    std::cout << "Сериализация объекта employee:\n";
    std::cout << "  {\n";
    
    bool first = true;
    employeeEntity.for_each([&serializeVisitor, &first](auto& prop) {
        if (!first) {
            std::cout << ",\n";
        }
        first = false;
        serializeVisitor(prop);
    });
    
    std::cout << "\n  }\n\n";

    // ============================================================
    // ШАГ 6: ПРАКТИЧЕСКИЙ ПРИМЕР - ВАЛИДАЦИЯ ДАННЫХ
    // ============================================================
    //
    // Визиторы можно использовать для валидации данных
    
    std::cout << "ШАГ 6: Практический пример - валидация данных\n";
    std::cout << "----------------------------------------------\n";
    
    std::vector<std::string> validationErrors;

    // Валидация обычных свойств
    auto validateProperty = [&employee, &validationErrors](const auto& prop) {
        auto value = prop.value(employee);
        
        Validator validator;
        validator.validate(std::forward<decltype(value)>(value));
    };

    // Валидация ссылочных свойств
    auto validateRefProperty = [&employee, &validationErrors](const auto& refProp) {
        auto nestedObj = refProp.value(employee);
        auto nestedEntity = refProp.reference_entity();
        
        // Валидируем свойства вложенного объекта
        nestedEntity.for_each([&nestedObj, &validationErrors](const auto& nestedProp) {
            auto value = nestedProp.value(nestedObj);
            
            Validator validator;
            validator.validate(std::forward<decltype(value)>(value));
        });
    };

    // Создаем визитор для валидации
    auto validateVisitor = reflection_api::visitor::make_any_property_visitor(
        validateProperty,
        validateRefProperty
    );

    // Выполняем валидацию
    employeeEntity.for_each([&validateVisitor](auto& prop) {
        validateVisitor(prop);
    });

    std::cout << "Результаты валидации:\n";
    if (validationErrors.empty()) {
        std::cout << "  ✓ Все данные валидны\n";
    } else {
        std::cout << "  ✗ Найдены ошибки:\n";
        for (const auto& error : validationErrors) {
            std::cout << "    - " << error << "\n";
        }
    }
    std::cout << "\n";

    // ============================================================
    // ШАГ 7: ОБРАБОТКА ВЛОЖЕННЫХ ОБЪЕКТОВ
    // ============================================================
    //
    // Визиторы можно использовать для обработки вложенных объектов
    
    std::cout << "ШАГ 7: Обработка вложенных объектов\n";
    std::cout << "-----------------------------------\n";
    
    // Обработка ссылочных свойств
    auto recursiveVisitor = reflection_api::visitor::make_reference_property_visitor(
        [](const auto& refProp) {
            std::cout << "  [Вложенное свойство] " << refProp.name() << ":\n";
            
            // Выводим все свойства вложенного объекта
            refProp.reference_entity().for_each([](const auto& nestedProp) {
                std::cout << "    - " << nestedProp.name() << "\n";
            });
        }
    );

    std::cout << "Обработка вложенных объектов:\n";
    employeeEntity.for_each([&recursiveVisitor](auto& prop) {
        recursiveVisitor(prop);
    });
    std::cout << "\n";

    // ============================================================
    // ЗАКЛЮЧЕНИЕ
    // ============================================================
    //
    // В этом примере мы изучили:
    // 1. Различные типы визиторов и их назначение
    // 2. Как разделять обработку обычных и ссылочных свойств
    // 3. Практические примеры использования визиторов:
    //    - Сериализация с поддержкой вложенных структур
    //    - Валидация данных
    //    - Рекурсивная обработка вложенных объектов
    //
    // Визиторы предоставляют мощный механизм для:
    // - Разделения логики обработки разных типов свойств
    // - Реализации сложных алгоритмов работы с метаданными
    // - Создания универсальных утилит (сериализация, валидация и т.д.)

    std::cout << "========================================\n";
    std::cout << "Пример завершен успешно!\n";
    std::cout << "========================================\n";

    return 0;
}

