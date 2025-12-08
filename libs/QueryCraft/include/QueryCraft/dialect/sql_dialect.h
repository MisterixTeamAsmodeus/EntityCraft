#pragma once

/**
 * @file sql_dialect.h
 * @brief Базовый интерфейс SQL‑диалекта для QueryCraft.
 */

#include <cstddef>
#include <string>

namespace query_craft {

/**
 * @brief Базовый интерфейс SQL‑диалекта.
 *
 * Реализации диалекта отвечают за:
 * - экранирование идентификаторов;
 * - форматирование LIMIT/OFFSET;
 * - генерацию плейсхолдеров для параметров;
 * - особенности синтаксиса отдельных конструкций.
 */
class sql_dialect
{
public:
    virtual ~sql_dialect() = default;

    /**
     * @brief Кавычит идентификатор (имя таблицы, колонки и т.п.).
     */
    virtual std::string quote_identifier(const std::string& identifier) const = 0;

    /**
     * @brief Возвращает строковое представление плейсхолдера параметра.
     *
     * @param index Порядковый номер параметра, начиная с 1.
     */
    virtual std::string placeholder(std::size_t index) const = 0;

    /**
     * @brief Форматирует часть LIMIT/OFFSET.
     *
     * @param limit  Лимит выборки. 0 — нет ограничения.
     * @param offset Смещение. 0 — без смещения.
     */
    virtual std::string format_limit_offset(std::size_t limit, std::size_t offset) const = 0;

    /**
     * @brief Возвращает SQL‑фрагмент для блокировки выбранных строк.
     *
     * Базовая реализация возвращает стандартный фрагмент
     * @code
     *  " FOR UPDATE"
     * @endcode
     * Конкретные диалекты могут переопределять поведение или возвращать
     * пустую строку, если конструкция не поддерживается.
     */
    virtual std::string select_for_update_clause() const
    {
        return " FOR UPDATE";
    }

    /**
     * @brief Возвращает true, если диалект поддерживает RETURNING.
     */
    virtual bool supports_returning() const = 0;

    /**
     * @brief Возвращает строковое имя функции по логическому имени.
     *
     * По умолчанию логическое имя уже может быть SQL‑именем.
     */
    virtual std::string function_name(const std::string& logical_name) const = 0;

    /**
     * @brief Возвращает строковое представление пользовательского бинарного оператора.
     *
     * @param logical_name Логическое имя оператора (например, "json_get").
     * По умолчанию возвращается logical_name без изменений. Конкретные
     * диалекты могут переопределить это поведение и сопоставить логическое
     * имя с оператором конкретной СУБД (например, "json_get" → "->").
     */
    virtual std::string custom_binary_operator(const std::string& logical_name) const
    {
        return logical_name;
    }

    /**
     * @brief Возвращает строковое представление пользовательского унарного оператора.
     *
     * @param logical_name Логическое имя оператора (например, "jsonb_exists").
     * По умолчанию возвращается logical_name без изменений. Конкретные
     * диалекты могут переопределить это поведение и сопоставить логическое
     * имя с оператором конкретной СУБД (например, "jsonb_exists" → "?").
     */
    virtual std::string custom_unary_operator(const std::string& logical_name) const
    {
        return logical_name;
    }
};

} // namespace query_craft
