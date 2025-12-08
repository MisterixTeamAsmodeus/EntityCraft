#pragma once

#include <string>

namespace query_craft {
namespace ast {

/**
 * @brief Узел, представляющий строковый литерал.
 */
struct literal
{
    /// Значение литерала. Например, "100", "true", "2025-01-01", etc.
    std::string value;
};

} // namespace ast
} // namespace query_craft
