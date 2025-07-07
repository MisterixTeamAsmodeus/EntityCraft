# ReflectionApi
## Описание
ReflectionApi - это современная C++ библиотека (требуется C++14 или новее), предоставляющая механизмы компилируемой рефлексии для работы с классами и их членами. Библиотека позволяет инспектировать и модифицировать поля классов, работать с вложенными структурами и преобразовывать данные между различными представлениями.

## Основные возможности
Рефлексия полей классов:

1. Доступ к полям через указатели на члены
   - Поддержка get/set методов
   - Работа с различными типами свойств (константные, изменяемые)

2. Иерархические структуры:
   - Поддержка вложенных объектов через reference_property
   - Рекурсивный обход сложных структур данных

3. Гибкие интерфейсы:
   - Посетители (visitors) для разных типов свойств
   - Шаблонные алгоритмы для работы с кортежами свойств
   - Интеграция с TypeConverterApi для преобразования типов

4. Безопасность типов:

   - Проверки типов на этапе компиляции
   - Специализации для различных вариантов доступа к данным

## Преимущества
- Компилируемая рефлексия - вся информация о структурах доступна на этапе компиляции
- Гибкость - поддержка различных способов доступа к данным (поля, методы)
- Расширяемость - возможность добавления новых типов свойств и посетителей
- Производительность - минимум накладных расходов благодаря шаблонной реализации

## Примеры использования
```c++
#include "reflectionapi.h"

// Простой класс с полями
struct Person {
    int age = 0;
    std::string name;
};

// Класс с методами доступа
class Account {
public:
    void setBalance(double b) { balance = b; }
    double getBalance() const { return balance; }
private:
    double balance = 0.0;
};

// Вложенная структура
struct Transaction {
    double amount;
    std::string description;
};

struct BankOperation {
    Account account;
    Transaction transaction;
};

// Создание метаинформации
auto personEntity = make_entity<Person>(
    make_property("age", &Person::age),
    make_property("name", &Person::name)
);

auto accountEntity = make_entity<Account>(
    make_property("balance", &Account::setBalance, &Account::getBalance)
);

auto transactionEntity = make_entity<Transaction>(
    make_property("amount", &Transaction::amount),
    make_property("description", &Transaction::description)
);

auto operationEntity = make_entity<BankOperation>(
    make_reference_property("account", &BankOperation::account, accountEntity),
    make_reference_property("transaction", &BankOperation::transaction, transactionEntity)
);

// Использование
Person p;
personEntity.visit_property("age", [&p](auto& prop) {
    prop.set_value(p, 25); // Установка значения
});

BankOperation op;
operationEntity.visit_property("transaction", [&op](auto& prop) {
    prop.reference_entity().visit_property("amount", [&op](auto& transProp) {
        transProp.set_value(op.transaction, 100.50); // Установка вложенного свойства
    });
});
```

## Требования
Компилятор с поддержкой C++14 или новее

Зависимость от TypeConverterApi для работы с преобразованием типов

## Установка
1. Скопируйте заголовочные файлы в ваш проект

2. Подключите основной заголовочный файл reflectionapi.h

3. Убедитесь, что TypeConverterApi доступен в путях поиска заголовков

## Лицензия
Библиотека распространяется под лицензией MIT. Подробности см. в файле LICENSE.

## Документация
Полная документация доступна в заголовочных файлах и в примерах использования. Основные сущности:
- **entity** - описание структуры класса
- **property** - описание поля или свойства класса
- **reference_property** - описание вложенной структуры
- **visitor** - обработка разных типов свойств