# SqliteAdapter

## Описание

SqliteAdapter - это реализация библиотеки DatabaseAdapter для работы с SQLite базами данных. Библиотека предоставляет удобный и типобезопасный интерфейс для взаимодействия с SQLite через стандартизированный API DatabaseAdapter.

Реализация основана на библиотеке sqlite-amalgamation. Всё взаимодействие с базой данных реализовано через нативный SQLite C API, что обеспечивает высокую производительность и надежность.

## Основные возможности

- **Полная реализация интерфейса IConnection** - поддержка всех операций DatabaseAdapter
- **Потокобезопасная работа** - все операции с подготовленными запросами защищены мьютексами
- **Пул соединений** - эффективное управление множественными подключениями к SQLite базе
- **Поддержка транзакций** - полная поддержка транзакций с различными уровнями изоляции
- **Подготовленные запросы** - кэширование и оптимизация часто выполняемых запросов
- **Автоматическое определение типов** - автоматическое определение и привязка типов параметров
- **Обработка ошибок SQLite** - удобные функции для работы с кодами ошибок SQLite
- **Логирование** - поддержка кастомных логгеров для отладки и мониторинга
- **RAII-управление ресурсами** - автоматическое освобождение ресурсов при уничтожении объектов

## Требования

- **C++14** или выше
- **CMake 3.10** или выше
- **SQLite3** библиотека (версия 3.x)
- **DatabaseAdapter** - базовая библиотека (зависимость)

### Получение SQLite3

Библиотека поддерживает три способа получения SQLite3:

1. **INSTALLED** (по умолчанию) - использование установленной библиотеки через `find_package(unofficial-sqlite3)`
2. **CUSTOM** - использование пользовательской установки SQLite3
3. **EXTERNAL** - автоматическая загрузка через FetchContent из репозитория sqlite-amalgamation

Настройка через CMake переменную:
```cmake
set(ENTITY_CRAFT_SQLITE3_MODULES_LOCATION "INSTALLED" CACHE STRING "Location where to find SQLite3 modules")
```

## Установка

### Через CMake

```cmake
# Свяжите с вашим проектом
target_link_libraries(your_target SqliteAdapter)
```

### Использование

```cpp
#include <SqliteAdapter/sqliteadapter.hpp>
```

## Быстрый старт

### Базовый пример

```cpp
#include <SqliteAdapter/sqliteadapter.hpp>
#include <DatabaseAdapter/transaction_guard.hpp>
#include <iostream>

int main()
{
    try {
        // Настройки подключения к SQLite
        database_adapter::sqlite::settings settings;
        settings.url = "example.db"; // Путь к файлу базы данных
        
        // Создание подключения
        auto connection = std::make_shared<database_adapter::sqlite::connection>(settings);
        
        // Проверка валидности
        if(!connection->is_valid()) {
            std::cerr << "Ошибка подключения к базе данных" << std::endl;
            return 1;
        }
        
        // Создание таблицы
        connection->exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)");
        
        // Вставка данных
        connection->exec("INSERT INTO users (name, age) VALUES ('Иван', 25)");
        
        // Выборка данных
        auto result = connection->exec("SELECT * FROM users");
        for(const auto& row : result) {
            std::cout << "ID: " << row.at("id") 
                      << ", Name: " << row.at("name")
                      << ", Age: " << row.at("age") << std::endl;
        }
        
    } catch(const database_adapter::sql_exception& e) {
        std::cerr << "Ошибка SQL: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

## Настройка подключения

Для подключения к SQLite базе данных используется структура `database_connection_settings`:

```cpp
database_adapter::sqlite::settings settings;
settings.url = "path/to/database.db";  // Путь к файлу базы данных
// Для SQLite остальные поля (database_name, port, login, password) не используются
```

**Примечание**: SQLite не требует аутентификации, поэтому поля `login` и `password` игнорируются. Поле `url` должно содержать путь к файлу базы данных.

## API Справочник

### Класс connection

Основной класс для работы с SQLite базой данных, реализующий интерфейс `IConnection`.

#### Конструктор

```cpp
explicit connection(const settings& settings);
```

Создает новое подключение к SQLite базе данных. При создании автоматически открывается соединение.

**Параметры:**
- `settings` - настройки подключения к базе данных

**Исключения:**
- `open_database_exception` - при ошибке подключения к базе данных

#### Методы

##### `bool is_valid()`

Проверяет валидность соединения с базой данных.

**Возвращает:** `true` если соединение активно и готово к работе, иначе `false`

##### `query_result exec(const std::string& query)`

Выполняет SQL-запрос и возвращает результат.

**Параметры:**
- `query` - SQL-запрос для выполнения

**Возвращает:** объект `query_result` с результатами запроса

**Исключения:**
- `sql_exception` - при ошибке выполнения запроса

**Пример:**
```cpp
auto result = connection->exec("SELECT * FROM users WHERE age > 18");
```

##### `void prepare(const std::string& query, const std::string& name)`

Подготавливает SQL-запрос для повторного использования.

**Параметры:**
- `query` - SQL-запрос с параметрами (используется `?` для параметров)
- `name` - уникальное имя для сохранения подготовленного запроса

**Исключения:**
- `sql_exception` - при ошибке подготовки запроса

**Пример:**
```cpp
connection->prepare("INSERT INTO users (name, age) VALUES (?, ?)", "insert_user");
```

##### `query_result exec_prepared(const std::vector<std::string>& params, const std::string& name)`

Выполняет подготовленный запрос с указанными параметрами.

**Параметры:**
- `params` - вектор строковых значений параметров (порядок соответствует `?` в запросе)
- `name` - имя подготовленного запроса

**Возвращает:** объект `query_result` с результатами запроса

**Исключения:**
- `sql_exception` - при ошибке выполнения запроса или если запрос с таким именем не найден

**Пример:**
```cpp
connection->prepare("SELECT * FROM users WHERE age > ?", "get_adults");
auto result = connection->exec_prepared({"18"}, "get_adults");
```

##### `bool open_transaction(transaction_isolation_level level)`

Открывает транзакцию с указанным уровнем изоляции.

**Параметры:**
- `level` - уровень изоляции транзакции

**Возвращает:** `true` если транзакция успешно открыта, иначе `false`

**Примечание:** SQLite поддерживает ограниченные уровни изоляции. Большинство уровней изоляции транслируются в стандартное поведение SQLite.

#### Статические методы

##### `static void set_logger(std::shared_ptr<ILogger>&& logger)`

Устанавливает глобальный логгер для всех соединений SQLite.

**Параметры:**
- `logger` - указатель на объект логгера, реализующий интерфейс `ILogger`

**Пример:**
```cpp
class MyLogger : public database_adapter::ILogger {
public:
    void log_error(const std::string& message) override {
        std::cerr << "[SQLite ERROR] " << message << std::endl;
    }
    
    void log_sql(const std::string& message) override {
        std::cout << "[SQLite SQL] " << message << std::endl;
    }
};

database_adapter::sqlite::connection::set_logger(std::make_shared<MyLogger>());
```

### Класс connection_pool

Пул соединений для эффективного управления множественными подключениями к SQLite базе данных.

#### Статические настройки

```cpp
static database_connection_settings connection_settings;  // Настройки подключения
static size_t start_pool_size;                             // Начальный размер пула
static size_t max_pool_size;                               // Максимальный размер пула
static std::chrono::seconds wait_time;                     // Время ожидания доступного соединения
static std::chrono::seconds idle_timeout;                  // Таймаут простоя соединения
```

#### Методы

##### `static std::shared_ptr<connection_pool> instance()`

Получает единственный экземпляр пула соединений (Singleton).

**Возвращает:** указатель на пул соединений

**Пример:**
```cpp
database_adapter::sqlite::connection_pool::connection_settings.url = "example.db";
database_adapter::sqlite::connection_pool::start_pool_size = 2;
database_adapter::sqlite::connection_pool::max_pool_size = 10;

auto pool = database_adapter::sqlite::connection_pool::instance();
auto connection = pool->open_connection();
```

##### `explicit connection_pool(database_connection_settings settings, ...)`

Создает новый пул соединений с указанными параметрами.

**Параметры:**
- `settings` - настройки подключения к базе данных
- `start_pool_size` - начальный размер пула (по умолчанию 2)
- `max_pool_size` - максимальный размер пула (по умолчанию 10)
- `wait_time` - время ожидания доступного соединения (по умолчанию 2 секунды)
- `idle_timeout` - таймаут простоя соединения (по умолчанию 300 секунд)

### Работа с кодами ошибок SQLite

Библиотека предоставляет удобные функции для работы с кодами ошибок SQLite:

#### `sqlite_error_code to_sqlite_error_code(int code)`

Преобразует целочисленный код ошибки SQLite в enum `sqlite_error_code`.

**Параметры:**
- `code` - целочисленный код ошибки SQLite

**Возвращает:** соответствующий enum `sqlite_error_code`

#### `int to_int(sqlite_error_code code)`

Преобразует enum `sqlite_error_code` в целочисленный код ошибки SQLite.

**Параметры:**
- `code` - enum код ошибки

**Возвращает:** целочисленный код ошибки SQLite

#### `bool is_success(sqlite_error_code code)`

Проверяет, является ли код ошибки успешным результатом.

**Параметры:**
- `code` - код ошибки для проверки

**Возвращает:** `true` если код указывает на успех (OK, ROW, DONE), иначе `false`

#### `bool is_critical_error(sqlite_error_code code)`

Проверяет, является ли код ошибки критической ошибкой.

**Параметры:**
- `code` - код ошибки для проверки

**Возвращает:** `true` если код указывает на критическую ошибку, иначе `false`

**Пример:**
```cpp
try {
    connection->exec("SELECT * FROM nonexistent_table");
} catch(const database_adapter::sql_exception& e) {
    auto error_code = database_adapter::sqlite::to_sqlite_error_code(e.error_code());
    
    if(database_adapter::sqlite::is_critical_error(error_code)) {
        std::cerr << "Критическая ошибка SQLite!" << std::endl;
    }
}
```

## Примеры использования

### Работа с транзакциями

```cpp
#include <DatabaseAdapter/transaction_guard.hpp>

// Использование RAII-обертки для транзакций
{
    database_adapter::transaction_guard guard(connection);
    
    connection->exec("INSERT INTO users (name, age) VALUES ('Иван', 25)");
    connection->exec("INSERT INTO users (name, age) VALUES ('Мария', 30)");
    
    // Сommit до выхода из области видимости
    guard.commit();
} // Если commit не был вызван, автоматически выполнится rollback
```

### Использование prepared statements

```cpp
// Подготовка запроса
connection->prepare("INSERT INTO users (name, age) VALUES (?, ?)", "insert_user");

// Множественное выполнение с разными параметрами
for(int i = 0; i < 10; ++i) {
    connection->exec_prepared(
        {"User" + std::to_string(i), std::to_string(20 + i)},
        "insert_user"
    );
}
```

### Использование connection pool

```cpp
// Настройка пула
database_adapter::sqlite::connection_pool::connection_settings.url = "example.db";
database_adapter::sqlite::connection_pool::start_pool_size = 3;
database_adapter::sqlite::connection_pool::max_pool_size = 10;

// Получение пула
auto pool = database_adapter::sqlite::connection_pool::instance();

// Получение соединений из пула
std::vector<std::shared_ptr<database_adapter::IConnection>> connections;
for(int i = 0; i < 5; ++i) {
    auto conn = pool->open_connection();
    if(conn) {
        connections.push_back(conn);
        // Использование соединения
        conn->exec("SELECT * FROM users");
    }
}

// Соединения автоматически вернутся в пул при уничтожении shared_ptr
connections.clear();
```

### Обработка результатов запросов

```cpp
auto result = connection->exec("SELECT id, name, age FROM users WHERE age > ?");

// Проверка на пустоту
if(result.empty()) {
    std::cout << "Результат пуст" << std::endl;
    return;
}

// Итерация по результатам
for(const auto& row : result) {
    std::cout << "ID: " << row.at("id") << std::endl;
    std::cout << "Name: " << row.at("name") << std::endl;
    std::cout << "Age: " << row.at("age") << std::endl;
}

// Доступ по индексу
if(!result.empty()) {
    const auto& first_row = result.at(0);
    std::string name = first_row.at("name");
}
```

## Особенности SQLite

### Уровни изоляции транзакций

SQLite поддерживает ограниченные уровни изоляции по сравнению с другими СУБД:

- **READ UNCOMMITTED** - не поддерживается, транслируется в стандартное поведение
- **READ COMMITTED** - стандартное поведение SQLite
- **REPEATABLE READ** - не поддерживается, транслируется в стандартное поведение
- **SERIALIZABLE** - стандартное поведение SQLite (по умолчанию)

**Примечание:** SQLite использует файловую блокировку для обеспечения целостности данных, что обеспечивает высокий уровень изоляции по умолчанию.

### Типы данных

SQLite использует динамическую типизацию. Библиотека автоматически определяет тип данных при привязке параметров и извлечении значений:

- **INTEGER** - целые числа
- **REAL** - числа с плавающей точкой
- **TEXT** - текстовые строки
- **BLOB** - бинарные данные
- **NULL** - значения NULL

Все значения возвращаются в виде строк, что соответствует интерфейсу `query_result`.

### Подготовленные запросы

SQLite использует позиционные параметры (`?`) вместо именованных. Библиотека автоматически обрабатывает привязку параметров с определением типов.

**Пример:**
```cpp
// SQLite использует ? для параметров
connection->prepare("SELECT * FROM users WHERE name = ? AND age > ?", "find_users");
auto result = connection->exec_prepared({"Иван", "18"}, "find_users");
```

## Обработка ошибок

Библиотека использует исключения для обработки ошибок, наследуемые от `std::exception`:

### `open_database_exception`

Выбрасывается при ошибке подключения к базе данных:

```cpp
try {
    database_adapter::sqlite::settings settings;
    settings.url = "/invalid/path/database.db";
    auto connection = std::make_shared<database_adapter::sqlite::connection>(settings);
} catch(const database_adapter::open_database_exception& e) {
    std::cerr << "Ошибка подключения: " << e.what() << std::endl;
    std::cerr << "Код ошибки: " << e.error_code() << std::endl;
}
```

### `sql_exception`

Выбрасывается при ошибке выполнения SQL-запроса:

```cpp
try {
    connection->exec("SELECT * FROM nonexistent_table");
} catch(const database_adapter::sql_exception& e) {
    std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    std::cerr << "Запрос: " << e.last_query() << std::endl;
    std::cerr << "Код ошибки: " << e.error_code() << std::endl;
    
    // Преобразование в код ошибки SQLite
    auto sqlite_code = database_adapter::sqlite::to_sqlite_error_code(e.error_code());
    if(sqlite_code == database_adapter::sqlite::sqlite_error_code::BUSY) {
        std::cerr << "База данных заблокирована" << std::endl;
    }
}
```

## Потокобезопасность

Библиотека обеспечивает потокобезопасность на следующих уровнях:

- **Подготовленные запросы** - все операции с подготовленными запросами защищены мьютексами
- **Пул соединений** - полностью потокобезопасен и может использоваться из нескольких потоков одновременно
- **Транзакции** - операции с транзакциями потокобезопасны (наследуется от DatabaseAdapter)

**Важно:** Хотя операции потокобезопасны, рекомендуется использовать одно соединение на поток для лучшей производительности, или использовать пул соединений.

## Производительность

### Рекомендации

1. **Используйте prepared statements** для часто выполняемых запросов
2. **Используйте connection pool** для многопоточных приложений
3. **Используйте транзакции** для группировки множественных операций
4. **Избегайте создания множественных соединений** - используйте пул соединений

### Пример оптимизации

```cpp
// ❌ Медленно - создание нового соединения для каждого запроса
for(int i = 0; i < 1000; ++i) {
    auto conn = std::make_shared<database_adapter::sqlite::connection>(settings);
    conn->exec("INSERT INTO users (name) VALUES ('User" + std::to_string(i) + "')");
}

// ✅ Быстро - использование пула и prepared statements
auto pool = database_adapter::sqlite::connection_pool::instance();
auto conn = pool->open_connection();
conn->prepare("INSERT INTO users (name) VALUES (?)", "insert_user");

{
    database_adapter::transaction_guard guard(conn);
    for(int i = 0; i < 1000; ++i) {
        conn->exec_prepared({"User" + std::to_string(i)}, "insert_user");
    }
    guard.commit();
}
```

## Полные примеры

Полные примеры использования библиотеки можно найти в директории `example/SqliteAdapter/` проекта EntityCraft:

- **example-1.cpp** - базовое использование SqliteAdapter с демонстрацией всех основных возможностей

Для сборки примеров:

```bash
cd example/SqliteAdapter
mkdir build && cd build
cmake ..
make
```

## Лицензия

Библиотека распространяется под лицензией MIT. Подробности см. в файле LICENSE в корне проекта.

## Поддержка

При возникновении вопросов или проблем создайте issue в репозитории проекта.
