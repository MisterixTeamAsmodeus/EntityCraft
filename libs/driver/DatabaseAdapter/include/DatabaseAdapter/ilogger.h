#pragma once

#include <string>

namespace database_adapter {
/// Класс для добавления системы логирования в работу ORM и отдельных запросов
class ILogger
{
public:
    virtual ~ILogger() = default;
    /**
     * Функция для логирования сообщения об ошибке
     * @param message Сообщение содержащее информацию о ошибке
     */
    virtual void log_error(const std::string& message) = 0;
    /**
     * Функция для логирования запросов для удобства отладки и контроля
     * @param message Сообщения содержащее sql выражение
     */
    virtual void log_sql(const std::string& message) = 0;
};
} // namespace database_adapter