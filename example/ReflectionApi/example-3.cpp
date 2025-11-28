/**
 * ПРИМЕР 3: Использование for_each и визиторов
 * 
 * Этот пример демонстрирует продвинутые возможности работы с entity:
 * 
 * 1. ИТЕРАЦИЯ ПО СВОЙСТВАМ
 *    - for_each - универсальный метод для перебора всех свойств
 *    - Работа с lambda-функциями для обработки свойств
 *    - Сбор информации о свойствах
 * 
 * 2. ВИЗИТОРЫ
 *    - property_visitor - обработка только обычных свойств
 *    - reference_property_visitor - обработка только ссылочных свойств
 *    - any_property_visitor - обработка всех типов свойств
 * 
 * 3. ПРАКТИЧЕСКИЕ ПРИМЕРЫ
 *    - Вывод всех свойств объекта
 *    - Подсчет свойств по типам
 *    - Модификация свойств определенного типа
 *    - Сериализация/десериализация
 */

#include <ReflectionApi/reflectionapi.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

// Класс для вывода значений свойств в зависимости от типа
struct Printer
{
    template<typename T, std::enable_if_t<std::is_same_v<T, int>, int> = 0>
    void print(T&& value) const
    {
        std::cout << value;
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, double>, int> = 0>
    void print(T&& value) const
    {
        std::cout << std::fixed << std::setprecision(2) << value;
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, bool>, int> = 0>
    void print(T&& value) const
    {
        std::cout << (value ? "true" : "false");
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, std::string>, int> = 0>
    void print(T&& value) const
    {
        std::cout << "'" << value << "'";
    }
};

struct Counter
{
    template<typename T, std::enable_if_t<std::is_same_v<T, int>, int> = 0>
    void count(T&&) noexcept
    {
        ++_intCount;
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, double>, int> = 0>
    void count(T&&) noexcept
    {
        ++_doubleCount;
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, bool>, int> = 0>
    void count(T&&) noexcept
    {
        ++_boolCount;
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, std::string>, int> = 0>
    void count(T&&) noexcept
    {
        ++_stringCount;
    }

    int _intCount = 0;
    int _doubleCount = 0;
    int _boolCount = 0;
    int _stringCount = 0;
};

struct Modifier
{
    template<typename T, std::enable_if_t<std::is_same_v<T, int>, int> = 0>
    void modify(T&& value) const noexcept
    {
        value = 0;
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, double>, int> = 0>
    void modify(T&& value) const noexcept
    {
        value = 0.0;
    }

    template<typename T, std::enable_if_t<!std::is_same_v<T, int> && !std::is_same_v<T, double>, int> = 0>
    void modify(T&&) const noexcept
    {
    }
};

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
// Структура для демонстрации различных типов свойств
struct Product
{
    int id = 0;              // Идентификатор продукта
    std::string name;        // Название продукта
    double price = 0.0;      // Цена продукта
    bool inStock = false;    // Наличие на складе
};

int main()
{
    std::cout << "========================================\n";
    std::cout << "ПРИМЕР 3: Использование for_each и визиторов\n";
    std::cout << "========================================\n\n";

    // ============================================================
    // ШАГ 1: СОЗДАНИЕ ENTITY
    // ============================================================
    
    std::cout << "ШАГ 1: Создание entity для Product\n";
    std::cout << "-----------------------------------\n";
    
    // Создаем entity для Product с различными типами свойств
    auto productEntity = reflection_api::make_entity<Product>(
        reflection_api::make_property("id", &Product::id),
        reflection_api::make_property("name", &Product::name),
        reflection_api::make_property("price", &Product::price),
        reflection_api::make_property("inStock", &Product::inStock)
    );

    std::cout << "Создана entity для Product с " 
              << productEntity.property_count() << " свойствами\n\n";

    // ============================================================
    // ШАГ 2: БАЗОВОЕ ИСПОЛЬЗОВАНИЕ for_each
    // ============================================================
    //
    // for_each позволяет перебрать все свойства entity
    // и выполнить для каждого какое-либо действие
    //
    // Сигнатура: for_each(Action&& action)
    // где Action - функция или lambda, принимающая property
    
    std::cout << "ШАГ 2: Итерация по всем свойствам с помощью for_each\n";
    std::cout << "------------------------------------------------------\n";
    
    // Простейший пример - вывод имен всех свойств
    productEntity.for_each([](const auto& prop) {
        std::cout << "  - Свойство: '" << prop.name() << "'\n";
    });
    std::cout << "\n";

    // ============================================================
    // ШАГ 3: СБОР ИНФОРМАЦИИ О СВОЙСТВАХ
    // ============================================================
    //
    // for_each можно использовать для сбора информации
    // о всех свойствах entity
    
    std::cout << "ШАГ 3: Сбор информации о свойствах\n";
    std::cout << "------------------------------------\n";
    
    // Собираем имена всех свойств
    std::vector<std::string> propertyNames;
    int propertyCount = 0;

    // Lambda-функция захватывает переменные по ссылке [&]
    productEntity.for_each([&propertyNames, &propertyCount](const auto& prop) {
        propertyNames.push_back(prop.name());
        ++propertyCount;
    });

    std::cout << "Найдено свойств: " << propertyCount << "\n";
    std::cout << "Имена свойств: ";
    for (size_t i = 0; i < propertyNames.size(); ++i) {
        std::cout << "'" << propertyNames[i] << "'";
        if (i < propertyNames.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "\n\n";

    // ============================================================
    // ШАГ 4: РАБОТА С ОБЪЕКТОМ ЧЕРЕЗ for_each
    // ============================================================
    //
    // for_each можно использовать для работы с конкретным объектом
    // Получая значения свойств и выполняя над ними операции
    
    std::cout << "ШАГ 4: Вывод значений всех свойств объекта\n";
    std::cout << "------------------------------------------\n";
    
    // Создаем объект Product с данными
    Product product;
    product.id = 1;
    product.name = "Ноутбук";
    product.price = 75000.0;
    product.inStock = true;

    // Перебираем все свойства и выводим их значения
    productEntity.for_each([&product](const auto& prop) {
        // Получаем значение свойства
        auto value = prop.value(product);
        
        // Выводим информацию о свойстве
        std::cout << "  " << std::setw(10) << std::left << prop.name() << " = ";
        
        Printer printer;
        printer.print(std::forward<decltype(value)>(value));
        std::cout << "\n";
    });
    std::cout << "\n";

    // ============================================================
    // ШАГ 5: ИСПОЛЬЗОВАНИЕ property_visitor
    // ============================================================
    //
    // property_visitor позволяет обрабатывать только обычные свойства
    // (не ссылочные). Это полезно, когда нужно разделить обработку
    // обычных и вложенных свойств
    
    std::cout << "ШАГ 5: Использование property_visitor\n";
    std::cout << "-------------------------------------\n";
    
    // Создаем визитор для обычных свойств
    // reflection_api::make_property_visitor создает визитор, который обрабатывает
    // только обычные property, игнорируя reference_property
    auto propVisitor = reflection_api::visitor::make_property_visitor([](const auto& prop) {
        std::cout << "  Обработано обычное свойство: '" << prop.name() << "'\n";
    });

    // Применяем визитор ко всем свойствам
    // Визитор автоматически определяет тип свойства и вызывает
    // соответствующий обработчик
    productEntity.for_each(propVisitor);
    std::cout << "\n";

    // ============================================================
    // ШАГ 6: ПОДСЧЕТ СВОЙСТВ ПО ТИПАМ
    // ============================================================
    //
    // Используя for_each и if constexpr, можно подсчитать
    // количество свойств каждого типа
    
    std::cout << "ШАГ 6: Подсчет свойств по типам\n";
    std::cout << "--------------------------------\n";
    
    Counter counter;

    // Создаем временный объект для определения типов свойств
    productEntity.for_each([&counter](const auto& prop) {
        Product temp;
        auto value = prop.value(temp);
        counter.count(std::forward<decltype(value)>(value));
    });

    std::cout << "  int свойств: " << counter._intCount << "\n";
    std::cout << "  string свойств: " << counter._stringCount << "\n";
    std::cout << "  double свойств: " << counter._doubleCount << "\n";
    std::cout << "  bool свойств: " << counter._boolCount << "\n\n";

    // ============================================================
    // ШАГ 7: МОДИФИКАЦИЯ СВОЙСТВ ОПРЕДЕЛЕННОГО ТИПА
    // ============================================================
    //
    // for_each можно использовать для модификации свойств
    // определенного типа во всех объектах
    
    std::cout << "ШАГ 7: Сброс всех числовых свойств\n";
    std::cout << "-----------------------------------\n";
    
    // Создаем копию объекта
    Product product2 = product;

    Modifier modifier;
    // Сбрасываем все числовые свойства
    productEntity.for_each([&product2, &modifier](auto& prop) {
        Product temp;
        auto value = prop.value(temp);

        modifier.modify(std::forward<decltype(value)>(value));
        prop.set_value(product2, value);
    });

    std::cout << "После сброса:\n";
    std::cout << "  id = " << product2.id << "\n";
    std::cout << "  price = " << product2.price << "\n";
    std::cout << "  name = '" << product2.name << "' (не изменено)\n";
    std::cout << "  inStock = " << (product2.inStock ? "true" : "false") << " (не изменено)\n\n";

    // ============================================================
    // ШАГ 8: ИСПОЛЬЗОВАНИЕ CONST ВЕРСИИ for_each
    // ============================================================
    //
    // entity имеет const версию for_each, которая позволяет
    // перебирать свойства без возможности их модификации
    
    std::cout << "ШАГ 8: Использование const версии for_each\n";
    std::cout << "------------------------------------------\n";
    
    // Создаем const ссылку на entity
    const auto& constEntity = productEntity;
    
    // const версия for_each гарантирует, что свойства не будут изменены
    constEntity.for_each([](const auto& prop) {
        std::cout << "  [const] Свойство: '" << prop.name() << "'\n";
    });
    std::cout << "\n";

    // ============================================================
    // ШАГ 9: ПРАКТИЧЕСКИЙ ПРИМЕР - СЕРИАЛИЗАЦИЯ
    // ============================================================
    //
    // Используя for_each, можно легко реализовать сериализацию
    // всех свойств объекта
    
    std::cout << "ШАГ 9: Практический пример - сериализация\n";
    std::cout << "-----------------------------------------\n";
    
    std::cout << "Сериализация объекта product:\n";
    std::cout << "  {\n";
    
    bool first = true;
    productEntity.for_each([&product, &first](const auto& prop) {
        if (!first) {
            std::cout << ",\n";
        }
        first = false;
        
        auto value = prop.value(product);
        std::cout << "    \"" << prop.name() << "\": ";
        Serializer serializer;
        serializer.serialize(std::forward<decltype(value)>(value));
    });
    
    std::cout << "\n  }\n\n";

    // ============================================================
    // ЗАКЛЮЧЕНИЕ
    // ============================================================
    //
    // В этом примере мы изучили:
    // 1. Как использовать for_each для итерации по свойствам
    // 2. Как собирать информацию о свойствах
    // 3. Как работать с объектами через for_each
    // 4. Как использовать визиторы для разделения обработки
    // 5. Как подсчитывать свойства по типам
    // 6. Как модифицировать свойства определенного типа
    // 7. Практические примеры использования (сериализация)
    //
    // for_each и визиторы предоставляют мощный механизм для
    // работы с метаданными объектов, что особенно полезно для:
    // - Сериализации/десериализации
    // - Валидации данных
    // - Генерации UI
    // - ORM-систем
    // - Рефлексии

    std::cout << "========================================\n";
    std::cout << "Пример завершен успешно!\n";
    std::cout << "========================================\n";

    return 0;
}
