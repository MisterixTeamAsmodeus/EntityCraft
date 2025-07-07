#pragma once

#include <exception>
#include <string>

namespace database_adapter {
/**
 * Исключение которое информирует о том, что произошла ошибка при подключении к базе данных
 */
class open_database_exception final : public std::exception
{
public:
    explicit open_database_exception(std::string message);

    open_database_exception(const open_database_exception& other) = default;
    open_database_exception(open_database_exception&& other) noexcept = default;
    open_database_exception& operator=(const open_database_exception& other) = default;
    open_database_exception& operator=(open_database_exception&& other) noexcept = default;

    ~open_database_exception() noexcept override = default;

    const char* what() const noexcept override;

private:
    /// Информация о ошибке подключения к базе данных
    std::string _message;
};
} // namespace database_adapter