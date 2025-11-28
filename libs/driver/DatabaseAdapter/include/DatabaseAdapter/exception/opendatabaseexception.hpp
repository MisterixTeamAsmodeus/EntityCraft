#pragma once

#include <exception>
#include <string>

namespace database_adapter {

/// @brief Исключение, которое информирует о том, что произошла ошибка при подключении к базе данных
class open_database_exception final : public std::exception
{
public:
    /**
     * @brief Конструктор, который принимает информацию об ошибке подключения
     * @param message Информационное сообщение об ошибке
     * @param error_code Код ошибки (опционально, по умолчанию 0)
     */
    explicit open_database_exception(std::string message, int error_code = 0);

    open_database_exception(const open_database_exception& other) = default;
    open_database_exception(open_database_exception&& other) noexcept = default;
    open_database_exception& operator=(const open_database_exception& other) = default;
    open_database_exception& operator=(open_database_exception&& other) noexcept = default;

    ~open_database_exception() noexcept override = default;

    const char* what() const noexcept override;

    /**
     * @brief Функция для получения кода ошибки
     * @return Возвращает код ошибки
     */
    int error_code() const;

private:
    /// @brief Информация об ошибке подключения к базе данных
    std::string _message;
    /// @brief Код ошибки подключения
    int _error_code = 0;
};

} // namespace database_adapter