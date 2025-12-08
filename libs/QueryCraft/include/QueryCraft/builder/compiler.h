#pragma once

#include "compiled_query.h"

#include <QueryCraft/ast/delete_query.h>
#include <QueryCraft/ast/insert_query.h>
#include <QueryCraft/ast/update_query.h>

#include <memory>

namespace query_craft {
class sql_dialect;

/**
 * @brief Компилятор AST‑запросов в SQL для конкретного диалекта.
 */
class sql_compiler
{
public:
    /**
     * @brief Конструктор.
     * @param dialect Диалект.
     * @note Конструктор принимает указатель на диалект, который будет использоваться для компиляции запроса.
     */
    explicit sql_compiler(std::shared_ptr<sql_dialect> dialect) noexcept;

    /**
     * @brief Компилирует SELECT‑запрос.
     * @param query SELECT‑запрос.
     * @return Скомпилированный запрос.
     */
    compiled_query compile(const ast::select_query& query) const;

    /**
     * @brief Компилирует INSERT‑запрос.
     * @param query INSERT‑запрос.
     * @return Скомпилированный запрос.
     */
    compiled_query compile(const ast::insert_query& query) const;

    /**
     * @brief Компилирует UPDATE‑запрос.
     * @param query UPDATE‑запрос.
     * @return Скомпилированный запрос.
     */
    compiled_query compile(const ast::update_query& query) const;

    /**
     * @brief Компилирует DELETE‑запрос.
     * @param query DELETE‑запрос.
     * @return Скомпилированный запрос.
     */
    compiled_query compile(const ast::delete_query& query) const;

private:
    /// Диалект.
    std::shared_ptr<sql_dialect> dialect_;
};

} // namespace query_craft
