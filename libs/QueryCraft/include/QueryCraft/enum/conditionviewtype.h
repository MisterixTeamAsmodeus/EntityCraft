#pragma once

#include <cstdint>

namespace query_craft {

/// @brief Перечисление для определения вида отображения названия колонки.
enum class condion_view_type : uint8_t
{
    /// Представление по имени.
    name,
    /// Представление по псевдониму.
    alias,
    /// Полное представление.
    full_name
};

} // namespace query_craft