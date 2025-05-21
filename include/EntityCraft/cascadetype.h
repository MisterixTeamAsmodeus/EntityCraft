#pragma once

#include <cstdint>

namespace entity_craft {

enum class cascade_type : uint8_t
{
    /// Каскадное сохранение применяется ко всем связанным сущностям.
    persist = 1 << 0,
    /// Каскадное обновление применяется ко всем связанным сущностям.
    merge = 1 << 1,
    /// Каскадное обновление применяется ко всем связанным сущностям. А так же определяет какие сущности были удалены из связанных
    merge_orphan = 1 << 2,
    /// Каскадное удаление применяется ко всем связанным сущностям.
    remove = 1 << 3,
    /// Операция каскадного удаления, сохранения и обновления применяется ко всем связанным сущностям.
    all = 1 << 4,
};

} // namespace entity_craft

inline entity_craft::cascade_type operator|(entity_craft::cascade_type a, entity_craft::cascade_type b)
{
    return static_cast<entity_craft::cascade_type>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}