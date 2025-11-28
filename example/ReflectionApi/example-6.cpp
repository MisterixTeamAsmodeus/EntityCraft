/**
 * ПРИМЕР 6: Практические сценарии использования
 *
 * Этот пример демонстрирует реальные сценарии использования библиотеки:
 *
 * 1. ORM-ПОДОБНАЯ РАБОТА
 *    - Маппинг объектов на таблицы базы данных
 *    - Автоматическое извлечение имен полей
 *    - Генерация SQL-запросов
 *
 * 2. КОНВЕРТАЦИЯ ТИПОВ
 *    - Использование property_converter
 *    - Преобразование между строками и типами
 *    - Настройка конвертеров
 *
 * 3. ДИНАМИЧЕСКАЯ РАБОТА С ОБЪЕКТАМИ
 *    - Создание объектов по именам свойств
 *    - Копирование значений между объектами
 *    - Сравнение объектов
 */

#include <iostream>
#include <map>
#include <ReflectionApi/reflectionapi.hpp>
#include <string>

// ============================================================
// СТРУКТУРЫ ДЛЯ ДЕМОНСТРАЦИИ
// ============================================================

struct TypeToSqlTypeConverter{
    template<typename T, std::enable_if_t<std::is_same_v<T, int>, int> = 0>
    std::string convert(T&& value) const
    {
        return "INTEGER";
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, std::string>, int> = 0>
    std::string convert(T&& value) const
    {
        return "VARCHAR(255)";
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, double>, int> = 0>
    std::string convert(T&& value) const
    {
        return "REAL";
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, bool>, int> = 0>
    std::string convert(T&& value) const
    {
        return "BOOLEAN";
    }
};

struct TypeToSqlValueConverter{
    template<typename T, std::enable_if_t<std::is_same_v<T, int>, int> = 0>
    void convert(T&& value) const
    {
        std::cout << std::to_string(value);
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, std::string>, int> = 0>
    void convert(T&& value) const
    {
        std::cout << value;
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, double>, int> = 0>
    void convert(T&& value) const
    {
        std::cout << std::to_string(value);
    }
    template<typename T, std::enable_if_t<std::is_same_v<T, bool>, int> = 0>
    void convert(T&& value) const
    {
        std::cout << (value ? "true" : "false");
    }
};

struct User
{
    int id = 0;
    std::string username;
    std::string email;
    int age = 0;
};

int main()
{
    std::cout << "========================================\n";
    std::cout << "ПРИМЕР 6: Практические сценарии использования\n";
    std::cout << "========================================\n\n";

    // ============================================================
    // ШАГ 1: СОЗДАНИЕ ENTITY
    // ============================================================

    std::cout << "ШАГ 1: Создание entity для User\n";
    std::cout << "--------------------------------\n";

    auto userEntity = reflection_api::make_entity<User>(
        reflection_api::make_property("id", &User::id),
        reflection_api::make_property("username", &User::username),
        reflection_api::make_property("email", &User::email),
        reflection_api::make_property("age", &User::age));

    std::cout << "Создана entity для User с "
              << userEntity.property_count() << " свойствами\n\n";

    // ============================================================
    // ШАГ 2: ГЕНЕРАЦИЯ SQL-ЗАПРОСОВ (ORM-ПОДОБНАЯ РАБОТА)
    // ============================================================

    std::cout << "ШАГ 2: Генерация SQL-запросов (ORM-подобная работа)\n";
    std::cout << "---------------------------------------------------\n";

    // Генерация CREATE TABLE
    std::cout << "CREATE TABLE запрос:\n";
    std::cout << "  CREATE TABLE users (\n";

    bool first = true;
    userEntity.for_each([&first](const auto& prop) {
        if(!first) {
            std::cout << ",\n";
        }
        first = false;

        std::cout << "    " << prop.name() << " ";

        User temp;
        auto value = prop.value(temp);

        TypeToSqlTypeConverter converter;
        std::cout << converter.convert(std::forward<decltype(value)>(value));
    });

    std::cout << "\n  );\n\n";

    // Генерация INSERT запроса
    User user;
    user.id = 1;
    user.username = "john_doe";
    user.email = "john@example.com";
    user.age = 30;

    std::cout << "INSERT запрос:\n";
    std::cout << "  INSERT INTO users (";

    first = true;
    userEntity.for_each([&first](const auto& prop) {
        if(!first) {
            std::cout << ", ";
        }
        first = false;
        std::cout << prop.name();
    });

    std::cout << ") VALUES (";

    first = true;
    userEntity.for_each([&user, &first](const auto& prop) {
        if(!first) {
            std::cout << ", ";
        }
        first = false;

        auto value = prop.value(user);

        TypeToSqlValueConverter converter;
        converter.convert(std::forward<decltype(value)>(value));
    });

    std::cout << ");\n\n";

    // ============================================================
    // ШАГ 3: ИСПОЛЬЗОВАНИЕ КОНВЕРТЕРОВ ТИПОВ
    // ============================================================

    std::cout << "ШАГ 3: Использование конвертеров типов\n";
    std::cout << "--------------------------------------\n";

    // Получаем property для id
    auto idProp = reflection_api::make_property("id", &User::id);

    // Получаем конвертер для этого property
    auto converter = idProp.property_converter();

    // Конвертируем из строки в int
    int idValue = 0;
    converter->fill_from_string(idValue, "42");
    std::cout << "Конвертация из строки '42' в int: " << idValue << "\n";

    // Конвертируем из int в строку
    std::string idString = converter->convert_to_string(100);
    std::cout << "Конвертация из int 100 в строку: '" << idString << "'\n\n";

    // ============================================================
    // ШАГ 4: ДИНАМИЧЕСКОЕ СОЗДАНИЕ ОБЪЕКТОВ
    // ============================================================

    std::cout << "ШАГ 4: Динамическое создание объектов\n";
    std::cout << "--------------------------------------\n";

    // Создаем объект из map (например, из JSON или формы)
    std::map<std::string, std::string> formData = {
        { "id", "2" },
        { "username", "jane_smith" },
        { "email", "jane@example.com" },
        { "age", "25" }
    };

    // Создаем пустой объект
    User newUser = userEntity.empty_entity();

    // Заполняем объект из map
    for(const auto& pair : formData) {
        if(userEntity.has_property(pair.first)) {
            // Получаем property для определения типа
            userEntity.for_each([&newUser, &pair, &userEntity](const auto& prop) {
                if(prop.name() == pair.first) {
                    User temp;
                    auto value = prop.value(temp);

                    // Используем конвертер для преобразования
                    auto conv = prop.property_converter();

                    decltype(value) tempValue;
                    conv->fill_from_string(tempValue, pair.second);
                    userEntity.set_property_value(newUser, tempValue, pair.first);
                }
            });
        }
    }

    std::cout << "Создан объект из map:\n";
    std::cout << "  id = " << newUser.id << "\n";
    std::cout << "  username = '" << newUser.username << "'\n";
    std::cout << "  email = '" << newUser.email << "'\n";
    std::cout << "  age = " << newUser.age << "\n\n";

    // ============================================================
    // ШАГ 5: КОПИРОВАНИЕ ЗНАЧЕНИЙ МЕЖДУ ОБЪЕКТАМИ
    // ============================================================

    std::cout << "ШАГ 5: Копирование значений между объектами\n";
    std::cout << "-------------------------------------------\n";

    User sourceUser;
    sourceUser.id = 10;
    sourceUser.username = "source_user";
    sourceUser.email = "source@example.com";
    sourceUser.age = 35;

    User targetUser = userEntity.empty_entity();

    // Копируем все свойства из sourceUser в targetUser
    userEntity.for_each([&sourceUser, &targetUser](const auto& prop) {
        auto value = prop.value(sourceUser);
        prop.set_value(targetUser, value);
    });

    std::cout << "Скопированы значения из sourceUser в targetUser:\n";
    std::cout << "  targetUser.id = " << targetUser.id << "\n";
    std::cout << "  targetUser.username = '" << targetUser.username << "'\n";
    std::cout << "  targetUser.email = '" << targetUser.email << "'\n";
    std::cout << "  targetUser.age = " << targetUser.age << "\n\n";

    // ============================================================
    // ШАГ 6: СРАВНЕНИЕ ОБЪЕКТОВ
    // ============================================================

    std::cout << "ШАГ 6: Сравнение объектов\n";
    std::cout << "-------------------------\n";

    User user1;
    user1.id = 1;
    user1.username = "test";
    user1.email = "test@example.com";
    user1.age = 20;

    User user2;
    user2.id = 1;
    user2.username = "test";
    user2.email = "test@example.com";
    user2.age = 20;

    bool areEqual = true;
    userEntity.for_each([&user1, &user2, &areEqual](const auto& prop) {
        auto value1 = prop.value(user1);
        auto value2 = prop.value(user2);

        if(value1 != value2) {
            areEqual = false;
        }
    });

    std::cout << "Сравнение user1 и user2: "
              << (areEqual ? "равны" : "не равны") << "\n\n";

    // ============================================================
    // ШАГ 7: ЭКСПОРТ В MAP (ДЛЯ JSON ИЛИ ДРУГИХ ФОРМАТОВ)
    // ============================================================

    std::cout << "ШАГ 7: Экспорт в map (для JSON или других форматов)\n";
    std::cout << "---------------------------------------------------\n";

    std::map<std::string, std::string> exportMap;

    userEntity.for_each([&user, &exportMap](const auto& prop) {
        auto value = prop.value(user);
        auto converter = prop.property_converter();
        std::string strValue = converter->convert_to_string(value);
        exportMap[prop.name()] = strValue;
    });

    std::cout << "Экспортированный map:\n";
    for(const auto& pair : exportMap) {
        std::cout << "  " << pair.first << " = '" << pair.second << "'\n";
    }
    std::cout << "\n";

    // ============================================================
    // ЗАКЛЮЧЕНИЕ
    // ============================================================
    //
    // В этом примере мы изучили практические сценарии:
    // 1. Генерацию SQL-запросов (ORM-подобная работа)
    // 2. Использование конвертеров типов
    // 3. Динамическое создание объектов из данных
    // 4. Копирование значений между объектами
    // 5. Сравнение объектов
    // 6. Экспорт в различные форматы
    //
    // Эти примеры показывают, как ReflectionApi может быть
    // использована в реальных приложениях для:
    // - ORM-систем
    // - Сериализации/десериализации
    // - Валидации данных
    // - Генерации кода
    // - Работы с формами и API

    std::cout << "========================================\n";
    std::cout << "Пример завершен успешно!\n";
    std::cout << "========================================\n";

    return 0;
}
