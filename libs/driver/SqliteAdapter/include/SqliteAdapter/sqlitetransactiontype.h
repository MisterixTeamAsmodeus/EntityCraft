#pragma once

#include <cstdint>

namespace database_adapter {
namespace sqlite {

/// Типы транзакции поддерживаемые в sqlite3
enum class transaction_type : uint8_t
{
    DEFERRED = 0,
    IMMEDIATE,
    EXCLUSIVE
};

} // namespace sqlite
} // namespace database_adapter