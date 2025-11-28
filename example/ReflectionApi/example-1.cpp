/**
 * ПРИМЕР 1: Базовое использование property и entity
 * 
 * Этот пример демонстрирует основные возможности библиотеки ReflectionApi:
 * 
 * 1. СОЗДАНИЕ PROPERTY
 *    - property представляет собой обертку над полем класса или методами доступа
 *    - Связывает имя свойства с указателем на член-класса (поле или методы)
 *    - Позволяет получать и устанавливать значения через единый интерфейс
 * 
 * 2. СОЗДАНИЕ ENTITY
 *    - entity объединяет несколько property в единую структуру метаданных
 *    - Предоставляет обобщенный доступ к свойствам объекта по имени
 *    - Позволяет работать с объектами без знания их конкретной структуры
 * 
 * 3. ОСНОВНЫЕ ОПЕРАЦИИ
 *    - Получение и установка значений свойств
 *    - Проверка наличия свойств
 *    - Подсчет количества свойств
 *    - Создание пустых объектов
 */

#include <ReflectionApi/reflectionapi.hpp>
#include <iostream>
#include <string>

// Простая структура для демонстрации базовых возможностей
// Структура должна иметь конструктор по умолчанию для работы с entity
struct Person
{
    int id = 0;              // Идентификатор персоны
    std::string name;        // Имя персоны
    int age = 0;             // Возраст персоны
};

int main()
{
    std::cout << "========================================\n";
    std::cout << "ПРИМЕР 1: Базовое использование property и entity\n";
    std::cout << "========================================\n\n";

    // ============================================================
    // ШАГ 1: СОЗДАНИЕ PROPERTY ДЛЯ ОТДЕЛЬНЫХ ПОЛЕЙ
    // ============================================================
    // 
    // make_property создает объект property, который связывает:
    // - Имя свойства (строка) - используется для идентификации
    // - Указатель на член-переменную - для прямого доступа к полю
    //
    // property позволяет работать с полем через единый интерфейс,
    // независимо от типа данных поля
    
    std::cout << "ШАГ 1: Создание property для отдельных полей\n";
    std::cout << "--------------------------------------------\n";
    
    // Создаем property для поля id
    // Первый параметр - имя свойства (используется для поиска)
    // Второй параметр - указатель на член-переменную класса
    auto idProperty = reflection_api::make_property("id", &Person::id);
    
    // Создаем property для поля name
    auto nameProperty = reflection_api::make_property("name", &Person::name);
    
    // Создаем property для поля age
    auto ageProperty = reflection_api::make_property("age", &Person::age);

    std::cout << "Созданы property для полей:\n";
    std::cout << "  - idProperty: имя = '" << idProperty.name() << "'\n";
    std::cout << "  - nameProperty: имя = '" << nameProperty.name() << "'\n";
    std::cout << "  - ageProperty: имя = '" << ageProperty.name() << "'\n\n";

    // ============================================================
    // ШАГ 2: РАБОТА С ОБЪЕКТОМ ЧЕРЕЗ ОТДЕЛЬНЫЕ PROPERTY
    // ============================================================
    //
    // property предоставляет методы:
    // - set_value(obj, value) - установить значение свойства в объекте
    // - value(obj) - получить значение свойства из объекта
    //
    // Это позволяет работать с полями объекта через единый интерфейс
    
    std::cout << "ШАГ 2: Работа с объектом через отдельные property\n";
    std::cout << "--------------------------------------------------\n";
    
    // Создаем объект Person
    Person person1;
    
    // Устанавливаем значения через property
    // set_value принимает объект и новое значение
    idProperty.set_value(person1, 1);
    nameProperty.set_value(person1, std::string("Иван"));
    ageProperty.set_value(person1, 25);

    std::cout << "Установлены значения через property:\n";
    
    // Получаем значения через property
    // value возвращает значение свойства из объекта
    std::cout << "  person1.id = " << idProperty.value(person1) << "\n";
    std::cout << "  person1.name = " << nameProperty.value(person1) << "\n";
    std::cout << "  person1.age = " << ageProperty.value(person1) << "\n\n";

    // ============================================================
    // ШАГ 3: СОЗДАНИЕ ENTITY - НАБОРА СВОЙСТВ
    // ============================================================
    //
    // entity объединяет несколько property в единую структуру
    // Это позволяет работать с объектом через имена свойств,
    // без необходимости хранить отдельные property
    //
    // make_entity<ClassType>(properties...) создает entity для типа ClassType
    // с указанными свойствами
    
    std::cout << "ШАГ 3: Создание entity - набора свойств\n";
    std::cout << "---------------------------------------\n";
    
    // Создаем entity для класса Person
    // entity хранит все property в кортеже и предоставляет
    // методы для работы с ними по имени
    auto personEntity = make_entity<Person>(
        idProperty,
        nameProperty,
        ageProperty
    );

    std::cout << "Создана entity для Person:\n";
    
    // property_count() возвращает количество свойств в entity
    std::cout << "  Количество свойств: " << personEntity.property_count() << "\n";
    
    // has_property(name) проверяет наличие свойства с заданным именем
    std::cout << "  Проверка наличия свойств:\n";
    std::cout << "    - has_property(\"id\") = " 
              << (personEntity.has_property("id") ? "true" : "false") << "\n";
    std::cout << "    - has_property(\"name\") = " 
              << (personEntity.has_property("name") ? "true" : "false") << "\n";
    std::cout << "    - has_property(\"age\") = " 
              << (personEntity.has_property("age") ? "true" : "false") << "\n";
    std::cout << "    - has_property(\"email\") = " 
              << (personEntity.has_property("email") ? "true" : "false") << "\n\n";

    // ============================================================
    // ШАГ 4: РАБОТА С ОБЪЕКТОМ ЧЕРЕЗ ENTITY
    // ============================================================
    //
    // entity предоставляет методы для работы со свойствами по имени:
    // - set_property_value(obj, value, property_name) - установить значение
    // - get_property_value(obj, target, property_name) - получить значение
    //
    // Это позволяет работать с объектом динамически, используя строковые имена
    
    std::cout << "ШАГ 4: Работа с объектом через entity\n";
    std::cout << "---------------------------------------\n";
    
    // Создаем новый объект Person
    Person person2;

    // Устанавливаем значения через entity по имени свойства
    // set_property_value ищет свойство по имени и устанавливает значение
    personEntity.set_property_value(person2, 2, "id");
    personEntity.set_property_value(person2, std::string("Мария"), "name");
    personEntity.set_property_value(person2, 30, "age");

    std::cout << "Установлены значения через entity:\n";
    
    // Получаем значения через entity
    // get_property_value ищет свойство по имени и записывает значение в target
    int id = 0;
    std::string name;
    int age = 0;
    
    personEntity.get_property_value(person2, id, "id");
    personEntity.get_property_value(person2, name, "name");
    personEntity.get_property_value(person2, age, "age");

    std::cout << "  person2.id = " << id << "\n";
    std::cout << "  person2.name = " << name << "\n";
    std::cout << "  person2.age = " << age << "\n\n";

    // ============================================================
    // ШАГ 5: СОЗДАНИЕ ПУСТОГО ОБЪЕКТА
    // ============================================================
    //
    // empty_entity() создает объект типа ClassType с помощью
    // конструктора по умолчанию
    // Это полезно для создания объектов с начальными значениями
    
    std::cout << "ШАГ 5: Создание пустого объекта\n";
    std::cout << "--------------------------------\n";
    
    // Создаем пустой объект через entity
    Person emptyPerson = personEntity.empty_entity();
    
    std::cout << "Создан пустой объект через empty_entity():\n";
    std::cout << "  emptyPerson.id = " << emptyPerson.id << "\n";
    std::cout << "  emptyPerson.name = '" << emptyPerson.name << "'\n";
    std::cout << "  emptyPerson.age = " << emptyPerson.age << "\n\n";

    // ============================================================
    // ЗАКЛЮЧЕНИЕ
    // ============================================================
    //
    // В этом примере мы изучили:
    // 1. Как создавать property для отдельных полей класса
    // 2. Как работать с объектами через property
    // 3. Как создавать entity для объединения нескольких property
    // 4. Как работать с объектами через entity по именам свойств
    // 5. Как создавать пустые объекты через entity
    //
    // Эти базовые операции являются основой для всех более сложных
    // сценариев использования библиотеки ReflectionApi

    std::cout << "========================================\n";
    std::cout << "Пример завершен успешно!\n";
    std::cout << "========================================\n";

    return 0;
}
