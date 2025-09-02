#pragma once

#include <list>
#include <string>
#include <unordered_map>

namespace database_adapter {

/**
 * @brief Класс для хранения и представления результатов SQL-запросов
 */
class query_result
{
public:
    /// @brief Псевдоним для имени столбца результата
    using column_name = std::string;

    /// @brief Псевдоним для значения столбца результата
    using value = std::string;

    /// @brief Псевдоним для строки результата запроса
    using row = std::unordered_map<column_name, value>;

    query_result() = default;

    /**
     * @brief Конструктор, который принимает список result_row и инициализирует результат запроса
     * @param result Список result_row для инициализации результата
     */
    explicit query_result(const std::list<row>& result);

    /**
     * @brief Добавляет новую строку результата в конец списка
     * @param value Строка результата для добавления
     */
    void add(const row& value);

    /**
     * @brief Возвращает копию результата
     * @return Возвращает копию списка строк полученных после выполнения запроса
     */
    std::list<row> data() const;

    /**
     * @brief Возвращает ссылку на результат
     * @return Возвращает ссылку на список строк полученных после выполнения запроса
     */
    std::list<row>& mutable_data();

    /**
     * @brief Проверка на пустоту полученного результата
     * @return Возвращает true, если список пустой, иначе false
     */
    bool empty() const;

private:
    /// @brief Поле, которое хранит список всех строк результатов запроса
    std::list<row> _result = {};
};

} // namespace database_adapter
