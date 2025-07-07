#pragma once

#include "helper/tuplehelper.h"
#include "joincolumn.h"
#include "sortcolumn.h"
#include "table.h"

namespace query_craft {

/// Класс, представляющий таблицу SQL.
/// Расширяет класс Table и добавляет функциональность для генерации sql запросов.
class sql_table : public table
{
public:
    /// Тип, представляющий строку таблицы.
    /// Каждая строка представляется в виде вектора значений столбцов.
    using row = std::vector<std::string>;

public:
    sql_table() = default;

    /**
     * Конструктор с указанием имени таблицы и схемы.
     *
     * @param table_name Имя таблицы.
     * @param scheme     Имя схемы. По умолчанию пустая строка.
     * @param columns    Список столбцов таблицы. По умолчанию пустой список.
     */

    explicit sql_table(std::string table_name, std::string scheme = "", const std::initializer_list<column_info>& columns = {});

    /**
     * Конструктор с указанием имени таблицы, схемы и переменного числа столбцов.
     *
     * @param table_name Имя таблицы.
     * @param scheme    Имя схемы.
     * @param columns   Столбцы таблицы.
     */
    template<typename... Args>
    explicit sql_table(std::string table_name, std::string scheme, Args&&... columns)
        : table(std::move(table_name),
              std::move(scheme),
              std::forward<Args>(columns)...)
    {
    }

    /**
     * Конструктор с указанием имени таблицы, схемы и диапазона итераторов столбцов.
     *
     * @param table_name Имя таблицы.
     * @param scheme    Имя схемы.
     * @param start_it   Итератор начала диапазона столбцов.
     * @param end_it     Итератор конца диапазона столбцов.
     */
    template<class StartColumnIt, class EndColumnIt>
    explicit sql_table(std::string table_name, std::string scheme, StartColumnIt&& start_it, EndColumnIt&& end_it)
        : sql_table(std::move(table_name),
              std::move(scheme),
              std::forward<StartColumnIt>(start_it),
              std::forward<EndColumnIt>(end_it))
    {
    }

    explicit sql_table(const table& other);

    explicit sql_table(table&& other);

    sql_table(const sql_table& other) = default;

    sql_table(sql_table&& other) noexcept = default;

    sql_table& operator=(const sql_table& other) = default;

    sql_table& operator=(sql_table&& other) noexcept = default;

    /**
     * Добавление строки в таблицу.
     *
     * @param row Строка для добавления.
     * @return Ссылка на текущую таблицу.
     */
    sql_table& add_row(const row& row);

    /**
     * Добавление строки в таблицу с использованием переменного числа аргументов.
     *
     * @param args Значения для добавления в качестве столбцов.
     * @return Ссылка на текущую таблицу.
     * @note Использует метод convertToString (по умолчанию использует оператор << для преобразования в строку),
     * для изменения поведения нужно переопределить метод для своего типа
     */
    template<typename... Args>
    sql_table& add_row_args(Args&&... args)
    {
        auto values = std::make_tuple<Args...>(std::forward<Args>(args)...);

        row row;

        helper::for_each(values, [&row](auto&& value) {
            row.emplace_back(type_converter_api::type_converter<decltype(value)>().convert_to_string(value));
        });

        if(!rows.empty() && rows.begin()->size() != row.size())
            throw std::logic_error("Ошибка. Не совпадает размер строки с уже добавленными в таблицу");

        rows.push_back(row);
        return *this;
    }

    /**
     * Генерация SQL-запроса для вставки строки в таблицу.
     *
     * @param columns Столбцы для вставки. По умолчанию все стол
     * @param need_returning Флаг означающий что в конце запроса необходимо вернуть вставленные колонки
     * @param returning_columns Колонки которые необходимо вернуть после вставкибцы.
     * @return SQL-запрос для вставки.
     * @note Очищает добавленные строки
     * @note Перегрузка для передачи параметров через initializer_list, так как бывают ситуации когда он не может не явно кастануть к vector
     */
    std::string insert_args(const std::initializer_list<column_info>& columns, bool need_returning = false, const std::initializer_list<column_info>& returning_columns = {});

    /**
     * Генерация SQL-запроса для вставки строки в таблицу.
     *
     * @param columns Столбцы для вставки. По умолчанию все столбцы.
     * @param need_returning Флаг означающий что в конце запроса необходимо вернуть вставленные колонки
     * @param returning_columns Колонки которые необходимо вернуть после вставки
     * @return SQL-запрос для вставки.
     * @note Очищает добавленные строки
     */
    std::string insert_sql(const std::vector<column_info>& columns = {}, bool need_returning = false, const std::vector<column_info>& returning_columns = {});

    /**
     * Генерация SQL-запроса для обновления строки в таблице.
     *
     * @param condition Условие для выбора строки.
     * @param columns   Столбцы для обновления. По умолчанию все столбцы.
     * @return SQL-запрос для обновления.
     * @note Очищает добавленные строки
     * @note Перегрузка для передачи параметров через initializer_list, так как бывают ситуации когда он не может не явно кастануть к vector
     */
    std::string update_args_sql(const condition_group& condition = {}, const std::initializer_list<column_info>& columns = {});

    /**
     * Генерация SQL-запроса для обновления строки в таблице.
     *
     * @param condition Условие для выбора строки.
     * @param columns   Столбцы для обновления. По умолчанию все столбцы.
     * @return SQL-запрос для обновления.
     * @note Очищает добавленные строки
     */
    std::string update_sql(const condition_group& condition = {}, const std::vector<column_info>& columns = {});

    /**
     * Генерация SQL-запроса для удаления строки из таблицы.
     *
     * @param condition Условие для выбора строки.
     * @return SQL-запрос для удаления.
     */
    std::string remove_sql(const condition_group& condition = {}) const;

    /**
     * Генерация SQL-запроса для выборки строк из таблицы.
     *
     * @param join_columns   Информация о join соединениях
     * @param condition     Условие для выбора строк.
     * @param sort_columns   Информация о колонках необходимых для сортировок
     * @param limit         Лимит выборки.
     * @param offset        Смещение выборки.
     * @param columns       Столбцы для выборки. По умолчанию все столбцы.
     * @return SQL-запрос для выборки.
     * @note Перегрузка для передачи параметров через initializer_list, так как бывают ситуации когда он не может не явно кастануть к vector
     */
    std::string select_args_sql(
        const std::initializer_list<join_column>& join_columns = {},
        const condition_group& condition = {},
        const std::initializer_list<sort_column>& sort_columns = {},
        size_t limit = 0,
        size_t offset = 0,
        const std::initializer_list<column_info>& columns = {}) const;

    /**
     * Генерация SQL-запроса для выборки строк из таблицы.
     *
     * @param join_columns   Информация о join соединениях
     * @param condition     Условие для выбора строк.
     * @param sort_columns   Информация о колонках необходимых для сортировок
     * @param limit         Лимит выборки.
     * @param offset        Смещение выборки.
     * @param columns       Столбцы для выборки. По умолчанию все столбцы.
     * @return SQL-запрос для выборки.
     */
    std::string select_sql(
        const std::vector<join_column>& join_columns = {},
        const condition_group& condition = {},
        const std::vector<sort_column>& sort_columns = {},
        size_t limit = 0,
        size_t offset = 0,
        const std::vector<column_info>& columns = {}) const;

private:
    /// Вектор, содержащий строки таблицы.
    /// Каждая строка представляется в виде вектора значений столбцов.
    std::vector<row> rows;
};

} // namespace query_craft