#pragma once

#include <list>
#include <string>
#include <unordered_map>

namespace database_adapter {
namespace models {

/**
 * @brief Класс для хранения и предоставления результатов SQL-запросов
 */
class query_result
{
public:
    /// @brief Псевдоним для имени столбца результата
    typedef std::string column_name;

    /// @brief Псевдоним для значения столбца результата
    typedef std::string value;

    /// @brief Псевдоним для строки результата запроса
    typedef std::unordered_map<column_name, value> result_row;

    query_result() = default;

    /**
     * @brief Конструктор, который принимает список result_row и инициализирует результат запроса
     * @param result Список result_row для инициализации результата
     */
    explicit query_result(const std::list<result_row>& result);

    /**
     * @brief Добавляет новую строку результата в конец списка
     * @param value Строка результата для добавления
     */
    void add_row(const result_row& value);

    /**
     * @brief Возвращает список всех строк результата
     * @return Список всех строк результата
     */
    std::list<result_row> data() const;

    /**
     * Перегрузка оператора для удобства использования внутри цикла
     * @return Список всех строк результата
     */
    std::list<result_row>& operator()();

    bool empty() const;

private:
    /// @brief Поле, которое хранит список всех строк результатов запроса
    std::list<result_row> _result = {};
};
} // namespace models
} // namespace database_adapter
