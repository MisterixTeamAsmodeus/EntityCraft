#pragma once

#include "DatabaseAdapter/iconnection.hpp"

#include <chrono>
#include <memory>

namespace database_adapter {

/**
 * @brief Класс для хранения соединения и времени его последнего использования
 */
class ConnectionEntry
{
public:
    /**
     * @brief Конструктор, создающий запись о соединении
     * @param conn Умный указатель на соединение
     */
    explicit ConnectionEntry(std::shared_ptr<IConnection> conn);

    /**
     * @brief Получить соединение
     * @return Умный указатель на соединение
     */
    std::shared_ptr<IConnection> connection() const;

    /**
     * @brief Получить время последнего использования
     * @return Время последнего использования соединения
     */
    std::chrono::system_clock::time_point last_used() const;

    /**
     * @brief Обновить время последнего использования до текущего момента
     */
    void update_last_used();

    /**
     * @brief Установить время последнего использования
     * @param time_point Время использования
     */
    void set_last_used(const std::chrono::system_clock::time_point& time_point);

private:
    std::shared_ptr<IConnection> _connection;
    std::chrono::system_clock::time_point _last_used;
};

} // namespace database_adapter

