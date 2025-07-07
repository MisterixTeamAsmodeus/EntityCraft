#pragma once

#include "model/queryresult.h"

#include <memory>
#include <string>

namespace database_adapter {
class IConnection;
} // namespace database_adapter

namespace database_adapter {
/**
 * @brief Интерфейс для работы с транзакциями в базе данных.
 * Класс ITransaction предоставляет основные операции с транзакциями,
 * которые должны быть реализованы в конкретных драйверах базы данных.
 * @note Класс должен быть специализирован для конкретной реализации базы данных и генерировать sql команды для выполнения внесенных изменений.
 */
class ITransaction
{
public:
    explicit ITransaction(std::shared_ptr<IConnection> connection);

    /**
     * @brief Деструктор по умолчанию.
     */
    virtual ~ITransaction() = default;

    /**
     * Функция для создания транзакции с заданным уровнем изоляции
     * @param type Уровень изоляции. Зависит от конкретной реализации
     */
    virtual void open_transaction(int type) = 0;

    /**
     * @brief Фиксирует изменения в базе данных.
     * Эта функция должна фиксировать все изменения, внесенные в базу данных
     * с момента начала текущей транзакции.
     */
    virtual void commit() = 0;

    /**
     * @brief Добавляет точку сохранения в текущую транзакцию.
     * Эта функция должна создать точку сохранения, которую можно использовать
     * для отката изменений до этой точки в текущей транзакции.
     * @param save_point Имя точки сохранения.
     */
    virtual void add_save_point(const std::string& save_point) = 0;

    /**
     * @brief Откатывает изменения в базе данных до указанной точки сохранения.
     * Эта функция должна откатить все изменения, внесенные в базу данных
     * с момента создания указанной точки сохранения.
     * @param save_point Точка сохранения, до которой необходимо откатить изменения.
     * @note Если строка пустая должен произойти откат всех изменений
     */
    virtual void rollback_to_save_point(const std::string& save_point) = 0;

    /**
     * Функция для создания транзакции с базовым уровнем изоляции
     */
    void open_base_transaction();

    /**
     * Отменить изменения в базе данных
     */
    void rollback();

    /**
     * @brief Выполняет SQL-запрос в рамках текущей транзакции.
     *
     * Эта функция выполняет предоставленный SQL-запрос как часть текущей транзакции базы данных.
     * Она позволяет выполнять операции с базой данных, такие как SELECT, INSERT, UPDATE или DELETE,
     * в рамках транзакции.
     *
     * @param query Строка, содержащая SQL-запрос для выполнения.
     * @return Результат выполнения SQL-запроса.
     * @throws Выбрасывает исключение sql_exception в случае ошибки выполнения запроса
     */
    models::query_result exec(const std::string& query);

protected:
    std::shared_ptr<IConnection> _connection;
};
} // namespace database_adapter
