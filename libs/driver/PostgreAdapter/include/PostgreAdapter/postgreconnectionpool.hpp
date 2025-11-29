#pragma once

#include "postgreadapter_global.hpp"
#include "postgreconnection.hpp"

#include <DatabaseAdapter/connectionpool.hpp>

namespace database_adapter {
namespace postgre {

/**
 * @brief Пул соединений для PostgreSQL
 * @details Реализует интерфейс IConnectionPool для эффективного управления множественными подключениями к PostgreSQL.
 *          Использует паттерн Singleton для обеспечения единственного экземпляра пула в приложении.
 */
class POSTGRE_EXPORT connection_pool final : public IConnectionPool
{
public:
    /// @brief Настройки подключения к базе данных для пула соединений
    static database_connection_settings connection_settings;

    /// @brief Начальный размер пула соединений (количество соединений, создаваемых при инициализации)
    static size_t start_pool_size;

    /// @brief Максимальный размер пула соединений
    static size_t max_pool_size;

    /// @brief Время ожидания доступного соединения из пула
    static std::chrono::seconds wait_time;

    /**
     * @brief Возвращает единственный экземпляр пула соединений (Singleton)
     * @return Умный указатель на экземпляр пула соединений
     * @note Использует статические настройки connection_settings, start_pool_size, max_pool_size и wait_time
     */
    static std::shared_ptr<connection_pool> instance();

public:
    /**
     * @brief Конструктор пула соединений
     * @param settings Настройки подключения к базе данных
     * @param start_pool_size Начальный размер пула соединений
     * @param max_pool_size Максимальный размер пула соединений
     * @param wait_time Время ожидания доступного соединения из пула
     */
    explicit connection_pool(database_connection_settings settings, size_t start_pool_size = 2, size_t max_pool_size = 10, std::chrono::seconds wait_time = std::chrono::seconds(2));

protected:
    /**
     * @brief Создает новое соединение с базой данных PostgreSQL
     * @param settings Настройки подключения к базе данных
     * @return Умный указатель на созданное соединение
     */
    std::shared_ptr<IConnection> create_connection(const database_connection_settings& settings) override;
};

} // namespace postgre
} // namespace database_adapter

