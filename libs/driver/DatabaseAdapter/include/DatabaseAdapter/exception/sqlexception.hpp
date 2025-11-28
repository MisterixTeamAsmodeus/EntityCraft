#pragma once

#include <exception>
#include <string>

namespace database_adapter {

/// @brief Исключение, которое информирует о том, что при ошибке выполнения SQL-запроса. Оно содержит информацию об ошибке и строку запроса на котором произошла ошибка
class sql_exception final : public std::exception
{
public:
    /**
     * @brief Конструктор, который принимает информацию об ошибки выполнения SQL запроса
     * @param message Информационное сообщение об ошибке
     * @param last_query Запрос на котором произошла ошибка
     * @param error_code Код ошибки (опционально, по умолчанию 0)
     */
    explicit sql_exception(std::string message, std::string last_query = "", int error_code = 0);

    sql_exception(const sql_exception& other) = default;
    sql_exception(sql_exception&& other) noexcept = default;
    sql_exception& operator=(const sql_exception& other) = default;
    sql_exception& operator=(sql_exception&& other) noexcept = default;

    ~sql_exception() noexcept override = default;
    const char* what() const noexcept override;

    /**
     * @brief Функция для получения запроса на котором произошла ошибка
     * @return Возвращает запрос на котором произошла ошибка
     */
    std::string last_query() const;

    /**
     * @brief Функция для получения кода ошибки
     * @return Возвращает код ошибки
     */
    int error_code() const;

private:
    /// @brief Информация об ошибке выполнения SQL-запроса.
    std::string _message;
    /// @brief Текст последнего выполненного SQL-запроса.
    std::string _last_query {};
    /// @brief Код ошибки базы данных
    int _error_code = 0;
};

} // namespace database_adapter