#pragma once

#include <cstdint>

namespace query_craft {

/**
 * @brief Перечисление логических операторов.
 */
enum class logical_operator : uint8_t
{
    /**
     * Логический оператор "И".
     */
    and_,

    /**
     * Логический оператор "ИЛИ".
     */
    or_
};

} // namespace query_craft
