#pragma once

#include <string>

namespace query_craft {
namespace ast {

/**
 * @brief Описывает ссылку на идентификатор (таблица, колонка и т.п.).
 */
struct identifier
{
    /// Схема таблицы.
    std::string schema;
    /// Имя идентификатора (колонки, функции, etc.).
    std::string name;
    /// Псевдоним для идентификатора (для SELECT * FROM table AS t).
    std::string alias;
};

} // namespace ast
} // namespace query_craft
