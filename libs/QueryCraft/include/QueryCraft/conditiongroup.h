#pragma once

#include "enum/conditionviewtype.h"
#include "enum/logicaloperator.h"
#include "operator/equalsoperator.h"
#include "operator/inoperator.h"
#include "operator/lessoperator.h"
#include "operator/lessorequalsoperator.h"
#include "operator/moreoperator.h"
#include "operator/moreorequalsoperator.h"
#include "operator/notequalsoperator.h"
#include "operator/notinoperator.h"

#include <TypeConverterApi/typeconverterapi.h>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <tuple>
#include <vector>

namespace query_craft {

/// Структура condition_group предоставляет возможность создания сложных условий с использованием логических операторов
/// (И, ИЛИ) и поддерживает создание условий для столбцов таблицы.
struct condition_group
{
    /// Cтруктура представляющая отдельное условие.
    struct condition
    {
        /// Определяет структуру для хранения информации о столбце таблицы.
        struct column
        {

            /**
             * @brief Перечисление настроек столбца в базе данных.
             */
            enum class settings : uint8_t
            {
                /**
                 * Нет настроек.
                 */
                none = 0,

                /**
                 * Столбец является первичным ключом.
                 */
                primary_key = 1 << 0,

                not_null = 1 << 1,

                /**
                 * Настройка для primary key которая будет возвращать созданный id после insert запроса
                 */
                auto_increment = 1 << 2,
            };

            /**
             * Возвращает константу, представляющую пустое значение для столбца.
             * @return Строка "NULL".
             */
            constexpr static char* null_value()
            {
                return "NULL";
            }

            column() = default;

            /**
             * Конструктор с одним параметром. Инициализирует столбец с указанным именем и настройками по умолчанию.
             * @param name Имя столбца.
             * @param settings Настройки столбца.
             */
            explicit column(std::string name, settings settings = settings::none);

            column(const column& other) = default;

            column(column&& other) noexcept = default;

            bool operator==(const column& rhs) const;

            bool operator!=(const column& rhs) const;

            /**
             * Возвращает имя столбца.
             * @return Имя столбца.
             */
            std::string name() const;

            /**
             * Возвращает полное имя столбца.
             * @return Полное имя столбца.
             */
            std::string full_name() const;

            /**
             * Устанавливает полное имя столбца.
             * @param fullName Полное имя столбца.
             */
            void set_full_name(const std::string& fullName);

            /**
             * Возвращает псевдоним столбца.
             * @return Псевдоним столбца.
             */
            std::string alias() const;

            /**
             * Устанавливает псевдоним столбца.
             * @param alias Псевдоним столбца.
             */
            void set_alias(const std::string& alias);

            /**
             * Добавляет настройку к столбцу.
             * @param settings Настройка, которую нужно добавить.
             */
            void add_settings(settings settings);

            /**
             * Проверяет, имеет ли столбец указанную настройку.
             * @param settings Настройка, которую нужно проверить.
             * @return true, если столбец имеет указанную настройку, иначе false.
             */
            bool has_settings(settings settings) const;

            column& operator=(const column& other) = default;

            column& operator=(column&& other) noexcept = default;

            /**
             * Возвращает условие, проверяющее, является ли значение столбца пустым.
             * @return Условие, проверяющее, является ли значение столбца пустым.
             */
            condition is_null() const;

            /**
             * Возвращает условие, проверяющее, является ли значение столбца не пустым.
             * @return Условие, проверяющее, является ли значение столбца не пустым.
             */
            condition not_null() const;

            condition like(const std::string& pattern) const;

            /**
             * Возвращает условие, проверяющее, принадлежит ли значение столбца к указанным значениям.
             * @tparam Args Типы аргументов, которые могут быть преобразованы в строки.
             * @param args Значения, к которым должно принадлежать значение столбца.
             * @return Условие, проверяющее, принадлежит ли значение столбца к указанным значениям.
             */
            template<typename... Args>
            condition in(Args&&... args) const
            {
                std::vector<std::string> values;
                for(const auto& arg : { std::forward<Args>(args)... }) {
                    values.emplace_back(type_converter_api::type_converter<decltype(arg)>().convert_to_string(arg));
                }

                return create_condition(std::make_shared<operators::in_operator>(), std::move(values));
            }

            /**
             * Возвращает условие, проверяющее, принадлежит ли значение столбца к значениям, содержащимся в указанном диапазоне.
             * @tparam StartIt Тип итератора начала диапазона.
             * @tparam EndIt Тип итератора конца диапазона.
             * @param start_it Итератор начала диапазона.
             * @param end_it Итератор конца диапазона.
             * @return Условие, проверяющее, принадлежит ли значение столбца к значениям, содержащимся в указанном диапазоне.
             */
            template<class StartIt, class EndIt>
            condition in_list(StartIt&& start_it, EndIt&& end_it) const
            {
                std::vector<std::string> values;

                std::for_each(start_it, end_it, [&values](const auto& arg) {
                    values.emplace_back(type_converter_api::type_converter<decltype(arg)>().convert_to_string(arg));
                });

                return create_condition(std::make_shared<operators::in_operator>(), std::move(values));
            }

            /**
             * Возвращает условие, проверяющее, не принадлежит ли значение столбца к указанным значениям.
             * @tparam Args Типы аргументов, которые могут быть преобразованы в строки.
             * @param args Значения, к которым не должно принадлежать значение столбца.
             * @return Условие, проверяющее, не принадлежит ли значение столбца к указанным значениям.
             */
            template<typename... Args>
            condition notIn(Args&&... args) const
            {
                std::vector<std::string> values;
                for(const auto& arg : { std::forward<Args>(args)... })
                    values.emplace_back(type_converter_api::type_converter<decltype(arg)>().convert_to_string(arg));

                return create_condition(std::make_shared<operators::not_in_operator>(), std::move(values));
            }

            /**
             * Возвращает условие, проверяющее, не принадлежит ли значение столбца к значениям, содержащимся в указанном диапазоне.
             * @tparam StartIt Тип итератора начала диапазона.
             * @tparam EndIt Тип итератора конца диапазона.
             * @param start_it Итератор начала диапазона.
             * @param end_it Итератор конца диапазона.
             * @return Условие, проверяющее, не принадлежит ли значение столбца к значениям, содержащимся в указанном диапазоне.
             */
            template<class StartIt, class EndIt>
            condition not_in_list(StartIt&& start_it, EndIt&& end_it) const
            {
                std::vector<std::string> values;

                std::for_each(start_it, end_it, [&values](const auto& arg) {
                    values.emplace_back(type_converter_api::type_converter<decltype(arg)>().convert_to_string(arg));
                });

                return create_condition(std::make_shared<operators::not_in_operator>(), std::move(values));
            }

            /**
             * Возвращает условие, проверяющее, равно ли значение столбца указанному значению.
             * @tparam T Тип значения, которое может быть преобразовано в строку.
             * @param value Значение, к которому должно быть равно значение столбца.
             * @return Условие, проверяющее, равно ли значение столбца указанному значению.
             */
            template<typename T>
            condition operator==(const T& value) const
            {
                return create_condition(std::make_shared<operators::equals_operator>(),
                    { type_converter_api::type_converter<T>().convert_to_string(value) });
            }

            /**
             * Возвращает условие, проверяющее, равно ли значение столбца указанному столбцу.
             * @param value Столбец с которомы производится сравнение.
             * @return Условие, проверяющее, равны ли значения столбцов.
             */
            condition equals(const column& value) const;

            /**
             * Возвращает условие, проверяющее, не равно ли значение столбца указанному значению.
             * @tparam T Тип значения, которое может быть преобразовано в строку.
             * @param value Значение, к которому не должно быть равно значение столбца.
             * @return Условие, проверяющее, не равно ли значение столбца указанному значению.
             */
            template<typename T>
            condition operator!=(const T& value) const
            {
                return create_condition(std::make_shared<operators::not_equals_operator>(),
                    { type_converter_api::type_converter<T>().convert_to_string(value) });
            }

            /**
             * Возвращает условие, проверяющее, не равно ли значение столбца указанному столбцу.
             * @param value Столбец с которомы производится сравнение.
             * @return Условие, проверяющее, не равны ли значения столбцов.
             */
            condition not_equals(const column& value) const;

            /**
             * Возвращает условие, проверяющее, меньше ли значение столбца указанному значению.
             * @tparam T Тип значения, которое может быть преобразовано в строку.
             * @param value Значение, к которому должно быть меньше значение столбца.
             * @return Условие, проверяющее, меньше ли значение столбца указанному значению.
             */
            template<typename T>
            condition operator<(const T& value) const
            {
                return create_condition(std::make_shared<operators::less_operator>(),
                    { type_converter_api::type_converter<T>().convert_to_string(value) });
            }

            /**
             * Возвращает условие, проверяющее, меньше ли значение столбца указанному столбцу.
             * @param value Столбец с которомы производится сравнение.
             * @return Условие, проверяющее, меньше ли значения столбцов.
             */
            condition less(const column& value) const;

            /**
             * Возвращает условие, проверяющее, меньше или равно ли значение столбца указанному значению.
             * @tparam T Тип значения, которое может быть преобразовано в строку.
             * @param value Значение, к которому должно быть меньше или равно значение столбца.
             * @return Условие, проверяющее, меньше или равно ли значение столбца указанному значению.
             */
            template<typename T>
            condition operator<=(const T& value) const
            {
                return create_condition(std::make_shared<operators::less_or_equals_operator>(),
                    { type_converter_api::type_converter<T>().convert_to_string(value) });
            }

            /**
             * Возвращает условие, проверяющее, меньше или равно значение столбца указанному столбцу.
             * @param value Столбец с которомы производится сравнение.
             * @return Условие, проверяющее, меньше или равно значения столбцов.
             */
            condition less_or_equals(const column& value) const;

            /**
             * Возвращает условие, проверяющее, больше ли значение столбца указанному значению.
             * @tparam T Тип значения, которое может быть преобразовано в строку.
             * @param value Значение, к которому должно быть больше значение столбца.
             * @return Условие, проверяющее, больше ли значение столбца указанному значению.
             */
            template<typename T>
            condition operator>(const T& value) const
            {
                return create_condition(std::make_shared<operators::more_operator>(),
                    { type_converter_api::type_converter<T>().convert_to_string(value) });
            }

            /**
             * Возвращает условие, проверяющее, больше ли значение столбца указанному столбцу.
             * @param value Столбец с которомы производится сравнение.
             * @return Условие, проверяющее, больше ли значения столбцов.
             */
            condition more(const column& value) const;

            /**
             * Возвращает условие, проверяющее, больше или равно ли значение столбца указанному значению.
             * @tparam T Тип значения, которое может быть преобразовано в строку.
             * @param value Значение, к которому должно быть больше или равно значение столбца.
             * @return Условие, проверяющее, больше или равно ли значение столбца указанному значению.
             */
            template<typename T>
            condition operator>=(const T& value) const
            {
                return create_condition(std::make_shared<operators::more_or_equals_operator>(),
                    { type_converter_api::type_converter<T>().convert_to_string(value) });
            }

            /**
             * Возвращает условие, проверяющее, больше или равно значение столбца указанному столбцу.
             * @param value Столбец с которомы производится сравнение.
             * @return Условие, проверяющее, больше или равно значения столбцов.
             */
            condition more_or_equals(const column& value) const;

            /**
             * Проверяет, является ли условие валидным.
             * @return true, если была создана колонка, иначе false.
             */
            bool is_valid() const;

        private:
            /**
             * Создает и возвращает условие, основанное на указанном операторе и значениях.
             * @param conditionOperator Указатель на интерфейс оператора условия.
             * @param values Значения, которые будут использованы в условии.
             * @return Объект условия.
             */
            condition create_condition(std::shared_ptr<operators::IOperator>&& conditionOperator,
                std::vector<std::string>&& values,
                bool need_forging = true) const;

        private:
            /// Имя столбца.
            std::string _name {};

            /// Полное имя столбца.
            std::string _fullName {};

            /// Псевдоним столбца.
            std::string _alias {};

            /// Настройки столбца.
            settings _columnSettings = settings::none;
        };

        /**
         * Оператор логического "И" для объединения текущего условия с другим условием.
         * @param rhd Другое условие.
         * @return Объект ConditionGroup, представляющий объединенное условие.
         */
        condition_group operator&&(const condition& rhd) const;

        /**
         * Оператор логического "И" для объединения текущего условия с другим условием.
         * @param rhd Другое условие.
         * @return Объект ConditionGroup, представляющий объединенное условие.
         */
        condition_group operator&&(const condition_group& rhd) const;

        /**
         * Оператор логического "ИЛИ" для объединения текущего условия с другим условием.
         * @param rhd Другое условие.
         * @return Объект ConditionGroup, представляющий объединенное условие.
         */
        condition_group operator||(const condition& rhd) const;

        /**
         * Оператор логического "ИЛИ" для объединения текущего условия с другим условием.
         * @param rhd Другое условие.
         * @return Объект ConditionGroup, представляющий объединенное условие.
         */
        condition_group operator||(const condition_group& rhd) const;

        /**
         * Возвращает строковое представление текущего условия.
         * @param view_type Настройки для отображения названия колонки.
         * @return Строковое представление текущего условия.
         */
        std::string unwrap(condion_view_type view_type = condion_view_type::name) const;

        /**
         * Возвращает информацию о столбце текущего условия.
         * @return Объект ColumnInfo, содержащий информацию о столбце.
         */
        column condition_column() const;

        /**
         * Возвращает указатель на интерфейс оператора условия текущего условия.
         * @return Указатель на интерфейс оператора условия.
         */
        std::shared_ptr<operators::IOperator> condition_operator() const;

        /**
         * Возвращает значения текущего условия.
         * @return Вектор строк, содержащий значения текущего условия.
         */
        std::vector<std::string> values() const;

        /**
         * Проверяет, является ли текущее условие валидным.
         * @return true, если было создано условие, иначе false.
         */
        bool is_valid() const;

    private:
        std::shared_ptr<operators::IOperator> _condition_operator {};
        column _column {};
        std::vector<std::string> _values {};
        bool _need_forging = true;
    };

    condition_group() = default;

    /**
     * Конструктор, который создает объект condition_group на основе указанного условия.
     * @param condition Условие, которое будет использоваться для создания объекта ConditionGroup.
     */
    condition_group(const condition& condition);

    condition_group(const condition_group& other) = default;

    condition_group(condition_group&& other) noexcept = default;

    condition_group& operator=(const condition_group& other) = default;

    condition_group& operator=(condition_group&& other) noexcept = default;

    /**
     * Оператор логического "И" для объединения текущего условия с другим условием.
     * @param rhd Другое условие.
     * @return Объект ConditionGroup, представляющий объединенное условие.
     */
    condition_group operator&&(const condition_group& rhd) const;

    /**
     * Оператор логического "И" для объединения текущего условия с другим условием.
     * @param rhd Другое условие.
     * @return Объект ConditionGroup, представляющий объединенное условие.
     */
    condition_group operator&&(const condition& rhd) const;

    /**
     * Оператор логического "ИЛИ" для объединения текущего условия с другим условием.
     * @param rhd Другое условие.
     * @return Объект ConditionGroup, представляющий объединенное условие.
     */
    condition_group operator||(const condition_group& rhd) const;

    /**
     * Оператор логического "ИЛИ" для объединения текущего условия с другим условием.
     * @param rhd Другое условие.
     * @return Объект ConditionGroup, представляющий объединенное условие.
     */
    condition_group operator||(const condition& rhd) const;

    /**
     * Возвращает строковое представление текущего условия.
     * @param view_type Настройки для отображения названия колонки.
     * @param compressed Сжать выходную строку, если это возможно.
     * @return Строковое представление текущего условия.
     */
    std::string unwrap(condion_view_type view_type = condion_view_type::name, bool compressed = true) const;

    /**
     * Проверяет, является ли текущее условие валидным.
     * @return true, если было создано условие, иначе false.
     */
    bool is_valid() const;

private:
    /**
     * Проверяет, является ли текущее условие листом (не содержит дочерних условий).
     * @return true, если текущее условие является листом, иначе false.
     */
    bool is_sheet() const;

    /**
     * Рекурсивно обходит дерево условий и создает строковое представление.
     * @param node Указатель на текущее условие.
     * @param stream Строковый поток, куда будут добавляться условия.
     * @param view_type Настройки для отображения названия колонки.
     * @param compressed Сжать выходную строку.
     */
    static void unwrap_tree(const condition_group* node, std::stringstream& stream, condion_view_type view_type, bool compressed);

private:
    /**
     * Поле, содержащее кортеж, который хранит логический оператор и условие.
     * Используется для построения дерева условий в группе условий.
     */
    std::tuple<logical_operator, condition> _node {};

    /**
     * Указатель на левое поддерево группы условий.
     * Используется для представления сложных условий с использованием логических операторов.
     */
    std::shared_ptr<condition_group> _left {};

    /**
     * Указатель на правое поддерево группы условий.
     * Используется для представления сложных условий с использованием логических операторов.
     */
    std::shared_ptr<condition_group> _right {};
};

using column_info = condition_group::condition::column;
using condition_info = condition_group::condition;

using column_settings = condition_group::condition::column::settings;

/**
 * @brief Оператор "и" для перечисления ColumnInfo::Settings.
 *
 * @param a Первый операнд.
 * @param b Второй операнд.
 *
 * @return Результат операции "и" для двух перечислений ColumnInfo::Settings.
 */
column_settings operator|(column_settings a, column_settings b);

column_settings primary_key();

column_settings not_null();

} // namespace query_craft
