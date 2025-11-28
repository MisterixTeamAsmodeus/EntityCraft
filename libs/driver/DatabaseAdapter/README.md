# DatabaseAdapter

Библиотека DatabaseAdapter предоставляет обобщённое API для взаимодействия с различными реляционными базами данных. Она позволяет разработчикам легко интегрировать доступ к данным в свои приложения, обеспечивая гибкость и удобство использования различных источников данных.

## Основные возможности

- **Потокобезопасная работа с соединениями** - все операции с транзакциями защищены мьютексами
- **Пул соединений** - эффективное управление ресурсами с автоматическим переиспользованием соединений
- **Поддержка транзакций** - полная поддержка транзакций с различными уровнями изоляции
- **RAII-обертка для транзакций** - автоматическое управление жизненным циклом транзакций
- **Подготовленные запросы** - кэширование и оптимизация часто выполняемых запросов
- **Защита от SQL-инъекций** - валидация запросов и параметров
- **Типобезопасные уровни изоляции** - enum-класс для уровней изоляции транзакций
- **Оптимизированное хранение результатов** - эффективная структура данных для результатов запросов
- **Логирование** - поддержка кастомных логгеров для отладки и мониторинга

## Архитектура

Библиотека построена на основе паттерна адаптера и состоит из следующих основных компонентов:

- **`IConnection`** - базовый интерфейс для работы с соединением к базе данных
- **`IConnectionPool`** - пул соединений для управления множественными подключениями
- **`query_result`** - контейнер для хранения результатов SQL-запросов
- **`transaction_guard`** - RAII-обертка для автоматического управления транзакциями
- **`database_connection_settings`** - структура настроек подключения к базе данных

## Настройка соединения

Для подключения к базе данных необходимо создать объект `database_connection_settings`:

```cpp
database_adapter::database_connection_settings settings;
settings.database_name = "mydb";      // Имя базы данных
settings.url = "localhost";            // Адрес сервера или путь к файлу
settings.port = "5432";                // Порт (опционально)
settings.login = "user";               // Имя пользователя
settings.password = "password";        // Пароль
```

## API

### Работа с соединением

```cpp
#include "DatabaseAdapter/databaseadapter.h"

// Создание соединения (конкретная реализация зависит от адаптера)
auto connection = std::make_shared<MyConnection>(settings);

// Проверка валидности соединения
if(connection->is_valid()) {
    // Соединение активно и готово к работе
}

// Выполнение простого запроса
query_result result = connection->exec("SELECT * FROM users WHERE id = 1");

// Подготовка запроса для повторного использования
connection->prepare("SELECT * FROM users WHERE id = $1", "get_user_by_id");

// Выполнение подготовленного запроса
query_result user = connection->exec_prepared({"123"}, "get_user_by_id");
```

### Работа с транзакциями

#### Базовые операции

```cpp
// Открытие транзакции с уровнем изоляции по умолчанию
connection->begin_transaction();
// или
connection->open_base_transaction();

// Открытие транзакции с заданным уровнем изоляции
connection->open_transaction(transaction_isolation_level::SERIALIZABLE);

// Выполнение операций в транзакции
connection->exec("INSERT INTO users (name) VALUES ('John')");
connection->exec("UPDATE users SET name = 'Jane' WHERE id = 1");

// Проверка наличия активной транзакции
if(connection->is_transaction()) {
    // Транзакция активна
}

// Фиксация изменений
connection->commit();

// Откат изменений
connection->rollback();
```

#### RAII-обертка для транзакций

Класс `transaction_guard` обеспечивает автоматическое управление транзакциями:

```cpp
{
    // Автоматически открывает транзакцию
    transaction_guard guard(connection);
    
    connection->exec("INSERT INTO users (name) VALUES ('John')");
    connection->exec("UPDATE users SET name = 'Jane' WHERE id = 1");
    
    // Явный commit
    guard.commit();
} // Если commit не был вызван, автоматически выполнится rollback

// С заданным уровнем изоляции
{
    transaction_guard guard(connection, transaction_isolation_level::SERIALIZABLE);
    
    connection->exec("INSERT INTO users (name) VALUES ('John')");
    
    // Проверка состояния транзакции
    if(!guard.is_finished()) {
        guard.commit();
    }
}
```

#### Работа с savepoint

Savepoint позволяют создавать точки отката внутри транзакции:

```cpp
connection->begin_transaction();
connection->exec("INSERT INTO users (name) VALUES ('John')");

// Создание точки сохранения
connection->add_save_point("sp1");

connection->exec("UPDATE users SET name = 'Jane' WHERE id = 1");

// Откат до точки сохранения (отменяет только UPDATE)
connection->rollback_to_save_point("sp1");

// Откат всей транзакции (если передать пустую строку)
// connection->rollback_to_save_point("");

connection->commit();
```

### Пул соединений

Пул соединений (`IConnectionPool`) управляет множественными подключениями к базе данных, обеспечивая их переиспользование и эффективное распределение:

```cpp
database_connection_settings settings;
settings.database_name = "mydb";
settings.url = "localhost";
settings.login = "user";
settings.password = "password";

// Создание пула:
// - start_pool_size: начальное количество соединений (5)
// - max_pool_size: максимальное количество соединений (10)
// - wait_time: время ожидания доступного соединения (10 секунд)
// - idle_timeout: время простоя перед закрытием неиспользуемого соединения (300 секунд)
IConnectionPool pool(settings, 5, 10, std::chrono::seconds(10), std::chrono::seconds(300));

// Изменение настроек пула
pool.set_max_pool_size(20);
pool.set_wait_time(std::chrono::seconds(5));
pool.set_idle_timeout(std::chrono::seconds(600));

// Получение соединения из пула
auto connection = pool.open_connection();
if(connection) {
    query_result result = connection->exec("SELECT * FROM users");
    // Соединение автоматически вернется в пул при уничтожении shared_ptr
}

// Использование в многопоточном окружении
std::vector<std::thread> threads;
for(int i = 0; i < 10; ++i) {
    threads.emplace_back([&pool]() {
        auto conn = pool.open_connection();
        if(conn) {
            conn->exec("SELECT * FROM users");
        }
    });
}

for(auto& t : threads) {
    t.join();
}
```

**Важно**: Пул соединений потокобезопасен и может использоваться из нескольких потоков одновременно. Соединения автоматически возвращаются в пул при уничтожении `shared_ptr`.

### Работа с результатами запросов

Класс `query_result` предоставляет удобный интерфейс для работы с результатами SQL-запросов:

```cpp
query_result result = connection->exec("SELECT id, name, email FROM users");

// Проверка на пустоту
if(result.empty()) {
    std::cout << "Результат пуст" << std::endl;
}

// Получение количества строк
size_t count = result.size();
std::cout << "Найдено записей: " << count << std::endl;

// Итерация по результатам
for(const auto& row : result) {
    for(const auto& [column, value] : row) {
        std::cout << column << ": " << value << std::endl;
    }
}

// Доступ по индексу
if(!result.empty()) {
    const auto& first_row = result.at(0);
    
    // Доступ к значениям по имени столбца
    std::string id = first_row.at("id");
    std::string name = first_row.at("name");
    std::string email = first_row.at("email");
    
    // Проверка на NULL (значение будет равно "NULL")
    if(email == NULL_VALUE) {
        std::cout << "Email не указан" << std::endl;
    }
}

// Получение всех данных
std::vector<query_result::row> all_data = result.data();

// Модификация данных (для продвинутых случаев)
auto& mutable_data = result.mutable_data();
// ... модификация ...
```

**Примечание**: Значения NULL в результатах представлены строкой `"NULL"` (определена константа `NULL_VALUE`).

### Логирование

Библиотека поддерживает кастомные логгеры для отслеживания SQL-запросов и ошибок:

```cpp
class MyLogger : public database_adapter::ILogger {
public:
    void log_error(const std::string& message) override {
        std::cerr << "[ERROR] " << message << std::endl;
    }
    
    void log_sql(const std::string& message) override {
        std::cout << "[SQL] " << message << std::endl;
    }
};

// Установка логгера (зависит от конкретной реализации адаптера)
// Например, для SQLite:
// database_adapter::sqlite::connection::set_logger(std::make_shared<MyLogger>());
```

## Обработка ошибок

Библиотека использует исключения для обработки ошибок. Все исключения наследуются от `std::exception`:

### open_database_exception

Выбрасывается при ошибке подключения к базе данных:

```cpp
try {
    auto connection = std::make_shared<MyConnection>(settings);
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

- **Методы транзакций** - все методы работы с транзакциями (`commit()`, `rollback()`, `add_save_point()`, `rollback_to_save_point()`) защищены мьютексами
- **Пул соединений** - полностью потокобезопасен и может использоваться из нескольких потоков одновременно
- **Проверка транзакций** - метод `is_transaction()` потокобезопасен

**Важно**: Хотя операции с транзакциями потокобезопасны, рекомендуется использовать одно соединение на поток для лучшей производительности.

## Уровни изоляции транзакций

Библиотека поддерживает стандартные уровни изоляции транзакций:

```cpp
enum class transaction_isolation_level {
    DEFAULT = -1,           // Уровень по умолчанию (зависит от БД)
    READ_UNCOMMITTED = 0,  // Чтение незакоммиченных данных
    READ_COMMITTED = 1,     // Чтение только закоммиченных данных
    REPEATABLE_READ = 2,    // Повторяемое чтение
    SERIALIZABLE = 3        // Сериализуемый (самый строгий)
};

// Использование
connection->open_transaction(transaction_isolation_level::SERIALIZABLE);
```

**Примечание**: Конкретная реализация уровней изоляции зависит от используемой базы данных и её драйвера.

## Защита от SQL-инъекций

Библиотека обеспечивает базовую защиту от SQL-инъекций:

- **Валидация имен savepoint** - имена могут содержать только буквы, цифры и подчеркивания
- **Валидация SQL-запросов** - проверка на пустоту и наличие только пробелов
- **Подготовленные запросы** - рекомендуется использовать `prepare()` и `exec_prepared()` для параметризованных запросов

**Важно**: Для максимальной безопасности всегда используйте подготовленные запросы вместо конкатенации строк:

```cpp
// ❌ НЕБЕЗОПАСНО
std::string query = "SELECT * FROM users WHERE id = " + user_id;
connection->exec(query);

// ✅ БЕЗОПАСНО
connection->prepare("SELECT * FROM users WHERE id = $1", "get_user");
query_result result = connection->exec_prepared({user_id}, "get_user");
```

## Примеры использования

Полные примеры использования библиотеки можно найти в директории `example/` проекта EntityCraft. Там представлены примеры работы с различными адаптерами (SQLite, PostgreSQL) и демонстрация основных возможностей библиотеки.

## Требования

- C++14 или выше
- CMake 3.10 или выше
- Конкретная реализация адаптера (например, SqliteAdapter или PostgreAdapter)

## Лицензия

См. файл LICENSE в корне проекта.
