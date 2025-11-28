#pragma once

namespace database_adapter {

/**
 * @brief Уровни изоляции транзакций
 * @note Конкретные значения зависят от реализации базы данных
 */
enum class transaction_isolation_level
{
    /// @brief Уровень изоляции по умолчанию (зависит от базы данных)
    DEFAULT = -1,

    /// @brief READ UNCOMMITTED - самый низкий уровень изоляции
    READ_UNCOMMITTED = 0,

    /// @brief READ COMMITTED - чтение только закоммиченных данных
    READ_COMMITTED = 1,

    /// @brief REPEATABLE READ - повторяемое чтение
    REPEATABLE_READ = 2,

    /// @brief SERIALIZABLE - самый высокий уровень изоляции
    SERIALIZABLE = 3
};
} // namespace database_adapter

