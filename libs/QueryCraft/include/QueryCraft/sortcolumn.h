#pragma once

#include "conditiongroup.h"

#include <cstdint>

namespace query_craft {

struct sort_column
{
    /**
     * @brief Типы сортировки в SQL-запросах.
     */
    enum class type : uint8_t
    {
        /// Сортировка в порядке возрастания (ASC).
        asc,

        /// Сортировка в порядке убывания (DESC).
        desc
    };

    column_info column {};
    type sort_type = type::asc;
};

sort_column asc_sort(const column_info& column);

sort_column desc_sort(const column_info& column);

sort_column random_sort();

} // namespace query_craft