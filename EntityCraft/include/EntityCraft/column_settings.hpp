#pragma once

#include <cstdint>

namespace entity_craft {

enum class column_settings : uint8_t
{
    empty = 0,
    not_null = 1 << 0,
    auto_increment = 1 << 1,
    primary_key = 1 << 2,
};

inline column_settings operator|(column_settings a, column_settings b)
{
    return static_cast<column_settings>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline column_settings operator&(column_settings a, column_settings b)
{
    return static_cast<column_settings>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

inline bool has_setting(column_settings settings, column_settings setting) noexcept
{
    return (settings & setting) == setting;
}

inline column_settings primary_key() noexcept
{
    return column_settings::primary_key;
}

inline column_settings primary_key_auto_increment() noexcept
{
    return primary_key() | column_settings::auto_increment;
}

inline column_settings not_null() noexcept
{
    return column_settings::not_null;
}

inline column_settings auto_increment() noexcept
{
    return column_settings::auto_increment;
}

} // namespace entity_craft