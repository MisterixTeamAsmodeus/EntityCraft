#pragma once

namespace query_craft {
namespace ast {

/**
 * @brief Базовый интерфейс для запросов.
 *
 * Нужен в первую очередь для удобства хранения указателей
 * на произвольные запросы, если это потребуется.
 */
struct query_base
{
    virtual ~query_base() = default;
};

} // namespace ast
} // namespace query_craft
