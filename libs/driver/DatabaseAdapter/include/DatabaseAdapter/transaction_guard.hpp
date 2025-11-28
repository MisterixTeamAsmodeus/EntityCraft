#pragma once

#include "iconnection.hpp"
#include "transaction_isolation.hpp"

#include <memory>

namespace database_adapter {

/**
 * @brief RAII-обертка для автоматического управления транзакциями
 * @note При уничтожении объекта автоматически выполняется rollback, если транзакция не была закоммичена
 * @example
 * {
 *     transaction_guard guard(connection);
 *     connection->exec("INSERT INTO ...");
 *     guard.commit(); // Явный commit
 * } // Если commit не был вызван, автоматически выполнится rollback
 */
class transaction_guard
{
public:
    /**
     * @brief Конструктор, который открывает транзакцию
     * @param connection Соединение с базой данных
     * @throws sql_exception Выбрасывает исключение если не удалось открыть транзакцию
     */
    explicit transaction_guard(std::shared_ptr<IConnection> connection);

    /**
     * @brief Конструктор, который открывает транзакцию с заданным уровнем изоляции
     * @param connection Соединение с базой данных
     * @param level Уровень изоляции транзакции
     * @throws sql_exception Выбрасывает исключение если не удалось открыть транзакцию
     */
    transaction_guard(std::shared_ptr<IConnection> connection, transaction_isolation_level level);

    /**
     * @brief Деструктор, который автоматически выполняет rollback, если транзакция не была закоммичена
     */
    ~transaction_guard();

    transaction_guard(const transaction_guard&) = delete;
    transaction_guard& operator=(const transaction_guard&) = delete;
    transaction_guard(transaction_guard&&) = delete;
    transaction_guard& operator=(transaction_guard&&) = delete;

    /**
     * @brief Выполняет commit транзакции
     * @throws sql_exception Выбрасывает исключение если commit не удался
     * @note После вызова commit() деструктор не будет выполнять rollback
     */
    void commit();

    /**
     * @brief Выполняет rollback транзакции
     * @throws sql_exception Выбрасывает исключение если rollback не удался
     * @note После вызова rollback() деструктор не будет выполнять rollback
     */
    void rollback();

    /**
     * @brief Проверяет, была ли транзакция закоммичена или откачена
     * @return true если транзакция завершена (commit или rollback), false иначе
     */
    bool is_finished() const;

    /**
     * @brief Получить соединение с базой данных
     * @return shared_ptr на соединение
     */
    std::shared_ptr<IConnection> get_connection() const;

private:
    std::shared_ptr<IConnection> _connection;
    bool _finished = false;
};

} // namespace database_adapter

