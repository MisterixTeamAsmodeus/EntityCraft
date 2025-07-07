#pragma once

#include <cstdint>

namespace database_adapter {
/// Типы транзакции поддерживаемые в sqlite3
enum class TransactionType : uint8_t
{
    DEFERRED = 0,
    IMMEDIATE,
    EXCLUSIVE
};
} // namespace database_adapter