# ReflectionApi

Библиотека для работы с рефлексией в C++, предоставляющая механизм метаданных для описания структуры классов и работы с их свойствами во время выполнения.

## Описание

ReflectionApi — это header-only библиотека, которая позволяет:
- Описывать структуру классов через метаданные
- Получать и устанавливать значения свойств по имени
- Работать с вложенными объектами
- Итерироваться по всем свойствам объекта
- Использовать визиторы для обработки различных типов свойств

Библиотека особенно полезна для:
- ORM-систем
- Сериализации/десериализации
- Генерации UI на основе метаданных
- Валидации данных
- Реализации паттерна рефлексии в C++

## Требования

- C++14 или выше
- CMake 3.8 или выше
- Зависимость: `TypeConverterApi`

## Установка

### Через CMake

```cmake
# Свяжите с вашим проектом
target_link_libraries(your_target ReflectionApi)
```

### Использование

```cpp
#include <ReflectionApi/reflectionapi.hpp>
```

## Быстрый старт

### Базовый пример

```cpp
#include <ReflectionApi/reflectionapi.hpp>
#include <iostream>
#include <string>

struct Person
{
    int id = 0;
    std::string name;
    int age = 0;
};

int main()
{
    // Создаем property для каждого поля
    auto idProperty = reflection_api::make_property("id", &Person::id);
    auto nameProperty = reflection_api::make_property("name", &Person::name);
    auto ageProperty = reflection_api::make_property("age", &Person::age);
    
    // Создаем entity, объединяющую все свойства
    auto personEntity = make_entity<Person>(
        idProperty,
        nameProperty,
        ageProperty
    );
    
    // Работаем с объектом через entity
    Person person;
    personEntity.set_property_value(person, 1, "id");
    personEntity.set_property_value(person, std::string("Иван"), "name");
    personEntity.set_property_value(person, 25, "age");
    
    // Получаем значения
    int id = 0;
    std::string name;
    int age = 0;
    
    personEntity.get_property_value(person, id, "id");
    personEntity.get_property_value(person, name, "name");
    personEntity.get_property_value(person, age, "age");
    
    std::cout << "ID: " << id << ", Name: " << name << ", Age: " << age << std::endl;
    
    return 0;
}
```

## Основные концепции

### Property (Свойство)

`property` представляет собой обертку над полем класса или методами доступа (геттерами/сеттерами). Связывает имя свойства с указателем на член-класса.

#### Создание property через указатель на поле

```cpp
struct Person {
    int id;
    std::string name;
};

auto idProperty = reflection_api::make_property("id", &Person::id);
auto nameProperty = reflection_api::make_property("name", &Person::name);
```

#### Создание property через геттеры и сеттеры

```cpp
class BankAccount {
public:
    void setBalance(double balance) { _balance = balance; }
    double getBalance() const { return _balance; }
private:
    double _balance = 0.0;
};

auto balanceProperty = reflection_api::make_property(
    "balance",
    &BankAccount::setBalance,
    &BankAccount::getBalance
);
```

#### Работа с property

```cpp
Person person;
idProperty.set_value(person, 1);           // Установить значение
int id = idProperty.value(person);           // Получить значение
std::string name = idProperty.name();       // Получить имя свойства
```

### Entity (Сущность)

`entity` объединяет несколько `property` в единую структуру метаданных, предоставляя обобщенный доступ к свойствам объекта по имени.

#### Создание entity

```cpp
auto personEntity = make_entity<Person>(
    reflection_api::make_property("id", &Person::id),
    reflection_api::make_property("name", &Person::name),
    reflection_api::make_property("age", &Person::age)
);
```

#### Методы entity

```cpp
// Установить значение свойства по имени
personEntity.set_property_value(person, 1, "id");

// Получить значение свойства по имени
int id = 0;
personEntity.get_property_value(person, id, "id");

// Проверить наличие свойства
bool hasId = personEntity.has_property("id");

// Получить количество свойств
size_t count = personEntity.property_count();

// Получить имена всех свойств
std::vector<std::string> names = personEntity.get_property_names();

// Создать пустой объект
Person empty = personEntity.empty_entity();

// Итерация по всем свойствам
personEntity.for_each([](const auto& prop) {
    std::cout << "Property: " << prop.name() << std::endl;
});
```

### Reference Property (Ссылочное свойство)

`reference_property` представляет свойство, которое само является объектом. Связывает свойство с `entity` вложенного объекта, позволяя работать с вложенными структурами рекурсивно.

#### Создание reference_property

```cpp
struct Address {
    std::string street;
    std::string city;
    int zipCode = 0;
};

struct PersonWithAddress {
    int id = 0;
    std::string name;
    Address address;  // Вложенный объект
};

// Сначала создаем entity для вложенного типа
auto addressEntity = reflection_api::make_entity<Address>(
    reflection_api::make_property("street", &Address::street),
    reflection_api::make_property("city", &Address::city),
    reflection_api::make_property("zipCode", &Address::zipCode)
);

// Затем создаем reference_property
auto addressRefProperty = make_reference_property(
    "address",
    &PersonWithAddress::address,
    addressEntity
);
```

#### Работа с reference_property

```cpp
PersonWithAddress person;
Address addr;
addr.street = "Ленина, 10";
addr.city = "Москва";
addr.zipCode = 123456;

// Установить весь вложенный объект
addressRefProperty.set_value(person, addr);

// Получить весь вложенный объект
Address retrievedAddr = addressRefProperty.value(person);

// Доступ к свойствам вложенного объекта через reference_entity
auto refEntity = addressRefProperty.reference_entity();
std::string street;
refEntity.get_property_value(retrievedAddr, street, "street");
```

## API Справочник

### Функции создания

#### `make_property`

Создает объект `property` для поля класса или методов доступа.

```cpp
// Через указатель на поле
template<typename ClassType, typename PropertyType>
auto make_property(
    std::string name,
    helper::Variable_t<ClassType, PropertyType> variable
);

// Через геттер и сеттер
template<typename ClassType, typename PropertyType>
auto make_property(
    std::string name,
    helper::Setter_t<ClassType, PropertyType> setter,
    helper::ConstGetter_t<ClassType, PropertyType> getter
);
```

#### `make_entity`

Создает объект `entity` для типа с указанными свойствами.

```cpp
template<typename ClassType, typename... Properties>
constexpr auto make_entity(const Properties&... properties);
```

#### `make_reference_property`

Создает объект `reference_property` для вложенного объекта.

```cpp
template<typename ClassType, typename PropertyType, typename... ReferenceProperties>
auto make_reference_property(
    std::string name,
    helper::Variable_t<ClassType, PropertyType> variable,
    entity<PropertyType, ReferenceProperties...> reference_entity
);
```

### Класс property

#### Методы

- `void set_value(ClassType& obj, const PropertyType& data) const` - установить значение
- `PropertyType value(const ClassType& obj) const` - получить значение
- `const std::string& name() const noexcept` - получить имя свойства
- `static constexpr PropertyType empty_property()` - создать пустое значение свойства

### Класс entity

#### Методы

- `template<typename TargetType> void set_property_value(ClassType& obj, TargetType&& target, const std::string& property_name) const` - установить значение свойства по имени
- `template<typename TargetType> void get_property_value(const ClassType& obj, TargetType& target, const std::string& property_name) const` - получить значение свойства по имени
- `bool has_property(const std::string& property_name) const noexcept` - проверить наличие свойства
- `constexpr std::size_t property_count() const noexcept` - получить количество свойств
- `std::vector<std::string> get_property_names() const` - получить имена всех свойств
- `template<typename Action> void for_each(Action&& action) const` - выполнить действие для каждого свойства
- `static constexpr ClassType empty_entity()` - создать пустой объект

### Класс reference_property

Наследуется от `property` и добавляет:

- `entity<PropertyType, ReferenceProperties...> reference_entity() const noexcept` - получить entity вложенного объекта

## Продвинутое использование

### Итерация по свойствам (for_each)

Метод `for_each` позволяет перебрать все свойства entity и выполнить для каждого какое-либо действие.

```cpp
// Простой пример - вывод имен всех свойств
personEntity.for_each([](const auto& prop) {
    std::cout << "Property: " << prop.name() << std::endl;
});

// Работа с объектом через for_each
Person person;
person.id = 1;
person.name = "Иван";
person.age = 25;

personEntity.for_each([&person](const auto& prop) {
    auto value = prop.value(person);
    std::cout << prop.name() << " = " << value << std::endl;
});

// Сбор информации о свойствах
std::vector<std::string> propertyNames;
personEntity.for_each([&propertyNames](const auto& prop) {
    propertyNames.push_back(prop.name());
});
```

### Визиторы

Визиторы позволяют разделить обработку различных типов свойств (обычных и ссылочных).

#### property_visitor

Обрабатывает только обычные свойства (игнорирует `reference_property`).

```cpp
#include <ReflectionApi/visitor/propertyvisitor.hpp>

auto propVisitor = reflection_api::visitor::make_property_visitor([](const auto& prop) {
    std::cout << "Обычное свойство: " << prop.name() << std::endl;
});

personEntity.for_each(propVisitor);
```

#### reference_property_visitor

Обрабатывает только ссылочные свойства (игнорирует обычные `property`).

```cpp
#include <ReflectionApi/visitor/referencepropertyvisitor.hpp>

auto refVisitor = reflection_api::visitor::make_reference_property_visitor([](const auto& prop) {
    std::cout << "Ссылочное свойство: " << prop.name() << std::endl;
    std::cout << "Количество свойств в reference_entity: " 
              << prop.reference_entity().property_count() << std::endl;
});

personEntity.for_each(refVisitor);
```

#### any_property_visitor

Обрабатывает все типы свойств (и обычные, и ссылочные).

```cpp
#include <ReflectionApi/visitor/anypropertyvisitor.hpp>

auto anyVisitor = reflection_api::visitor::make_any_property_visitor(
    [](const auto& prop) {
        std::cout << "Обычное свойство: " << prop.name() << std::endl;
    },
    [](const auto& refProp) {
        std::cout << "Ссылочное свойство: " << refProp.name() << std::endl;
    }
);

personEntity.for_each(anyVisitor);
```

### Сериализация

Пример реализации сериализации с использованием `for_each`:

```cpp
void serialize(const auto& entity, const auto& obj) {
    std::cout << "{\n";
    bool first = true;
    entity.for_each([&obj, &first](const auto& prop) {
        if (!first) {
            std::cout << ",\n";
        }
        first = false;
        auto value = prop.value(obj);
        std::cout << "  \"" << prop.name() << "\": ";
        // Здесь можно добавить логику сериализации в зависимости от типа
        std::cout << value;
    });
    std::cout << "\n}\n";
}
```

### Валидация

Пример валидации всех свойств объекта:

```cpp
bool validate(const auto& entity, const auto& obj) {
    bool isValid = true;
    entity.for_each([&obj, &isValid](const auto& prop) {
        auto value = prop.value(obj);
        // Добавить логику валидации в зависимости от типа и имени свойства
        if (prop.name() == "age" && value < 0) {
            isValid = false;
        }
    });
    return isValid;
}
```

## Примеры использования

Библиотека включает несколько подробных примеров в директории `example/ReflectionApi/`:

- **example-1.cpp** - Базовое использование property и entity
- **example-2.cpp** - Работа с геттерами и сеттерами
- **example-3.cpp** - Использование for_each и визиторов
- **example-4.cpp** - Работа с reference_property (вложенные структуры)
- **example-5.cpp** - Дополнительные примеры
- **example-6.cpp** - Продвинутые сценарии

Для сборки примеров:

```bash
cd example/ReflectionApi
mkdir build && cd build
cmake ..
make
```

## Ограничения

1. **Конструктор по умолчанию**: Для работы с `entity` класс должен иметь конструктор по умолчанию (используется в `empty_entity()`).

2. **Типы свойств**: Библиотека работает с любыми типами, поддерживаемыми C++, но для некоторых операций (например, сериализация) может потребоваться дополнительная логика.

3. **Производительность**: Использование рефлексии добавляет небольшой overhead по сравнению с прямым доступом к полям. Для критичных к производительности участков кода рекомендуется профилирование.

## Лицензия

См. файл LICENSE в корне проекта.

## Поддержка

При возникновении вопросов или проблем создайте issue в репозитории проекта.
