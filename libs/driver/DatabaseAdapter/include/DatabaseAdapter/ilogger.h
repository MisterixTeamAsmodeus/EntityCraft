#pragma once

#include <string>

namespace database_adapter {

/// Класс для добавления логирования запросов и ошибок выполнения
class ILogger
{
public:
    virtual ~ILogger() = default;
    /**
     * Функция для логирования сообщения об ошибке
     * @param message Сообщение содержащее информацию об ошибке
     */
    virtual void log_error(const std::string& message) = 0;
    /**
     * Функция для логирования запросов для удобства отладки и контроля выполняемых запросов
     * @param message Сообщения содержащее sql выражение
     */
    virtual void log_sql(const std::string& message) = 0;
};
} // namespace database_adapter