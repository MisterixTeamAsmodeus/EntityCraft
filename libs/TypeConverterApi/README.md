# TypeConverterApi

## Описание

Type Converter API - это C++ библиотека, предоставляющая удобные инструменты для конвертации между различными типами данных и строками, а также между различными контейнерами. 

Библиотека использует современные возможности C++14 (SFINAE, шаблоны, type traits) для обеспечения гибкости и расширяемости. Поддерживаются также типы из C++17 при компиляции с соответствующим стандартом.

## Основные возможности

### Конвертация простых типов

- **Встроенные числовые типы** с оптимизированной производительностью (без использования stringstream):
  - Целочисленные: `int`, `long`, `long long`, `short`, `char`
  - Беззнаковые: `unsigned int`, `unsigned long`, `unsigned long long`, `unsigned short`, `unsigned char`
  - С плавающей точкой: `float`, `double`, `long double`
- **Специальная обработка bool** (с поддержкой "true"/"false", "1"/"0", "True"/"False", "TRUE"/"FALSE")
- **Поддержка enum-типов** через базовый тип (автоматическое определение underlying type)
- **Работа со строками**:
  - `std::string`
  - `std::wstring` (широкие строки, конвертация UTF-8 ↔ wstring)
  - `std::u16string` (UTF-16, конвертация UTF-8 ↔ UTF-16)
  - `std::u32string` (UTF-32, конвертация UTF-8 ↔ UTF-32)
- **Конвертация умных указателей**:
  - `std::shared_ptr<T>`
  - `std::unique_ptr<T>`
  - `T*` (с предупреждением о необходимости управления памятью)

### Конвертация контейнеров

- **Последовательные контейнеры**:
  - `std::vector<T>` (формат: "item1,item2,item3" или "item1|item2|item3")
  - `std::array<T, N>` (фиксированный размер)
  - `std::deque<T>`
  - `std::list<T>`
- **Ассоциативные контейнеры**:
  - `std::set<T>`
  - `std::multiset<T>`
  - `std::map<K, V>` (формат: "key1:value1,key2:value2")
  - `std::unordered_map<K, V>`
- **Парные типы**:
  - `std::pair<A, B>` (формат: "first:second" или "first,second")

### C++17 типы (требуется компилятор с поддержкой C++17)

- `std::optional<T>` (формат: "value" или "null"/"nullopt"/"none" для отсутствия значения)
- `std::variant<Types...>` (формат: "type_index:value" или автоматическое определение типа)
- `std::string_view` (только конвертация в строку, fill_from_string не поддерживается)

### Qt типы (опционально, требуется определение USE_TYPE_QT)

- `QDateTime` (с настраиваемым форматом, по умолчанию "yyyy-MM-dd HH:mm:ss.zzz")
- `QDate` (с настраиваемым форматом, по умолчанию "yyyy-MM-dd")
- `QString`
- `QByteArray`
- `QJsonObject` (конвертация из/в JSON строку)

### Конвертация между контейнерами

Библиотека предоставляет класс `container_converter` для конвертации между различными типами контейнеров:

- Конвертация между последовательными контейнерами (`vector`, `list`, `deque`, `array`)
- Конвертация между ассоциативными контейнерами (`set`, `multiset`)
- Конвертация между контейнерами с изменением типа элементов
- Оптимизированная конвертация для числовых типов (использует `static_cast` вместо строковой конвертации)
- Поддержка контейнеров с оператором `<<`

### Утилиты и свободные функции

- `to_string<T>(value)` - конвертация значения в строку
- `from_string<T>(value, str)` - заполнение значения из строки
- `from_string<T>(str)` - создание значения из строки
- `container_to_string(container, delimiter)` - конвертация контейнера в строку с настраиваемым разделителем
- `container_from_string(container, str, delimiter)` - заполнение контейнера из строки с настраиваемым разделителем
- `string_convert<TargetString>(source)` - конвертация между строковыми типами (string, wstring, u16string, u32string)

## Расширяемость

- Возможность добавления специализаций для пользовательских типов через специализацию `type_converter<T>`
- Автоматическая поддержка типов с операторами `<<` и `>>` через stringstream (fallback)

## Преимущества

- **Производительность** - оптимизированные специализации для встроенных типов
- **Типобезопасность** - все преобразования выполняются с проверкой типов на этапе компиляции
- **Гибкость** - можно легко добавлять поддержку новых типов через специализации шаблонов
- **Обработка ошибок** - понятные сообщения об ошибках с использованием `std::invalid_argument` и `std::out_of_range`
- **Современный C++** - использование SFINAE, type traits, variadic templates
- **Удобный API** - свободные функции для упрощения использования

## Примеры использования

### Базовое использование

```cpp
#include <TypeConverterApi/typeconverterapi.hpp>

// Конвертация простых типов
int value = 42;
std::string str = type_converter_api::to_string(value); // "42"

int parsed;
type_converter_api::from_string(parsed, "123"); // parsed = 123

// Использование класса напрямую
type_converter_api::type_converter<int> converter;
std::string result = converter.convert_to_string(42);
```

### Работа с контейнерами

```cpp
// Вектор
std::vector<int> vec = {1, 2, 3};
std::string vec_str = type_converter_api::to_string(vec); // "1,2,3"

std::vector<int> vec2;
type_converter_api::from_string(vec2, "4,5,6"); // vec2 = {4, 5, 6}

// Array
std::array<int, 3> arr;
type_converter_api::from_string(arr, "1,2,3"); // arr = {1, 2, 3}

// Map
std::map<std::string, int> map;
type_converter_api::from_string(map, "one:1,two:2,three:3");

// Pair
std::pair<int, std::string> pair;
type_converter_api::from_string(pair, "42:answer"); // pair = {42, "answer"}

// Set
std::set<int> set;
type_converter_api::from_string(set, "1,2,3,2,1"); // set = {1, 2, 3}
```

### Работа с широкими строками

```cpp
std::wstring wstr = L"Привет";
std::string str = type_converter_api::to_string(wstr); // UTF-8

std::u16string u16str;
type_converter_api::from_string(u16str, "Hello"); // Конвертация из UTF-8
```

### Конвертация между контейнерами

```cpp
// Конвертация vector<int> в list<int>
std::vector<int> source = {1, 2, 3};
std::list<int> target;
type_converter_api::container_converter<std::list<int>> converter;
converter.convert_to_target(target, source);

// Конвертация с изменением типа элементов
std::vector<int> source = {1, 2, 3};
std::vector<long> target;
type_converter_api::container_converter<std::vector<long>> converter;
converter.convert_to_target(target, source);

// Упрощенный синтаксис
auto target = type_converter_api::container_converter<std::list<int>>().convert(source);
```

### Работа с C++17 типами

```cpp
#if __cplusplus >= 201703L
// Optional
std::optional<int> opt;
type_converter_api::from_string(opt, "42"); // opt = 42
type_converter_api::from_string(opt, "null"); // opt = std::nullopt

// Variant
std::variant<int, std::string, double> var;
type_converter_api::from_string(var, "42"); // var = 42 (int)
type_converter_api::from_string(var, "0:hello"); // var = "hello" (string, индекс 0)
type_converter_api::from_string(var, "2:3.14"); // var = 3.14 (double, индекс 2)
#endif
```

### Работа с Qt типами (если USE_TYPE_QT определен)

```cpp
#ifdef USE_TYPE_QT
// QDateTime
QDateTime dt;
type_converter_api::type_converter<QDateTime> converter("yyyy-MM-dd HH:mm:ss");
converter.fill_from_string(dt, "2024-01-01 12:00:00");

// QString
QString qstr;
type_converter_api::from_string(qstr, "Hello");

// QJsonObject
QJsonObject json;
type_converter_api::from_string(json, R"({"key": "value"})");
#endif
```

### Утилиты

```cpp
// Массовая конвертация контейнера
std::vector<int> vec = {1, 2, 3};
std::string result = type_converter_api::container_to_string(vec, "|"); // "1|2|3"

// Конвертация между строковыми типами
std::string str = "Hello";
std::wstring wstr = type_converter_api::string_convert<std::wstring>(str);
std::u16string u16str = type_converter_api::string_convert<std::u16string>(str);
```

## Требования

- Компилятор с поддержкой **C++14** или новее
  - Для C++17 типов (`optional`, `variant`, `string_view`) требуется **C++17** или новее
- Стандартная библиотека C++
- Для Qt типов (опционально):
  - Qt 5.x или новее
  - Автоматическое определение макроса `USE_TYPE_QT` при компиляции

## Установка

Библиотека является header-only, для использования достаточно подключить основной заголовочный файл:

```cpp
#include <TypeConverterApi/typeconverterapi.hpp>
```

## Обработка ошибок

Библиотека выбрасывает следующие исключения:

- `std::invalid_argument` - когда строка не может быть преобразована в требуемый тип
- `std::out_of_range` - когда значение выходит за границы типа
- `std::runtime_error` - когда преобразование не поддерживается для типа или контейнера
- `std::bad_alloc` - при ошибках выделения памяти (для сырых указателей T*)

Все исключения содержат информативные сообщения об ошибках, включая тип, который не удалось преобразовать, и входную строку (если применимо).

## Best Practices

1. **Используйте умные указатели** вместо сырых указателей (`std::shared_ptr`, `std::unique_ptr`)
2. **Обрабатывайте исключения** при парсинге пользовательского ввода
3. **Используйте свободные функции** (`to_string`, `from_string`) для упрощения кода
4. **Создавайте специализации** для пользовательских типов при необходимости:
   ```cpp
   namespace type_converter_api {
       template<>
       class type_converter<MyCustomType> {
           // реализация
       };
   }
   ```
5. **Используйте `container_converter`** для эффективной конвертации между контейнерами
6. **Для числовых типов** библиотека автоматически использует оптимизированные пути конвертации

## Лицензия

Библиотека распространяется под лицензией MIT. Подробности см. в файле LICENSE.