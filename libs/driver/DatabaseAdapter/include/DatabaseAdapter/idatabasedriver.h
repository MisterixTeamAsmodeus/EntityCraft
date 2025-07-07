#pragma once

#include "model/queryresult.h"

#include <memory>
#include <string>
#include <vector>

namespace database_adapter {
class IConnection;

namespace models {
struct database_settings;
} // namespace models
class ITransaction;
} // namespace database_adapter

namespace database_adapter {
#define NULL_VALUE "NULL";

/**
 * @brief Интерфейс для работы с базами данных.
 * Класс IDataBaseDriver предоставляет основные операции с базами данных,
 * которые должны быть реализованы в конкретных драйверах базы данных.
 */
class IDataBaseDriver
{
public:
    /**
     * @brief Конструктор IDataBaseDriver с указанными настройками подключения.
     * @param settings Настройки подключения к базе данных.
     */
    explicit IDataBaseDriver(models::database_settings settings);

    /**
     * @brief Конструктор IDataBaseDriver с готовым соединением к базе данных.
     * @param connection Подключение к базе данных.
     */
    explicit IDataBaseDriver(std::shared_ptr<IConnection> connection);

    /**
     * @brief Деструктор по умолчанию.
     */
    virtual ~IDataBaseDriver() = default;

    /**
     * @brief Открывает новую транзакцию.
     * Эта функция должна открыть новую транзакцию и вернуть указатель на нее.
     * @param type Тип транзакции (Зависит от реализации базы данных).
     * @return Указатель на открытую транзакцию. Если транзакция не была открыта возвращается nullptr.
     */
    virtual std::shared_ptr<ITransaction> open_transaction(int type) const = 0;

    /**
     * Возвращает флаг который сообщает о том что данная база данных поддреживает returning после insert
     */
    virtual bool has_returning_statment();

    /**
     * Функция которая модифицирует запрос на всавку чтобы она возвращала требуемые поля после вставки
     * @param sql Запрос который необходимо модифицировать
     * @param returning_columns Список колонок которые необходимо вернуть
     * @note Для баз данных в которых нет поддержки returning
     */
    virtual void append_returning(std::string& sql, const std::vector<std::string>& returning_columns);

    /**
     * @brief Проверяет, открыто ли соединение с базой данных.
     * @return true, если соединение открыто, иначе false.
     */
    bool is_open() const;

    /**
     * @brief Выполняет SQL-запрос к базе данных.
     * Эта функция должна выполнить указанный SQL-запрос к базе данных
     * и вернуть результат в виде объекта QueryResult.
     * @param query SQL-запрос.
     * @return Результат выполнения SQL-запроса.
     * @throws Выбрасывает исключение sql_exception в случае ошибки выполнения запроса
     */
    models::query_result exec(const std::string& query);

    /**
     * @brief Открывает новую транзакцию с типом -1.
     * Эта функция должна открыть новую транзакцию и вернуть указатель на нее.
     * @return Указатель на открытую транзакцию. Если транзакция не была открыта возвращается nullptr.
     */
    std::shared_ptr<ITransaction> open_base_transaction() const;

protected:
    std::shared_ptr<IConnection> _connection;
};

} // namespace database_adapter
