#pragma once

#include <cstdint>

namespace entity_craft {

enum class cascade_type : uint8_t
{
    /// Каскадное сохранение применяется ко всем связанным сущностям.
    persist = 1 << 0,
    /// Каскадное обновление применяется ко всем связанным сущностям.
    /// @note Обновление подразумевает из себя выполнение метода upsert, удаление связанных сущностей происходить не будет
    merge = 1 << 1,
    /// Каскадное удаление применяется ко всем связанным сущностям.
    remove = 1 << 2,
    /// Операция каскадного удаления, сохранения и обновления применяется ко всем связанным сущностям.
    all = 1 << 3,
};

} // namespace entity_craft

inline entity_craft::cascade_type operator|(entity_craft::cascade_type a, entity_craft::cascade_type b)
{
    return static_cast<entity_craft::cascade_type>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}