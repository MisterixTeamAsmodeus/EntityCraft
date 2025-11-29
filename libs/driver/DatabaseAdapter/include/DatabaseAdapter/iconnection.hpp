#pragma once

#include "model/queryresult.hpp"
#include "transaction_isolation.hpp"

#include <mutex>
#include <string>
#include <vector>

namespace database_adapter {
struct database_connection_settings;
} // namespace database_adapter

namespace database_adapter {

/// @brief Класс который инкапсулирует всю логику взаимодействия с базой данных. От выполнения запросов и открытие транзакций до кеширования запросов
class IConnection
{
public:
    /**
     * @brief Конструктор, который принимает в себя информацию о подключении к базе данных
     * @param settings Информация о подключении к базе данных
     * @note Во время вызова конструктора должно происходить подключение к базе данных
     * @throws open_database_exception Выбрасывает исключение в случае ошибки подключения к базе или невалидных настроек
     */
    explicit IConnection(const database_connection_settings& settings);

    virtual ~IConnection() = default;

    /**
     * @brief Проверка валидно ли соединение или нет
     * @return Возвращает статус запроса, true если активно и запросы выполняются, иначе false
     * @note В базовой реализации проверяет корректность выполнения запроса "SELECT 1"
     */
    virtual bool is_valid();

    /**
     * @brief Проверка открыта ли транзакция в текущем соединении
     * @return Возвращает true, если в текущем соединении имеется активная транзакция, иначе false.
     * @note Потокобезопасный метод
     */
    bool is_transaction() const;

    /**
     * @brief Выполняет SQL-запрос к базе данных. Эта функция должна выполнить указанный SQL-запрос к базе данных и вернуть результат в виде объекта QueryResult.
     * @param query SQL-запрос.
     * @return Результат выполнения SQL-запроса.
     * @throws sql_exception Выбрасывает исключение в случае ошибки выполнения запроса или если запрос пустой
     */
    virtual query_result exec(const std::string& query) = 0;

    /**
     * @brief Выполняет подготовку запроса для возможности динамической подстановки параметров и кэширования запросов
     * @param query Запрос который необходимо подготовить
     * @param name Наименование подготовленного запроса
     * @throws sql_exception Выбрасывает исключение в случае ошибки выполнения запроса
     */
    virtual void prepare(const std::string& query, const std::string& name) = 0;

    /**
     * @brief Выполнить подготовленный запрос с подставленными значениями
     * @return Результат выполнения SQL-запроса.
     * @throws sql_exception Выбрасывает исключение в случае ошибки выполнения запроса
     */
    virtual query_result exec_prepared(const std::vector<std::string>& params, const std::string& name) = 0;

    /**
     * @brief Алиас для open_transaction(). Открывает новую транзакцию с уровнем изоляции по умолчанию.
     * @return Возвращает true, если транзакция была успешно открыта, иначе false
     */
    bool begin_transaction();

    /**
     * @brief Открывает новую транзакцию с заданным уровнем изоляции (типобезопасная версия).
     * @param level Уровень изоляции транзакции
     * @return Возвращает true, если транзакция была успешно открыта, иначе false
     */
    virtual bool open_transaction(transaction_isolation_level level) = 0;

    /**
     * @brief Фиксирует изменения в базе данных с момента начала текущей транзакции.
     * @throws sql_exception Выбрасывает исключение если нет активной транзакции
     * @note Потокобезопасный метод
     */
    void commit();

    /**
     * @brief Добавляет точку сохранения в текущую транзакцию.
     * @param save_point Имя точки сохранения. Должно содержать только буквы, цифры и подчеркивания.
     * @throws sql_exception Выбрасывает исключение если имя savepoint невалидно или нет активной транзакции
     * @note Потокобезопасный метод
     */
    void add_save_point(const std::string& save_point);

    /**
     * @brief Откатывает изменения в базе данных до указанной точки сохранения.
     * @param save_point Точка сохранения, до которой необходимо откатить изменения. Должно содержать только буквы, цифры и подчеркивания.
     * @note Если строка пустая произойдёт откат всех изменений
     * @throws sql_exception Выбрасывает исключение если имя savepoint невалидно (когда не пустое) или нет активной транзакции
     * @note Потокобезопасный метод
     */
    void rollback_to_save_point(const std::string& save_point);

    /**
     * @brief Откатить все в текущей транзакции.
     * @throws sql_exception Выбрасывает исключение если нет активной транзакции
     * @note Потокобезопасный метод
     */
    void rollback();

protected:
    /**
     * @brief Валидирует SQL-запрос перед выполнением
     * @param query SQL-запрос для валидации
     * @throws sql_exception Выбрасывает исключение если запрос пустой или содержит только пробелы
     * @note Этот метод должен вызываться в производных классах перед выполнением запроса
     */
    void validate_query(const std::string& query) const;

protected:
    /// @brief Мьютекс для защиты внутреннего состояния соединения от одновременного доступа из разных потоков
    mutable std::mutex _mutex;

    /// @brief Флаг обозначающий имеется ли открытая транзакция или нет.
    bool _has_transaction = false;
};

} // namespace database_adapter