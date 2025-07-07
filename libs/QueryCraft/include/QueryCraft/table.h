#pragma once

#include "conditiongroup.h"

#include <unordered_map>

namespace query_craft {
/**
 * Класс Table представляет таблицу в базе данных и предоставляет интерфейс для работы с ее столбцами.
 */
class table
{
public:
    table() = default;

    /**
     * Конструктор с указанием имени таблицы и схемы.
     *
     * @param table_name Имя таблицы.
     * @param scheme    Имя схемы. По умолчанию пустая строка.
     * @param columns   Список столбцов таблицы. По умолчанию пустой список.
     */
    explicit table(std::string table_name, std::string scheme = "", const std::initializer_list<column_info>& columns = {});

    /**
     * Конструктор с указанием имени таблицы, схемы и переменного числа столбцов.
     *
     * @param table_name Имя таблицы.
     * @param scheme    Имя схемы.
     * @param columns   Столбцы таблицы.
     */
    template<typename... Args>
    explicit table(std::string table_name, std::string scheme, Args&&... columns)
        : _scheme(std::move(scheme))
        , _table_name(std::move(table_name))
    {
        auto columnList = { std::forward<Args>(columns)... };
        for(const column_info& column : columnList) {
            column_info tempColumn(column);
            add_column(tempColumn);
        }
    }

    /**
     * Конструктор с указанием имени таблицы, схемы и диапазона итераторов столбцов.
     *
     * @param table_name Имя таблицы.
     * @param scheme    Имя схемы.
     * @param startIt   Итератор начала диапазона столбцов.
     * @param endIt     Итератор конца диапазона столбцов.
     */
    template<class StartColumnIt, class EndColumnIt>
    explicit table(std::string table_name, std::string scheme, StartColumnIt&& startIt, EndColumnIt&& endIt)
        : _scheme(std::move(scheme))
        , _table_name(std::move(table_name))
    {
        std::for_each(startIt, endIt, [this](const column_info& column) {
            column_info tempColumn(column);
            add_column(tempColumn);
        });
    }

    table(const table& other) = default;

    table(table&& other) noexcept = default;

    table& operator=(const table& other) = default;

    table& operator=(table&& other) noexcept = default;

    /**
     * Добавление столбца в таблицу.
     *
     * @param column Столбец для добавления.
     * @return Ссылка на текущую таблицу.
     */
    table& add_column(column_info& column);

    /**
     * Получение полного имени таблицы.
     *
     * @return Имя таблицы с названием схемы если она есть.
     */
    std::string table_name() const;

    /**
     * Получение информации о столбце по его имени.
     *
     * @param name Имя столбца.
     * @return Информация о столбце.
     */
    column_info column(const std::string& name) const;

    /**
     * Получение информации о столбце по его номеру.
     *
     * @param index Номер столбца.
     * @return Информация о столбце.
     */
    column_info column(int index) const;

    /**
     * Получение списка столбцов таблицы.
     *
     * @return Список столбцов.
     */
    std::vector<column_info> columns() const;

protected:
    /// Название схемы таблицы.
    std::string _scheme {};
    /// Название таблицы.
    std::string _table_name {};
    /// Список столбцов таблицы. Нужен для сохранения порядка при запросах.
    std::vector<column_info> _columns;
    /// Таблица столбцов. Нужна для быстрого поиска столбца по имени за O(1).
    std::unordered_map<std::string, column_info> _columns_map {};
};

} // namespace query_craft