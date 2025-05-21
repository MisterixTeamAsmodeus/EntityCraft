#pragma once

#include <cstdint>

namespace entity_craft {

enum class relation_type : uint8_t
{
    one_to_one,
    many_to_one,
    one_to_many,
    /// Реализация ситуации когда ссылка хранится не в таргет структуре а в зависимой
    one_to_one_inverted
};

} // namespace entity_craft
