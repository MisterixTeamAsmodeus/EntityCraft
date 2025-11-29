#pragma once

#include "postgreadapter_global.hpp"

#include <DatabaseAdapter/iconnection.hpp>
#include <DatabaseAdapter/ilogger.hpp>
#include <DatabaseAdapter/transaction_isolation.hpp>
#include <libpq-fe.h>

#include <memory>
#include <mutex>
#include <unordered_set>

namespace database_adapter {
class POSTGRE_EXPORT IConnection;
class POSTGRE_EXPORT ILogger;
}

namespace database_adapter {
namespace postgre {
/**
 * @brief Алиас для настроек подключения к PostgreSQL
 */
using settings = database_connection_settings;

/**
 * @brief Класс для работы с подключением к базе данных PostgreSQL
 * @details Реализует интерфейс IConnection для работы с PostgreSQL через libpq
 */
class POSTGRE_EXPORT connection final : public IConnection
{
public:
    /**
     * @brief Устанавливает глобальный логгер для всех соединений
     * @param logger Умный указатель на объект логгера
     */
    static void set_logger(std::shared_ptr<ILogger>&& logger);

public:
    /**
     * @brief Конструктор соединения с базой данных PostgreSQL
     * @param settings Настройки подключения к базе данных
     * @param needCreateDatabaseIfNotExist Создавать ли базу данных, если она не существует
     * @param retryCount Количество попыток подключения при ошибке
     * @param retryDeltaSeconds Интервал между попытками подключения в секундах
     * @throws open_database_exception При ошибке подключения к базе данных
     */
    explicit connection(const settings& settings, bool needCreateDatabaseIfNotExist = false, int retryCount = 4, int retryDeltaSeconds = 2);
    
    /**
     * @brief Деструктор соединения
     * @details Автоматически закрывает соединение и очищает все подготовленные запросы
     */
    ~connection() override;

    /**
     * @brief Проверяет валидность соединения
     * @return true, если соединение активно и готово к работе, иначе false
     */
    bool is_valid() override;
    
    /**
     * @brief Выполняет SQL-запрос к базе данных
     * @param query SQL-запрос для выполнения
     * @return Результат выполнения запроса
     * @throws sql_exception При ошибке выполнения запроса
     */
    query_result exec(const std::string& query) override;

    /**
     * @brief Подготавливает SQL-запрос для последующего выполнения с параметрами
     * @param query SQL-запрос для подготовки
     * @param name Имя подготовленного запроса
     * @throws sql_exception При ошибке подготовки запроса
     */
    void prepare(const std::string& query, const std::string& name) override;
    
    /**
     * @brief Выполняет подготовленный запрос с параметрами
     * @param params Параметры для подстановки в запрос
     * @param name Имя подготовленного запроса
     * @return Результат выполнения запроса
     * @throws sql_exception При ошибке выполнения или если запрос не был подготовлен
     */
    query_result exec_prepared(const std::vector<std::string>& params, const std::string& name) override;

    /**
     * @brief Открывает транзакцию с указанным уровнем изоляции
     * @param level Уровень изоляции транзакции
     * @return true, если транзакция успешно открыта, иначе false
     */
    bool open_transaction(transaction_isolation_level level) override;

private:
    /**
     * @brief Подключается к базе данных PostgreSQL
     * @param settings Настройки подключения
     * @throws open_database_exception При ошибке подключения
     */
    void connect(const settings& settings);
    
    /**
     * @brief Отключается от базы данных и очищает все подготовленные запросы
     */
    void disconnect();
    
    /**
     * @brief Очищает все подготовленные запросы на сервере
     */
    void cleanup_prepared_statements();

private:
    static std::shared_ptr<ILogger> _logger;

private:
    PGconn* _connection = nullptr;
    
    /// @brief Мьютекс для защиты операций с подготовленными запросами
    mutable std::mutex _prepared_mutex;
    
    /// @brief Множество имен подготовленных запросов
    std::unordered_set<std::string> _prepared_statements;
};

} // namespace postgre
} // namespace database_adapter

