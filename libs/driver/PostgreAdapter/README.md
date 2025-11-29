# PostgreAdapter

## Описание

PostgreAdapter - это реализация библиотеки DatabaseAdapter для работы с PostgreSQL базами данных. Библиотека предоставляет удобный и типобезопасный интерфейс для взаимодействия с PostgreSQL через стандартизированный API DatabaseAdapter.

Реализация основана на библиотеке libpq (PostgreSQL C API). Всё взаимодействие с базой данных реализовано через нативный PostgreSQL C API, что обеспечивает высокую производительность и надежность.

## Основные возможности

- **Полная реализация интерфейса IConnection** - поддержка всех операций DatabaseAdapter
- **Потокобезопасная работа** - все операции с подготовленными запросами защищены мьютексами
- **Пул соединений** - эффективное управление множественными подключениями к PostgreSQL базе
- **Поддержка транзакций** - полная поддержка транзакций с различными уровнями изоляции
- **Подготовленные запросы** - кэширование и оптимизация часто выполняемых запросов
- **Автоматическая очистка ресурсов** - автоматическое освобождение подготовленных запросов при закрытии соединения
- **Обработка ошибок PostgreSQL** - удобные функции для работы с кодами ошибок PostgreSQL (SQLSTATE)
- **Логирование** - поддержка кастомных логгеров для отладки и мониторинга
- **RAII-управление ресурсами** - автоматическое освобождение ресурсов при уничтожении объектов
- **Повторные попытки подключения** - автоматические повторные попытки при ошибках подключения

## Требования

- **C++14** или выше
- **CMake 3.10** или выше
- **PostgreSQL** библиотека (libpq, версия 9.x или выше)
- **DatabaseAdapter** - базовая библиотека (зависимость)

### Получение libpq

Библиотека поддерживает два способа получения libpq:

1. **INSTALLED** (по умолчанию) - использование установленной библиотеки через `find_package(PostgreSQL)`
2. **CUSTOM** - использование пользовательской установки libpq

Настройка через CMake переменную:
```cmake
set(ENTITY_CRAFT_POSTGRE_MODULES_LOCATION "INSTALLED" CACHE STRING "Location where to find libpq modules")
```

## Установка

### Через CMake

```cmake
# Свяжите с вашим проектом
target_link_libraries(your_target PostgreAdapter)
```

### Использование

```cpp
#include <PostgreAdapter/postgreadapter.hpp>
```

## Быстрый старт

### Базовый пример

```cpp
#include <PostgreAdapter/postgreadapter.hpp>
#include <DatabaseAdapter/transaction_guard.hpp>
#include <iostream>

int main()
{
    try {
        // Настройки подключения к PostgreSQL
        database_adapter::postgre::settings settings;
        settings.database_name = "mydb";
        settings.url = "localhost";
        settings.port = "5432";
        settings.login = "postgres";
        settings.password = "password";
        
        // Создание подключения
        auto connection = std::make_shared<database_adapter::postgre::connection>(settings);
        
        // Проверка валидности
        if(!connection->is_valid()) {
            std::cerr << "Ошибка подключения к базе данных" << std::endl;
            return 1;
        }
        
        // Создание таблицы
        connection->exec("CREATE TABLE IF NOT EXISTS users (id SERIAL PRIMARY KEY, name VARCHAR(100), age INTEGER)");
        
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
        std::cerr << "Код ошибки: " << e.error_code() << std::endl;
        return 1;
    } catch(const database_adapter::open_database_exception& e) {
        std::cerr << "Ошибка подключения: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

### Использование подготовленных запросов

```cpp
#include <PostgreAdapter/postgreadapter.hpp>

int main()
{
    database_adapter::postgre::settings settings;
    settings.database_name = "mydb";
    settings.url = "localhost";
    settings.port = "5432";
    settings.login = "postgres";
    settings.password = "password";
    
    auto connection = std::make_shared<database_adapter::postgre::connection>(settings);
    
    // Подготовка запроса
    connection->prepare(
        "INSERT INTO users (name, age) VALUES ($1, $2)",
        "insert_user"
    );
    
    // Выполнение подготовленного запроса
    connection->exec_prepared({"Петр", "30"}, "insert_user");
    connection->exec_prepared({"Мария", "28"}, "insert_user");
    
    return 0;
}
```

### Использование транзакций

```cpp
#include <PostgreAdapter/postgreadapter.hpp>
#include <DatabaseAdapter/transaction_guard.hpp>

int main()
{
    database_adapter::postgre::settings settings;
    // ... настройки подключения ...
    
    auto connection = std::make_shared<database_adapter::postgre::connection>(settings);
    
    // Использование transaction_guard для автоматического управления транзакцией
    {
        database_adapter::transaction_guard guard(connection);
        
        connection->exec("INSERT INTO users (name, age) VALUES ('Иван', 25)");
        connection->exec("INSERT INTO users (name, age) VALUES ('Петр', 30)");
        
        guard.commit(); // Фиксация изменений
        // При выходе из блока без commit() произойдет автоматический rollback
    }
    
    return 0;
}
```

### Использование пула соединений

```cpp
#include <PostgreAdapter/postgreadapter.hpp>

int main()
{
    database_adapter::postgre::settings settings;
    // ... настройки подключения ...
    
    // Настройка пула соединений
    database_adapter::postgre::connection_pool::connection_settings = settings;
    database_adapter::postgre::connection_pool::start_pool_size = 5;
    database_adapter::postgre::connection_pool::max_pool_size = 20;
    database_adapter::postgre::connection_pool::wait_time = std::chrono::seconds(10);
    
    // Получение экземпляра пула
    auto pool = database_adapter::postgre::connection_pool::instance();
    
    // Получение соединения из пула
    auto connection = pool->open_connection();
    if(connection) {
        // Использование соединения
        auto result = connection->exec("SELECT * FROM users");
        // Соединение автоматически вернется в пул при уничтожении shared_ptr
    }
    
    return 0;
}
```

## API

### Класс connection

#### Конструктор

```cpp
explicit connection(
    const settings& settings,
    bool needCreateDatabaseIfNotExist = false,
    int retryCount = 4,
    int retryDeltaSeconds = 2
);
```

Создает новое соединение с базой данных PostgreSQL.

**Параметры:**
- `settings` - настройки подключения к базе данных
- `needCreateDatabaseIfNotExist` - создавать ли базу данных, если она не существует
- `retryCount` - количество попыток подключения при ошибке
- `retryDeltaSeconds` - интервал между попытками подключения в секундах

**Исключения:**
- `open_database_exception` - при ошибке подключения к базе данных

#### Методы

##### `bool is_valid()`

Проверяет валидность соединения. Возвращает `true`, если соединение активно и готово к работе.

##### `query_result exec(const std::string& query)`

Выполняет SQL-запрос к базе данных.

**Параметры:**
- `query` - SQL-запрос для выполнения

**Возвращает:**
- Результат выполнения запроса в виде объекта `query_result`

**Исключения:**
- `sql_exception` - при ошибке выполнения запроса

##### `void prepare(const std::string& query, const std::string& name)`

Подготавливает SQL-запрос для последующего выполнения с параметрами.

**Параметры:**
- `query` - SQL-запрос для подготовки (используйте `$1`, `$2`, и т.д. для параметров)
- `name` - уникальное имя подготовленного запроса

**Исключения:**
- `sql_exception` - при ошибке подготовки запроса

##### `query_result exec_prepared(const std::vector<std::string>& params, const std::string& name)`

Выполняет подготовленный запрос с параметрами.

**Параметры:**
- `params` - вектор параметров для подстановки в запрос
- `name` - имя подготовленного запроса

**Возвращает:**
- Результат выполнения запроса в виде объекта `query_result`

**Исключения:**
- `sql_exception` - при ошибке выполнения или если запрос не был подготовлен

##### `bool open_transaction(transaction_isolation_level level)`

Открывает транзакцию с указанным уровнем изоляции.

**Параметры:**
- `level` - уровень изоляции транзакции

**Возвращает:**
- `true`, если транзакция успешно открыта, иначе `false`

#### Статические методы

##### `void set_logger(std::shared_ptr<ILogger>&& logger)`

Устанавливает глобальный логгер для всех соединений PostgreSQL.

### Класс connection_pool

Пул соединений для эффективного управления множественными подключениями к PostgreSQL.

#### Статические настройки

```cpp
static database_connection_settings connection_settings;
static size_t start_pool_size;
static size_t max_pool_size;
static std::chrono::seconds wait_time;
```

#### Статические методы

##### `std::shared_ptr<connection_pool> instance()`

Возвращает единственный экземпляр пула соединений (Singleton).

### Работа с кодами ошибок

Библиотека предоставляет удобные функции для работы с кодами статуса PostgreSQL:

```cpp
#include <PostgreAdapter/postgreerrorcode.hpp>

// Преобразование статуса выполнения запроса
postgre_exec_status status = to_postgre_exec_status(PQresultStatus(result));

// Проверка успешности
if(is_success(status)) {
    // Запрос выполнен успешно
}

// Проверка критической ошибки
if(is_critical_error(status)) {
    // Произошла критическая ошибка
}

// Преобразование статуса подключения
postgre_conn_status conn_status = to_postgre_conn_status(PQstatus(connection));

// Проверка успешного подключения
if(is_connection_ok(conn_status)) {
    // Подключение установлено
}
```

## Обработка ошибок

Библиотека использует исключения для обработки ошибок. Все исключения наследуются от `std::exception`:

### open_database_exception

Выбрасывается при ошибке подключения к базе данных:

```cpp
try {
    auto connection = std::make_shared<database_adapter::postgre::connection>(settings);
} catch(const open_database_exception& e) {
    std::cerr << "Ошибка подключения: " << e.what() << std::endl;
    std::cerr << "Код ошибки: " << e.error_code() << std::endl;
}
```

### sql_exception

Выбрасывается при ошибке выполнения SQL-запроса:

```cpp
try {
    query_result result = connection->exec("SELECT * FROM nonexistent_table");
} catch(const sql_exception& e) {
    std::cerr << "Ошибка SQL: " << e.what() << std::endl;
    std::cerr << "Запрос: " << e.last_query() << std::endl;
    std::cerr << "Код ошибки: " << e.error_code() << std::endl;
}
```

## Потокобезопасность

Библиотека обеспечивает потокобезопасность на нескольких уровнях:

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
    auto conn = std::make_shared<database_adapter::postgre::connection>(settings);
    conn->exec("INSERT INTO users (name) VALUES ('User" + std::to_string(i) + "')");
}

// ✅ Быстро - использование пула и prepared statements
auto pool = database_adapter::postgre::connection_pool::instance();
auto conn = pool->open_connection();
conn->prepare("INSERT INTO users (name) VALUES ($1)", "insert_user");
for(int i = 0; i < 1000; ++i) {
    conn->exec_prepared({"User" + std::to_string(i)}, "insert_user");
}
```

## Особенности реализации

### Управление памятью

- Все операции с памятью используют RAII-принципы
- Параметры подготовленных запросов управляются через `std::vector`
- Автоматическая очистка подготовленных запросов при закрытии соединения

### Повторные попытки подключения

Библиотека автоматически выполняет повторные попытки подключения при ошибках. Количество попыток и интервал между ними настраиваются в конструкторе.

### Очистка ресурсов

При закрытии соединения автоматически выполняется команда `DEALLOCATE ALL` для очистки всех подготовленных запросов на сервере PostgreSQL.

## Лицензия

См. основной файл LICENSE проекта.
