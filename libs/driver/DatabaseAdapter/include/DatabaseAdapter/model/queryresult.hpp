#pragma once

#include <string>
#include <unordered_map>
#include <vector>

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
     * @brief Конструктор, который принимает вектор result_row и инициализирует результат запроса
     * @param result Вектор result_row для инициализации результата
     */
    explicit query_result(const std::vector<row>& result);

    /**
     * @brief Добавляет новую строку результата в конец списка
     * @param value Строка результата для добавления
     */
    void add(const row& value);

    /**
     * @brief Возвращает копию результата
     * @return Возвращает копию вектора строк полученных после выполнения запроса
     */
    std::vector<row> data() const;

    /**
     * @brief Возвращает ссылку на результат
     * @return Возвращает ссылку на вектор строк полученных после выполнения запроса
     */
    std::vector<row>& mutable_data();

    /**
     * @brief Проверка на пустоту полученного результата
     * @return Возвращает true, если вектор пустой, иначе false
     */
    bool empty() const;

    /**
     * @brief Возвращает количество строк в результате
     * @return Количество строк в результате
     */
    size_t size() const;

    /**
     * @brief Возвращает итератор на начало результата
     * @return Итератор на первую строку результата
     */
    std::vector<row>::const_iterator cbegin() const;

    /**
     * @brief Возвращает итератор на конец результата
     * @return Итератор на позицию после последней строки результата
     */
    std::vector<row>::const_iterator cend() const;

    /**
     * @brief Возвращает итератор на начало результата (неконстантная версия)
     * @return Итератор на первую строку результата
     */
    std::vector<row>::iterator begin();

    /**
     * @brief Возвращает итератор на конец результата (неконстантная версия)
     * @return Итератор на позицию после последней строки результата
     */
    std::vector<row>::iterator end();

    /**
     * @brief Получить строку по индексу
     * @param index Индекс строки
     * @return Ссылка на строку результата
     * @throws std::out_of_range если индекс выходит за границы
     */
    const row& at(size_t index) const;

    /**
     * @brief Получить строку по индексу (неконстантная версия)
     * @param index Индекс строки
     * @return Ссылка на строку результата
     * @throws std::out_of_range если индекс выходит за границы
     */
    row& at(size_t index);

private:
    /// @brief Поле, которое хранит вектор всех строк результатов запроса
    std::vector<row> _result = {};
};

} // namespace database_adapter

#define NULL_VALUE "NULL"
