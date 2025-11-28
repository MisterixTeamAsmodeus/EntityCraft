#pragma once

#include <cstdint>

namespace entity_craft {

enum class relation_type : uint8_t
{
    one_to_one = 0,
    many_to_one = 1,
    one_to_many = 2,
    /// Реализация ситуации когда ссылка хранится не в таргет структуре а в зависимой
    one_to_one_inverted = 3
};

} // namespace entity_craft
