#pragma once

#include <cstdint>

enum class RelationType : uint8_t
{
    ONE_TO_ONE,
    MANY_TO_ONE,
    ONE_TO_MANY
};