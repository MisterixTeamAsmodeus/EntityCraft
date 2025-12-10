#pragma once

#include "sql_dialect.h"

namespace query_craft {

/**
 * @brief Реализация диалекта SQLite.
 */
class sqlite_dialect final : public sql_dialect
{
public:
    std::string quote_identifier(const std::string& identifier) const override;
    std::string placeholder(std::size_t index) const override;
    std::string format_limit_offset(std::size_t limit, std::size_t offset) const override;
    std::string select_for_update_clause() const override;
    bool supports_returning() const override;
    std::string function_name(const std::string& logical_name) const override;
};

} // namespace query_craft
