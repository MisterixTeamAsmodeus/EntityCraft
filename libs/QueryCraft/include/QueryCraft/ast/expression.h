#pragma once

#include "binary_expression.h"
#include "expression_type.h"
#include "function_call.h"
#include "identifier.h"
#include "literal.h"
#include "parameter.h"
#include "unary_expression.h"

#include <memory>

namespace query_craft {

namespace ast {
enum class unary_operator;
enum class binary_operator;
struct select_query;

/**
 * @brief Универсальное выражение, представленное как простейший tagged‑union.
 */
struct expression
{
    /**
     * @brief Создаёт идентификатор (identifier) как выражение.
     *
     * @param id Идентификатор.
     */
    static expression make_identifier(identifier id);

    /**
     * @brief Создаёт литерал (literal) как выражение.
     *
     * @param lit Литерал.
     */
    static expression make_literal(literal lit);

    /**
     * @brief Создаёт параметр (parameter) как выражение.
     *
     * @param param Параметр.
     */
    static expression make_parameter(parameter param);

    /**
     * @brief Создаёт вызов функции (function_call) как выражение.
     *
     * @param fn Вызов функции.
     */
    static expression make_function(function_call fn);

    /**
     * @brief Создаёт бинарное выражение (binary_expression) как выражение.
     *
     * @param op Оператор.
     * @param left Левый операнд.
     * @param right Правый операнд.
     */
    static expression make_binary(binary_operator op, expression left, expression right);

    /**
     * @brief Создаёт бинарное выражение с произвольным оператором.
     *
     * @param logical_name Логическое имя оператора. Диалект может
     *                     сопоставить его с конкретным SQL‑оператором,
     *                     например "json_get" → "->" в PostgreSQL.
     */
    static expression make_custom_binary(const std::string& logical_name, expression left, expression right);

    /**
     * @brief Создаёт унарное выражение (unary_expression) как выражение.
     *
     * @param op Оператор.
     * @param operand Операнд.
     */
    static expression make_unary(unary_operator op, expression operand);

    /**
     * @brief Создаёт унарное выражение с произвольным оператором.
     *
     * @param logical_name Логическое имя оператора. Диалект может
     *                     сопоставить его с конкретным SQL‑оператором,
     *                     например "jsonb_exists" → "?" в PostgreSQL.
     * @param operand Операнд.
     */
    static expression make_custom_unary(const std::string& logical_name, expression operand);

    /**
     * @brief Создаёт подзапрос (subquery) как выражение.
     *
     * Подзапросы могут использоваться в различных контекстах:
     * - в SELECT списке: SELECT (SELECT COUNT(*) FROM ...) AS count
     * - в WHERE условиях: WHERE id IN (SELECT id FROM ...)
     * - в FROM: FROM (SELECT ...) AS subquery
     *
     * @param query SELECT-запрос, который будет использован как подзапрос.
     */
    static expression make_subquery(select_query query);

    /**
     * @brief Конструктор по умолчанию.
     *
     * Определение вынесено в .cpp файл, чтобы избежать проблемы
     * с неполным типом select_query в заголовочном файле.
     */
    expression();

    /**
     * @brief Глубокое копирование выражения.
     *
     * Поскольку структура содержит std::unique_ptr, требуется
     * явная реализация copy‑ctor/copy‑assign, чтобы expression
     * можно было использовать в std::vector и других контейнерах.
     */
    expression(const expression& other);

    /**
     * @brief Оператор присваивания.
     *
     * Поскольку структура содержит std::unique_ptr, требуется
     * явная реализация copy‑ctor/copy‑assign, чтобы expression
     * можно было использовать в std::vector и других контейнерах.
     */
    expression& operator=(const expression& other);

    /**
     * @brief Перемещение выражения.
     *
     * После перемещения исходный объект остаётся в валидном
     * пустом состоянии (type = expression_type::literal).
     */
    expression(expression&& other) noexcept;

    /**
     * @brief Оператор перемещающего присваивания.
     *
     * После перемещения исходный объект остаётся в валидном
     * пустом состоянии (type = expression_type::literal).
     */
    expression& operator=(expression&& other) noexcept;

    /**
     * @brief Деструктор.
     *
     * Объявлен явно, чтобы определение можно было перенести в cpp файл,
     * где будет доступно полное определение select_query.
     */
    ~expression();

    /// Тип выражения.
    expression_type type { expression_type::literal };

    /// Идентификатор.
    identifier identifier_value;
    /// Литерал.
    literal literal_value;
    /// Параметр.
    parameter parameter_value;
    /// Вызов функции.
    std::unique_ptr<function_call> function_call_value;
    /// Бинарное выражение.
    std::unique_ptr<binary_expression> binary_value;
    /// Унарное выражение.
    std::unique_ptr<unary_expression> unary_value;
    /// Подзапрос.
    std::unique_ptr<select_query> subquery_value;
};

} // namespace ast
} // namespace query_craft
