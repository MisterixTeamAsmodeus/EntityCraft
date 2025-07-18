#pragma once

#include <cstdint>

namespace database_adapter {
namespace postgre {

/// Типы транзакции поддерживаемые в PostgreSQL
enum class transaction_type : uint8_t
{
    READ_UNCOMMITTED,
    READ_COMMITTED,
    REPEATABLE_READ,
    SERIALIZABLE
};

} // namespace postgre
} // namespace database_adapter