#pragma once

#include <string>

namespace query_craft {
namespace ast {

/**
 * @brief Узел, представляющий параметр prepared‑statement.
 *
 * Значения параметров хранятся как строки. Преобразование типов
 * может быть выполнено внешним кодом через TypeConverterApi.
 */
struct parameter
{
    /// Значение параметра.
    std::string value;
};

} // namespace ast
} // namespace query_craft
