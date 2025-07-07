#pragma once

#include <exception>
#include <string>

namespace database_adapter {

/**
 * Исключение которое информирует о том, что при ошибке выполнения SQL-запроса.
 * Оно содержит информацию о ошибке и запрос на котором произошла ошибка
 */
class sql_exception final : public std::exception
{
public:
    explicit sql_exception(std::string message);
    sql_exception(std::string message, std::string last_query);

    sql_exception(const sql_exception& other) = default;
    sql_exception(sql_exception&& other) noexcept = default;
    sql_exception& operator=(const sql_exception& other) = default;
    sql_exception& operator=(sql_exception&& other) noexcept = default;

    ~sql_exception() noexcept override = default;
    const char* what() const noexcept override;

    std::string last_query() const;

private:
    /// Информация о ошибке выполнения SQL-запроса.
    std::string _message;
    /// Текст последнего выполненного SQL-запроса.
    std::string _last_query {};
};

} // namespace database_adapter