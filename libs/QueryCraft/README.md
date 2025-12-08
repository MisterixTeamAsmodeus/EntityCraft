# QueryCraft

QueryCraft — это современная C++ библиотека для построения SQL-запросов с использованием fluent builder API и DSL (Domain Specific Language). Библиотека предоставляет типобезопасный способ создания SQL-запросов без необходимости писать SQL-строки вручную.

## Основные возможности

- ✅ **Fluent Builder API** — интуитивный интерфейс для построения запросов
- ✅ **DSL для выражений** — удобные хелперы для создания условий и выражений
- ✅ **Поддержка множественных диалектов** — PostgreSQL и SQLite из коробки
- ✅ **Prepared Statements** — автоматическая генерация параметризованных запросов
- ✅ **Типобезопасность** — компиляция времени проверка корректности запросов
- ✅ **SELECT, INSERT, UPDATE, DELETE** — полная поддержка основных операций
- ✅ **JOIN** — поддержка INNER, LEFT, RIGHT, FULL JOIN
- ✅ **Агрегирующие функции** — COUNT, SUM, AVG, MIN, MAX и пользовательские функции
- ✅ **CTE (Common Table Expressions)** — поддержка WITH-запросов
- ✅ **Подзапросы** — возможность использования подзапросов в различных контекстах
- ✅ **GROUP BY и HAVING** — группировка данных и фильтрация групп
- ✅ **ORDER BY, LIMIT, OFFSET** — сортировка и пагинация
- ✅ **RETURNING** — поддержка RETURNING для INSERT, UPDATE, DELETE

## Быстрый старт

### Простой SELECT запрос

```cpp
#include <QueryCraft/builder/select_builder.h>
#include <QueryCraft/builder/dsl.h>
#include <QueryCraft/dialect/postgres_dialect.h>

using namespace query_craft::dsl;

select_builder builder;
builder.from("users")
    .columns({ col("id"), col("name"), col("email") });

auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
auto query = builder.compile(postgres_dialect);

// query.sql содержит: SELECT "id", "name", "email" FROM "users"
// query.parameters содержит параметры для prepared statement
```

### SELECT с условиями

```cpp
select_builder builder;
builder.from("users")
    .columns({ col("id"), col("name") })
    .where((col("age") > param(18)) && (col("active") == param(true)));

auto query = builder.compile(postgres_dialect);
```

### INSERT запрос

```cpp
#include <QueryCraft/builder/insert_builder.h>

insert_builder builder;
builder.into("users")
    .columns({ "name", "email", "age" })
    .values({ param(std::string("Иван Иванов")), 
              param(std::string("ivan@example.com")), 
              param(30) })
    .returning({ "id", "name", "created_at" });

auto query = builder.compile(postgres_dialect);
```

### UPDATE запрос

```cpp
#include <QueryCraft/builder/update_builder.h>

update_builder builder;
builder.table("users")
    .set("email", param(std::string("newemail@example.com")))
    .set("age", param(31))
    .where(col("id") == param(1))
    .returning({ "id", "name", "email" });

auto query = builder.compile(postgres_dialect);
```

### DELETE запрос

```cpp
#include <QueryCraft/builder/delete_builder.h>

delete_builder builder;
builder.from("users")
    .where(col("id") == param(1))
    .returning({ "id", "name", "email" });

auto query = builder.compile(postgres_dialect);
```

## DSL (Domain Specific Language)

QueryCraft предоставляет набор хелперов для создания выражений:

### Создание колонок и идентификаторов

```cpp
using namespace query_craft::dsl;

// Простая колонка
col("name")

// Колонка с алиасом
col("name", "users")
```

### Литералы и параметры

```cpp
// Строковый литерал (вставляется напрямую в SQL)
value("some string")

// Параметр для prepared statement
param(42)
param(std::string("text"))
param(true)
```

### Операторы

```cpp
// Сравнение
col("age") > param(18)
col("name") == param(std::string("Иван"))
col("price") <= param(100.0)

// Логические операторы
(col("age") > param(18)) && (col("active") == param(true))
(col("status") == param("active")) || (col("status") == param("pending"))

// Группировка условий
and_({
    col("age") > param(18),
    col("status") == param(std::string("active")),
    col("deleted") == param(false)
})

or_({
    col("status") == param(std::string("active")),
    col("status") == param(std::string("pending"))
})

// Оператор IN
in_(col("id"), subquery(select_builder().from("users").columns({col("id")})))
```

### Агрегирующие функции

```cpp
count({ col("id") })
sum({ col("amount") })
avg({ col("price") })
min({ col("price") })
max({ col("price") })

// Пользовательская агрегирующая функция
custom_aggregate("json_array_agg_logical", { col("value") })
```

### Функции

```cpp
// Произвольная функция
func("UPPER", { col("name") })
func("CONCAT", { col("first_name"), value(" "), col("last_name") })
```

### Подзапросы

```cpp
// Подзапрос в WHERE
select_builder subquery_builder;
subquery_builder.from("orders")
    .columns({ col("user_id") })
    .where(col("amount") > param(1000));

select_builder builder;
builder.from("users")
    .columns({ col("id"), col("name") })
    .where(col("id") == subquery(subquery_builder.to_ast()));

// Подзапрос в SELECT
select_builder subquery_builder;
subquery_builder.from("orders")
    .columns({ count({ col("id") }) })
    .where(col("user_id") == col("id", "users"));

select_builder builder;
builder.from("users")
    .columns({ col("id"), col("name"), subquery(subquery_builder.to_ast()) });
```

## SELECT запросы

### Базовые возможности

```cpp
select_builder builder;
builder.from("users")
    .columns({ col("id"), col("name"), col("email") })
    .where(col("active") == param(true))
    .order_by({ { col("name"), true } })  // true = ASC, false = DESC
    .limit(10)
    .offset(20);
```

### JOIN

```cpp
// INNER JOIN
builder.from("users")
    .columns({ col("id", "users"), col("name", "users"), col("order_id", "orders") })
    .inner_join("orders", col("id", "users") == col("user_id", "orders"));

// LEFT JOIN
builder.left_join("profiles", col("id", "users") == col("user_id", "profiles"), "public");

// RIGHT JOIN
builder.right_join("orders", col("id", "users") == col("user_id", "orders"));

// FULL JOIN
builder.full_join("orders", col("id", "users") == col("user_id", "orders"));

// Несколько JOIN
builder.from("users")
    .inner_join("orders", col("id", "users") == col("user_id", "orders"))
    .left_join("products", col("product_id", "orders") == col("id", "products"));
```

### GROUP BY и HAVING

```cpp
builder.from("orders")
    .columns({ col("user_id"), sum({ col("amount") }), count({ col("id") }) })
    .group_by({ col("user_id") })
    .having(sum({ col("amount") }) > param(1000));
```

### CTE (Common Table Expressions)

```cpp
// Простой CTE
select_builder cte_query;
cte_query.from("users")
    .columns({ col("id") })
    .where(col("active") == param(true));

select_builder builder;
builder.with("active_users", cte_query.to_ast())
    .from("active_users")
    .columns({ col("id") });

// Несколько CTE
select_builder active_users_cte;
active_users_cte.from("users")
    .columns({ col("id"), col("name") })
    .where(col("active") == param(true));

select_builder recent_orders_cte;
recent_orders_cte.from("orders")
    .columns({ col("user_id"), col("amount") })
    .where(col("created_at") > param(std::string("2024-01-01")));

select_builder builder;
builder.with("active_users", active_users_cte.to_ast())
    .with("recent_orders", recent_orders_cte.to_ast())
    .from("active_users")
    .columns({ col("name", "active_users"), col("amount", "recent_orders") })
    .inner_join("recent_orders", col("id", "active_users") == col("user_id", "recent_orders"));
```

## INSERT запросы

### Простой INSERT

```cpp
insert_builder builder;
builder.into("users")
    .columns({ "name", "email", "age" })
    .values({ param(std::string("Иван Иванов")), 
              param(std::string("ivan@example.com")), 
              param(30) });
```

### INSERT с несколькими строками

```cpp
insert_builder builder;
builder.into("users")
    .columns({ "name", "age" })
    .values({ param(std::string("Алексей")), param(25) })
    .values({ param(std::string("Мария")), param(28) })
    .values({ param(std::string("Петр")), param(32) });
```

### INSERT с RETURNING

```cpp
builder.into("users")
    .columns({ "name", "email" })
    .values({ param(std::string("Новый пользователь")), 
              param(std::string("new@example.com")) })
    .returning({ "id", "name", "created_at" });
```

## UPDATE запросы

### Простой UPDATE

```cpp
update_builder builder;
builder.table("users")
    .set("email", param(std::string("newemail@example.com")))
    .set("age", param(31))
    .where(col("id") == param(1));
```

### UPDATE с RETURNING

```cpp
builder.table("users")
    .set("active", param(false))
    .where(col("last_login") < param(std::string("2024-01-01")))
    .returning({ "id", "name", "email" });
```

### UPDATE с сложными условиями

```cpp
builder.table("users")
    .set("status", param(std::string("inactive")))
    .where((col("age") < param(18)) || (col("age") > param(65)));
```

## DELETE запросы

### Простой DELETE

```cpp
delete_builder builder;
builder.from("users")
    .where(col("id") == param(1));
```

### DELETE с RETURNING

```cpp
builder.from("users")
    .where(col("active") == param(false))
    .returning({ "id", "name", "email" });
```

### DELETE с сложными условиями

```cpp
builder.from("users")
    .where((col("created_at") < param(std::string("2020-01-01"))) && 
           (col("last_login") == param(std::string("NULL"))));
```

## Диалекты SQL

QueryCraft поддерживает несколько диалектов SQL. Каждый диалект генерирует SQL в соответствии со спецификой конкретной СУБД.

### PostgreSQL

```cpp
#include <QueryCraft/dialect/postgres_dialect.h>

auto postgres_dialect = std::make_shared<query_craft::postgres_dialect>();
auto query = builder.compile(postgres_dialect);

// Использует плейсхолдеры: $1, $2, $3, ...
// Поддерживает RETURNING
// Поддерживает схемы
```

### SQLite

```cpp
#include <QueryCraft/dialect/sqlite_dialect.h>

auto sqlite_dialect = std::make_shared<query_craft::sqlite_dialect>();
auto query = builder.compile(sqlite_dialect);

// Использует плейсхолдеры: ?
// Поддерживает RETURNING (с версии 3.35.0)
```

### Различия между диалектами

| Особенность | PostgreSQL | SQLite |
|------------|------------|--------|
| Плейсхолдеры | `$1, $2, $3, ...` | `?` |
| RETURNING | Да | Да (с версии 3.35.0) |
| Схемы | Полная поддержка | Ограниченная |
| Кавычки идентификаторов | Двойные кавычки (`"`) | Двойные кавычки (`"`) |

## Скомпилированные запросы

После компиляции билдера вы получаете объект `compiled_query`, который содержит:

- `sql` — итоговая SQL-строка
- `parameters` — вектор параметров для prepared statement

```cpp
auto query = builder.compile(postgres_dialect);

std::cout << "SQL: " << query.sql << std::endl;
for (const auto& param : query.parameters) {
    std::cout << "Parameter: " << param << std::endl;
}
```

## Зависимости

- **TypeConverterApi** — для преобразования типов в строки

## Сборка

QueryCraft использует CMake для сборки:

```cmake
target_link_libraries(your_target QueryCraft)
```

## Примеры использования

Полные примеры использования находятся в директории `example/QueryCraft/`:

- `example-1.cpp` — Базовые SELECT запросы
- `example-2.cpp` — INSERT, UPDATE, DELETE запросы
- `example-3.cpp` — JOIN запросы
- `example-4.cpp` — Агрегирующие функции и GROUP BY
- `example-5.cpp` — CTE и подзапросы
- `example-6.cpp` — Различия между диалектами

## Архитектура

Библиотека построена на основе следующих компонентов:

- **AST (Abstract Syntax Tree)** — представление SQL-запросов в виде дерева объектов
- **Builders** — fluent API для построения запросов
- **DSL** — хелперы для создания выражений
- **Compilers** — генерация SQL из AST
- **Dialects** — адаптация SQL под конкретные СУБД

## Лицензия

См. файл LICENSE в корне проекта.

## Поддержка

При возникновении вопросов или проблем создайте issue в репозитории проекта.

