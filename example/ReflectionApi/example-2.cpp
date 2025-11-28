/**
 * ПРИМЕР 2: Работа с геттерами и сеттерами
 * 
 * Этот пример демонстрирует использование property с методами доступа:
 * 
 * 1. PROPERTY ЧЕРЕЗ ГЕТТЕРЫ И СЕТТЕРЫ
 *    - Работа с классами, где поля приватные
 *    - Использование методов доступа вместо прямых указателей на поля
 *    - Различные варианты сигнатур геттеров и сеттеров
 * 
 * 2. РАЗЛИЧИЯ В СИГНАТУРАХ
 *    - Сеттеры с параметром const PropertyType& (для сложных типов)
 *    - Сеттеры с параметром PropertyType (для примитивных типов)
 *    - Геттеры const и не-const
 * 
 * 3. ПРЕИМУЩЕСТВА
 *    - Инкапсуляция: работа с приватными полями через публичные методы
 *    - Валидация: возможность добавить проверки в сеттеры
 *    - Гибкость: можно использовать вычисляемые свойства
 */

#include <ReflectionApi/reflectionapi.hpp>
#include <iostream>
#include <string>

// ============================================================
// КЛАСС С ПРИВАТНЫМИ ПОЛЯМИ И ПУБЛИЧНЫМИ МЕТОДАМИ ДОСТУПА
// ============================================================
// 
// Этот класс демонстрирует классический подход к инкапсуляции:
// - Поля приватные (недоступны напрямую)
// - Доступ через публичные геттеры и сеттеры
// - Возможность добавить валидацию или дополнительную логику

class BankAccount
{
public:
    // Геттеры и сеттеры для баланса
    // setBalance принимает значение по значению (для double это нормально)
    void setBalance(double balance) { 
        _balance = balance; 
    }
    
    // getBalance возвращает значение по значению (const метод)
    double getBalance() const { 
        return _balance; 
    }

    // Геттеры и сеттеры для номера счета
    // setAccountNumber принимает const std::string& (для строк это эффективнее)
    void setAccountNumber(const std::string& number) { 
        _accountNumber = number; 
    }
    
    // getAccountNumber возвращает const std::string& (избегаем копирования)
    std::string getAccountNumber() const {
        return _accountNumber; 
    }

    // Геттеры и сеттеры для активного статуса
    void setActive(bool active) { 
        _isActive = active; 
    }
    
    // isActive - альтернативное имя для геттера (не getActive)
    bool isActive() const { 
        return _isActive; 
    }

private:
    double _balance = 0.0;           // Приватное поле - баланс
    std::string _accountNumber;       // Приватное поле - номер счета
    bool _isActive = false;           // Приватное поле - активен ли счет
};

// ============================================================
// КЛАСС С ПРИМИТИВНЫМИ ТИПАМИ
// ============================================================
//
// Для примитивных типов (int, double, bool) сеттеры часто
// принимают параметры по значению, а не по const&
// ReflectionApi поддерживает оба варианта

class Counter
{
public:
    // setValue принимает int по значению (не const int&)
    // Это нормально для примитивных типов
    void setValue(int value) { 
        _value = value; 
    }
    
    // getValue возвращает int по значению
    int getValue() const { 
        return _value; 
    }

private:
    int _value = 0;
};

int main()
{
    std::cout << "========================================\n";
    std::cout << "ПРИМЕР 2: Работа с геттерами и сеттерами\n";
    std::cout << "========================================\n\n";

    // ============================================================
    // ШАГ 1: СОЗДАНИЕ PROPERTY ЧЕРЕЗ ГЕТТЕРЫ И СЕТТЕРЫ
    // ============================================================
    //
    // reflection_api::make_property может принимать не только указатели на поля,
    // но и указатели на методы доступа (геттеры и сеттеры)
    //
    // Сигнатура: reflection_api::make_property(name, setter, getter)
    // - name - имя свойства
    // - setter - указатель на метод-сеттер
    // - getter - указатель на метод-геттер
    
    std::cout << "ШАГ 1: Создание property через геттеры и сеттеры\n";
    std::cout << "------------------------------------------------\n";
    
    // Создаем property для баланса через методы доступа
    // Библиотека автоматически определяет типы из сигнатур методов
    auto balanceProperty = reflection_api::make_property(
        "balance",                          // Имя свойства
        &BankAccount::setBalance,           // Указатель на сеттер
        &BankAccount::getBalance            // Указатель на геттер
    );

    // Создаем property для номера счета
    auto accountNumberProperty = reflection_api::make_property(
        "accountNumber",
        &BankAccount::setAccountNumber,
        &BankAccount::getAccountNumber
    );

    // Создаем property для активного статуса
    // Обратите внимание: геттер называется isActive, а не getActive
    auto activeProperty = reflection_api::make_property(
        "isActive",
        &BankAccount::setActive,
        &BankAccount::isActive
    );

    std::cout << "Созданы property через геттеры и сеттеры:\n";
    std::cout << "  - balanceProperty: '" << balanceProperty.name() << "'\n";
    std::cout << "  - accountNumberProperty: '" << accountNumberProperty.name() << "'\n";
    std::cout << "  - activeProperty: '" << activeProperty.name() << "'\n\n";

    // ============================================================
    // ШАГ 2: РАБОТА С ОБЪЕКТОМ ЧЕРЕЗ PROPERTY
    // ============================================================
    //
    // Работа с property через геттеры/сеттеры ничем не отличается
    // от работы с property через указатели на поля
    // Интерфейс остается тем же: set_value() и value()
    
    std::cout << "ШАГ 2: Работа с объектом через property\n";
    std::cout << "----------------------------------------\n";
    
    // Создаем объект BankAccount
    BankAccount account;

    // Устанавливаем значения через property
    // Внутри property вызывает соответствующие сеттеры
    balanceProperty.set_value(account, 1000.50);
    accountNumberProperty.set_value(account, std::string("1234567890"));
    activeProperty.set_value(account, true);

    std::cout << "Установлены значения через property:\n";
    
    // Получаем значения через property
    // Внутри property вызывает соответствующие геттеры
    std::cout << "  account.balance = " << balanceProperty.value(account) << "\n";
    std::cout << "  account.accountNumber = '" << accountNumberProperty.value(account) << "'\n";
    std::cout << "  account.isActive = " << (activeProperty.value(account) ? "true" : "false") << "\n\n";

    // ============================================================
    // ШАГ 3: СОЗДАНИЕ ENTITY С PROPERTY ЧЕРЕЗ ГЕТТЕРЫ/СЕТТЕРЫ
    // ============================================================
    //
    // entity может содержать property, созданные любым способом:
    // - через указатели на поля
    // - через геттеры и сеттеры
    // - их комбинацию
    
    std::cout << "ШАГ 3: Создание entity с property через геттеры/сеттеры\n";
    std::cout << "-------------------------------------------------------\n";
    
    // Создаем entity для BankAccount
    // Все property создаются через геттеры и сеттеры
    auto accountEntity = make_entity<BankAccount>(
        reflection_api::make_property("balance", &BankAccount::setBalance, &BankAccount::getBalance),
        reflection_api::make_property("accountNumber", &BankAccount::setAccountNumber, &BankAccount::getAccountNumber),
        reflection_api::make_property("isActive", &BankAccount::setActive, &BankAccount::isActive)
    );

    std::cout << "Создана entity для BankAccount:\n";
    std::cout << "  Количество свойств: " << accountEntity.property_count() << "\n\n";

    // ============================================================
    // ШАГ 4: РАБОТА С ОБЪЕКТОМ ЧЕРЕЗ ENTITY
    // ============================================================
    //
    // Работа через entity с property через геттеры/сеттеры
    // полностью аналогична работе с property через указатели на поля
    
    std::cout << "ШАГ 4: Работа с объектом через entity\n";
    std::cout << "--------------------------------------\n";
    
    // Создаем новый объект BankAccount
    BankAccount account2;

    // Устанавливаем значения через entity по имени свойства
    accountEntity.set_property_value(account2, 5000.75, "balance");
    accountEntity.set_property_value(account2, std::string("9876543210"), "accountNumber");
    accountEntity.set_property_value(account2, false, "isActive");

    std::cout << "Установлены значения через entity:\n";
    
    // Получаем значения через entity
    double balance = 0.0;
    std::string accountNumber;
    bool isActive = false;

    accountEntity.get_property_value(account2, balance, "balance");
    accountEntity.get_property_value(account2, accountNumber, "accountNumber");
    accountEntity.get_property_value(account2, isActive, "isActive");

    std::cout << "  account2.balance = " << balance << "\n";
    std::cout << "  account2.accountNumber = '" << accountNumber << "'\n";
    std::cout << "  account2.isActive = " << (isActive ? "true" : "false") << "\n\n";

    // ============================================================
    // ШАГ 5: РАБОТА С ПРИМИТИВНЫМИ ТИПАМИ
    // ============================================================
    //
    // Для примитивных типов сеттеры часто принимают параметры
    // по значению, а не по const&
    // ReflectionApi автоматически определяет правильный тип сеттера
    
    std::cout << "ШАГ 5: Работа с примитивными типами\n";
    std::cout << "------------------------------------\n";
    
    // Создаем property для Counter
    // setValue принимает int по значению, но библиотека это поддерживает
    auto counterProperty = reflection_api::make_property(
        "value",
        &Counter::setValue,      // Сеттер принимает int по значению
        &Counter::getValue       // Геттер возвращает int по значению
    );

    Counter counter;
    counterProperty.set_value(counter, 42);

    std::cout << "Работа с примитивными типами:\n";
    std::cout << "  counter.value = " << counterProperty.value(counter) << "\n\n";

    // ============================================================
    // ШАГ 6: СОЗДАНИЕ ENTITY ДЛЯ ПРИМИТИВНЫХ ТИПОВ
    // ============================================================
    //
    // entity работает одинаково для всех типов property
    
    std::cout << "ШАГ 6: Создание entity для примитивных типов\n";
    std::cout << "-------------------------------------------\n";
    
    // Создаем entity для Counter
    auto counterEntity = make_entity<Counter>(
        reflection_api::make_property("value", &Counter::setValue, &Counter::getValue)
    );

    Counter counter2;
    counterEntity.set_property_value(counter2, 100, "value");

    int value = 0;
    counterEntity.get_property_value(counter2, value, "value");
    
    std::cout << "Работа через entity с примитивными типами:\n";
    std::cout << "  counter2.value = " << value << "\n\n";

    // ============================================================
    // ЗАКЛЮЧЕНИЕ
    // ============================================================
    //
    // В этом примере мы изучили:
    // 1. Как создавать property через геттеры и сеттеры
    // 2. Как работать с классами, имеющими приватные поля
    // 3. Различия в сигнатурах сеттеров для разных типов
    // 4. Как использовать entity с property через методы доступа
    //
    // Использование геттеров и сеттеров позволяет:
    // - Сохранять инкапсуляцию (поля остаются приватными)
    // - Добавлять валидацию в сеттеры
    // - Использовать вычисляемые свойства
    // - Работать с классами, где нет публичных полей

    std::cout << "========================================\n";
    std::cout << "Пример завершен успешно!\n";
    std::cout << "========================================\n";

    return 0;
}
