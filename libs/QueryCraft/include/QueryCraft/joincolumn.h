#pragma once

#include "table.h"

#include <cstdint>

namespace query_craft {

struct join_column
{

    /**
     * @brief Типы соединений в SQL-запросах.
     */
    enum class type : uint8_t
    {
        /// Внутреннее соединение (INNER JOIN). Выбираются только те записи, которые имеют совпадения в обеих таблицах.
        inner,

        /// Внешнее соединение (OUTER JOIN). Выбираются все записи из обеих таблиц, даже если в одной из них нет совпадений.
        outer,

        /// Левое внешнее соединение (LEFT JOIN). Выбираются все записи из левой таблицы и совпадающие записи из правой таблицы.
        left,

        /// Правое внешнее соединение (RIGHT JOIN). Выбираются все записи из правой таблицы и совпадающие записи из левой таблицы.
        right,

        /// Перекрестное соединение (CROSS JOIN). Выбираются все возможные комбинации записей из двух таблиц.
        cross
    };

    type join_type = type::inner;
    table joined_table {};
    condition_group condition {};
};

std::ostream& operator<<(std::ostream& os, const join_column& obj);

} // namespace query_craft