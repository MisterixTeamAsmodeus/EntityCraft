/**
 * ПРИМЕР 1: Базовое использование TypeConverterApi
 * 
 * Этот пример демонстрирует основные возможности библиотеки TypeConverterApi:
 * 
 * 1. КОНВЕРТАЦИЯ ПРОСТЫХ ТИПОВ
 *    - Числовые типы (int, double, float и т.д.)
 *    - Булевы значения
 *    - Строки (std::string, std::wstring)
 * 
 * 2. КОНВЕРТАЦИЯ КОНТЕЙНЕРОВ
 *    - std::vector
 *    - std::array
 *    - std::map
 *    - std::set
 *    - std::pair
 * 
 * 3. КОНВЕРТАЦИЯ МЕЖДУ КОНТЕЙНЕРАМИ
 *    - Конвертация между различными типами контейнеров
 *    - Конвертация с изменением типа элементов
 * 
 * 4. УТИЛИТЫ
 *    - Свободные функции to_string и from_string
 *    - Конвертация между строковыми типами
 */

#include <TypeConverterApi/typeconverterapi.hpp>
#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <list>
#include <deque>
#include <string>

int main()
{
    std::cout << "========================================\n";
    std::cout << "ПРИМЕР 1: Базовое использование TypeConverterApi\n";
    std::cout << "========================================\n\n";

    // ============================================================
    // ШАГ 1: КОНВЕРТАЦИЯ ПРОСТЫХ ТИПОВ
    // ============================================================
    std::cout << "ШАГ 1: Конвертация простых типов\n";
    std::cout << "--------------------------------\n";

    // Конвертация числовых типов
    int intValue = 42;
    std::string intStr = type_converter_api::to_string(intValue);
    std::cout << "int 42 -> string: \"" << intStr << "\"\n";

    double doubleValue = 3.14159;
    std::string doubleStr = type_converter_api::to_string(doubleValue);
    std::cout << "double 3.14159 -> string: \"" << doubleStr << "\"\n";

    // Обратная конвертация
    int parsedInt;
    type_converter_api::from_string(parsedInt, "123");
    std::cout << "string \"123\" -> int: " << parsedInt << "\n";

    double parsedDouble;
    type_converter_api::from_string(parsedDouble, "2.718");
    std::cout << "string \"2.718\" -> double: " << parsedDouble << "\n";

    // Конвертация bool
    bool boolValue = true;
    std::string boolStr = type_converter_api::to_string(boolValue);
    std::cout << "bool true -> string: \"" << boolStr << "\"\n";

    bool parsedBool;
    type_converter_api::from_string(parsedBool, "false");
    std::cout << "string \"false\" -> bool: " << (parsedBool ? "true" : "false") << "\n";

    // Конвертация строк
    std::string strValue = "Hello, World!";
    std::string strStr = type_converter_api::to_string(strValue);
    std::cout << "string \"Hello, World!\" -> string: \"" << strStr << "\"\n\n";

    // ============================================================
    // ШАГ 2: КОНВЕРТАЦИЯ КОНТЕЙНЕРОВ - VECTOR
    // ============================================================
    std::cout << "ШАГ 2: Конвертация контейнеров - vector\n";
    std::cout << "----------------------------------------\n";

    // Vector в строку
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::string vecStr = type_converter_api::to_string(vec);
    std::cout << "vector<int>{1, 2, 3, 4, 5} -> string: \"" << vecStr << "\"\n";

    // Строка в vector
    std::vector<int> vec2;
    type_converter_api::from_string(vec2, "10,20,30");
    std::cout << "string \"10,20,30\" -> vector<int>: ";
    for (size_t i = 0; i < vec2.size(); ++i) {
        std::cout << vec2[i];
        if (i < vec2.size() - 1) std::cout << ", ";
    }
    std::cout << "\n";

    // Vector строк
    std::vector<std::string> strVec = {"apple", "banana", "cherry"};
    std::string strVecStr = type_converter_api::to_string(strVec);
    std::cout << "vector<string>{\"apple\", \"banana\", \"cherry\"} -> string: \"" << strVecStr << "\"\n\n";

    // ============================================================
    // ШАГ 3: КОНВЕРТАЦИЯ КОНТЕЙНЕРОВ - ARRAY
    // ============================================================
    std::cout << "ШАГ 3: Конвертация контейнеров - array\n";
    std::cout << "---------------------------------------\n";

    std::array<int, 5> arr;
    type_converter_api::from_string(arr, "1,2,3,4,5");
    std::cout << "string \"1,2,3,4,5\" -> array<int, 5>: ";
    for (size_t i = 0; i < arr.size(); ++i) {
        std::cout << arr[i];
        if (i < arr.size() - 1) std::cout << ", ";
    }
    std::cout << "\n";

    std::string arrStr = type_converter_api::to_string(arr);
    std::cout << "array<int, 5>{1, 2, 3, 4, 5} -> string: \"" << arrStr << "\"\n\n";

    // ============================================================
    // ШАГ 4: КОНВЕРТАЦИЯ КОНТЕЙНЕРОВ - MAP
    // ============================================================
    std::cout << "ШАГ 4: Конвертация контейнеров - map\n";
    std::cout << "------------------------------------\n";

    std::map<std::string, int> map;
    type_converter_api::from_string(map, "one:1,two:2,three:3");
    std::cout << "string \"one:1,two:2,three:3\" -> map<string, int>:\n";
    for (const auto& pair : map) {
        std::cout << "  \"" << pair.first << "\" -> " << pair.second << "\n";
    }

    std::string mapStr = type_converter_api::to_string(map);
    std::cout << "map -> string: \"" << mapStr << "\"\n\n";

    // ============================================================
    // ШАГ 5: КОНВЕРТАЦИЯ КОНТЕЙНЕРОВ - SET
    // ============================================================
    std::cout << "ШАГ 5: Конвертация контейнеров - set\n";
    std::cout << "------------------------------------\n";

    std::set<int> set;
    type_converter_api::from_string(set, "3,1,4,1,5,9,2,6");
    std::cout << "string \"3,1,4,1,5,9,2,6\" -> set<int>: ";
    bool first = true;
    for (const auto& value : set) {
        if (!first) std::cout << ", ";
        std::cout << value;
        first = false;
    }
    std::cout << "\n";

    std::string setStr = type_converter_api::to_string(set);
    std::cout << "set -> string: \"" << setStr << "\"\n\n";

    // ============================================================
    // ШАГ 6: КОНВЕРТАЦИЯ КОНТЕЙНЕРОВ - PAIR
    // ============================================================
    std::cout << "ШАГ 6: Конвертация контейнеров - pair\n";
    std::cout << "-------------------------------------\n";

    std::pair<int, std::string> pair;
    type_converter_api::from_string(pair, "42:answer");
    std::cout << "string \"42:answer\" -> pair<int, string>: {" 
              << pair.first << ", \"" << pair.second << "\"}\n";

    std::string pairStr = type_converter_api::to_string(pair);
    std::cout << "pair -> string: \"" << pairStr << "\"\n\n";

    // ============================================================
    // ШАГ 7: КОНВЕРТАЦИЯ МЕЖДУ КОНТЕЙНЕРАМИ
    // ============================================================
    std::cout << "ШАГ 7: Конвертация между контейнерами\n";
    std::cout << "-------------------------------------\n";

    // Конвертация vector в list
    std::vector<int> sourceVec = {10, 20, 30, 40, 50};
    std::list<int> targetList;
    type_converter_api::container_converter<std::list<int>> converter1;
    converter1.convert_to_target(targetList, sourceVec);
    
    std::cout << "vector<int>{10, 20, 30, 40, 50} -> list<int>: ";
    first = true;
    for (const auto& value : targetList) {
        if (!first) std::cout << ", ";
        std::cout << value;
        first = false;
    }
    std::cout << "\n";

    // Упрощенный синтаксис
    auto convertedList = type_converter_api::container_converter<std::list<int>>().convert(sourceVec);
    std::cout << "Упрощенный синтаксис (convert): ";
    first = true;
    for (const auto& value : convertedList) {
        if (!first) std::cout << ", ";
        std::cout << value;
        first = false;
    }
    std::cout << "\n";

    // Конвертация vector в deque
    std::deque<int> targetDeque;
    type_converter_api::container_converter<std::deque<int>> converter2;
    converter2.convert_to_target(targetDeque, sourceVec);
    
    std::cout << "vector<int>{10, 20, 30, 40, 50} -> deque<int>: ";
    first = true;
    for (const auto& value : targetDeque) {
        if (!first) std::cout << ", ";
        std::cout << value;
        first = false;
    }
    std::cout << "\n";

    // Конвертация с изменением типа элементов
    std::vector<int> intVec = {1, 2, 3, 4, 5};
    std::vector<long> longVec;
    type_converter_api::container_converter<std::vector<long>> converter3;
    converter3.convert_to_target(longVec, intVec);
    
    std::cout << "vector<int>{1, 2, 3, 4, 5} -> vector<long>: ";
    for (size_t i = 0; i < longVec.size(); ++i) {
        std::cout << longVec[i];
        if (i < longVec.size() - 1) std::cout << ", ";
    }
    std::cout << "\n\n";

    // ============================================================
    // ШАГ 8: УТИЛИТЫ - КОНВЕРТАЦИЯ МЕЖДУ СТРОКОВЫМИ ТИПАМИ
    // ============================================================
    std::cout << "ШАГ 8: Утилиты - конвертация между строковыми типами\n";
    std::cout << "----------------------------------------------------\n";

    std::string utf8Str = "Привет, мир!";
    std::wstring wstr = type_converter_api::string_convert<std::wstring>(utf8Str);
    std::cout << "string \"Привет, мир!\" -> wstring: ";
    std::wcout << wstr << L"\n";

    std::string backToUtf8 = type_converter_api::string_convert<std::string>(wstr);
    std::cout << "wstring -> string: \"" << backToUtf8 << "\"\n\n";

    // ============================================================
    // ШАГ 9: УТИЛИТЫ - КОНВЕРТАЦИЯ КОНТЕЙНЕРА С РАЗДЕЛИТЕЛЕМ
    // ============================================================
    std::cout << "ШАГ 9: Утилиты - конвертация контейнера с разделителем\n";
    std::cout << "------------------------------------------------------\n";

    std::vector<std::string> items = {"apple", "banana", "cherry"};
    std::string itemsStr = type_converter_api::container_to_string(items, " | ");
    std::cout << "vector<string> с разделителем \" | \": \"" << itemsStr << "\"\n";

    std::vector<std::string> parsedItems;
    type_converter_api::container_from_string(parsedItems, "one | two | three", " | ");
    std::cout << "string \"one | two | three\" -> vector<string>: ";
    for (size_t i = 0; i < parsedItems.size(); ++i) {
        std::cout << "\"" << parsedItems[i] << "\"";
        if (i < parsedItems.size() - 1) std::cout << ", ";
    }
    std::cout << "\n\n";

    // ============================================================
    // ЗАКЛЮЧЕНИЕ
    // ============================================================
    std::cout << "========================================\n";
    std::cout << "Пример завершен успешно!\n";
    std::cout << "========================================\n";
    std::cout << "\nВ этом примере мы изучили:\n";
    std::cout << "1. Конвертацию простых типов (int, double, bool, string)\n";
    std::cout << "2. Конвертацию контейнеров (vector, array, map, set, pair)\n";
    std::cout << "3. Конвертацию между различными типами контейнеров\n";
    std::cout << "4. Утилиты для работы со строками и контейнерами\n";
    std::cout << "\nБиблиотека TypeConverterApi предоставляет мощные инструменты\n";
    std::cout << "для конвертации между различными типами данных и строками!\n";

    return 0;
}

