#pragma once

#include <cstdint>

enum class RelationType : uint8_t
{
    ONE_TO_ONE,
    MANY_TO_ONE,
    ONE_TO_MANY,
    /// Реализация ситуации когда ссылка хранится не в таргет структуре а в зависимой
    ONE_TO_ONE_INVERTED
};