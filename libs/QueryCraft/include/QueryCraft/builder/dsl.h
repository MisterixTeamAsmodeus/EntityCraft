#pragma once

#include <QueryCraft/ast/expression.h>

#include <initializer_list>
#include <string>
#include <TypeConverterApi/typeconverterapi.hpp>

namespace query_craft {
namespace dsl {

/**
 * @brief Хелпер для создания идентификатора колонки.
 * @param name Имя колонки.
 * @param alias Алиас (опционально).
 * @param schema Схема таблицы (опционально).
 * @return Выражение типа identifier.
 */
ast::expression col(const std::string& name, const std::string& alias = {}, const std::string& schema = {});

/**
 * @brief Перегрузка для строковых литералов (const char*).
 * @param name Имя колонки.
 * @param alias Алиас (опционально).
 * @param schema Схема таблицы (опционально).
 * @return Выражение типа identifier.
 */
ast::expression col(const char* name, const char* alias = nullptr, const char* schema = nullptr);

/**
 * @brief Хелпер для создания строкового литерала.
 * @param v Значение.
 * @tparam T Тип значения.
 * @return Выражение типа literal.
 */
template<typename T>
ast::expression value(const T& v)
{
    ast::literal lit;
    lit.value = type_converter_api::type_converter<T>().convert_to_string(v);
    return ast::expression::make_literal(std::move(lit));
}

/**
 * @brief Хелпер для создания параметра prepared‑statement.
 * @param v Значение.
 * @tparam T Тип значения.
 * @return Выражение типа parameter.
 */
template<typename T>
ast::expression param(const T& v)
{
    ast::parameter p;
    p.value = type_converter_api::type_converter<T>().convert_to_string(v);
    return ast::expression::make_parameter(std::move(p));
}

/**
 * @brief Перегрузка для строковых литералов (const char*).
 * @param v Значение.
 * @return Выражение типа parameter.
 */
ast::expression param(const char* v);

/**
 * @brief Хелпер для создания вызова функции (включая агрегирующие).
 * @param name Имя функции.
 * @param args Аргументы функции.
 * @return Выражение типа function_call.
 */
ast::expression func(const std::string& name, std::initializer_list<ast::expression> args);

/**
 * @brief Хелпер для агрегирующей функции COUNT.
 * @param args Аргументы функции.
 * @return Выражение типа function_call.
 */
ast::expression count(std::initializer_list<ast::expression> args);

/**
 * @brief Хелпер для агрегирующей функции SUM.
 * @param args Аргументы функции.
 * @return Выражение типа function_call.
 */
ast::expression sum(std::initializer_list<ast::expression> args);

/**
 * @brief Хелпер для агрегирующей функции AVG.
 * @param args Аргументы функции.
 * @return Выражение типа function_call.
 */
ast::expression avg(std::initializer_list<ast::expression> args);

/**
 * @brief Хелпер для агрегирующей функции MIN.
 * @param args Аргументы функции.
 * @return Выражение типа function_call.
 */
ast::expression min(std::initializer_list<ast::expression> args);

/**
 * @brief Хелпер для агрегирующей функции MAX.
 * @param args Аргументы функции.
 * @return Выражение типа function_call.
 */
ast::expression max(std::initializer_list<ast::expression> args);

/**
 * @brief Хелпер для пользовательской агрегирующей функции.
 *
 * Логическое имя агрегата может быть переопределено диалектом через
 * sql_dialect::function_name(). Это позволяет привязать одно логическое
 * имя к разным физическим функциям СУБД.
 * @param logical_name Логическое имя агрегата.
 * @param args Аргументы функции.
 * @return Выражение типа function_call.
 * @note Пример:
 *   auto agg = custom_aggregate("json_array_agg_logical", { col("data") });
 *   // в диалекте PostgreSQL logical_name → "jsonb_array_agg"
 */
ast::expression custom_aggregate(const std::string& logical_name, std::initializer_list<ast::expression> args);

/**
 * @brief Пользовательский бинарный оператор по логическому имени.
 * @param left Левый операнд.
 * @param logical_operator_name Логическое имя оператора.
 * @param right Правый операнд.
 * @return Выражение типа custom_binary.
 * @note Пример: custom_op(col("data"), "json_get", param("key"))
 * Для конкретной СУБД диалект может сопоставить "json_get" с оператором "->".
 */
ast::expression custom_op(const ast::expression& left, const std::string& logical_operator_name, const ast::expression& right);

/**
 * @brief Хелпер для создания подзапроса (subquery).
 * @param query SELECT‑запрос.
 * @return Выражение типа subquery.
 * @note Пример:
 *   auto subq = subquery(select_builder().from("users").columns({col("id")}));
 *   builder.where(col("id") == subq);
 */
ast::expression subquery(const ast::select_query& query);

/**
 * @brief Хелпер для группировки условий с AND.
 * @param conditions Список условий.
 * @return Выражение типа binary.
 * @note Пример:
 *   builder.where(and_({
 *       col("age") > param(18),
 *       col("status") == param(std::string("active")),
 *       col("deleted") == param(false)
 *   }));
 */
ast::expression and_(std::initializer_list<ast::expression> conditions);

/**
 * @brief Хелпер для группировки условий с OR.
 * @param conditions Список условий.
 * @return Выражение типа binary.
 * @note Пример:
 *   builder.where(or_({
 *       col("status") == param(std::string("active")),
 *       col("status") == param(std::string("pending"))
 *   }));
 */
ast::expression or_(std::initializer_list<ast::expression> conditions);

/**
 * @brief Хелпер для создания условия IN.
 * @param left Левый операнд (обычно колонка).
 * @param right Правый операнд (подзапрос или выражение).
 * @return Выражение типа binary.
 * @note Пример:
 *   builder.where(in_(col("id"), subquery(select_builder().from("users").columns({col("id")}))));
 */
ast::expression in_(const ast::expression& left, const ast::expression& right);

} // namespace dsl
} // namespace query_craft

/**
 * @brief Перегрузка оператора ! для ast::expression.
 * @param expr Выражение.
 * @return Выражение типа unary.
 */
query_craft::ast::expression operator!(const query_craft::ast::expression& expr);

/**
 * @brief Перегрузка оператора && для ast::expression.
 * @param left Левый операнд.
 * @param right Правый операнд.
 * @return Выражение типа binary.
 */
query_craft::ast::expression operator&&(const query_craft::ast::expression& left, const query_craft::ast::expression& right);

/**
 * @brief Перегрузка оператора || для ast::expression.
 * @param left Левый операнд.
 * @param right Правый операнд.
 * @return Выражение типа binary.
 */
query_craft::ast::expression operator||(const query_craft::ast::expression& left, const query_craft::ast::expression& right);

/**
 * @brief Перегрузка оператора == для ast::expression.
 * @param left Левый операнд.
 * @param right Правый операнд.
 * @return Выражение типа binary.
 */
query_craft::ast::expression operator==(const query_craft::ast::expression& left, const query_craft::ast::expression& right);

/**
 * @brief Перегрузка оператора == для ast::expression.
 * @param left Левый операнд.
 * @param value_expr Значение.
 * @tparam T Тип значения.
 * @return Выражение типа binary.
 */
template<typename T>
query_craft::ast::expression operator==(const query_craft::ast::expression& left, const T& value_expr)
{
    return query_craft::ast::expression::make_binary(query_craft::ast::binary_operator::equals, left, value(value_expr));
}

/**
 * @brief Перегрузка оператора != для ast::expression.
 * @param left Левый операнд.
 * @param right Правый операнд.
 * @return Выражение типа binary.
 */
query_craft::ast::expression operator!=(const query_craft::ast::expression& left, const query_craft::ast::expression& right);

/**
 * @brief Перегрузка оператора != для query_craft::ast::expression.
 * @param left Левый операнд.
 * @param value_expr Значение.
 * @tparam T Тип значения.
 * @return Выражение типа binary.
 */
template<typename T>
query_craft::ast::expression operator!=(const query_craft::ast::expression& left, const T& value_expr)
{
    return query_craft::ast::expression::make_binary(query_craft::ast::binary_operator::not_equals, left, value(value_expr));
}

/**
 * @brief Перегрузка оператора < для query_craft::ast::expression.
 * @param left Левый операнд.
 * @param right Правый операнд.
 * @return Выражение типа binary.
 */
query_craft::ast::expression operator<(const query_craft::ast::expression& left, const query_craft::ast::expression& right);

/**
 * @brief Перегрузка оператора < для query_craft::ast::expression.
 * @param left Левый операнд.
 * @param value_expr Значение.
 * @tparam T Тип значения.
 * @return Выражение типа binary.
 */
template<typename T>
query_craft::ast::expression operator<(const query_craft::ast::expression& left, const T& value_expr)
{
    return query_craft::ast::expression::make_binary(query_craft::ast::binary_operator::less, left, value(value_expr));
}

/**
 * @brief Перегрузка оператора <= для query_craft::ast::expression.
 * @param left Левый операнд.
 * @param right Правый операнд.
 * @return Выражение типа binary.
 */
query_craft::ast::expression operator<=(const query_craft::ast::expression& left, const query_craft::ast::expression& right);

/**
 * @brief Перегрузка оператора <= для query_craft::ast::expression.
 * @param left Левый операнд.
 * @param value_expr Значение.
 * @tparam T Тип значения.
 * @return Выражение типа binary.
 */
template<typename T>
query_craft::ast::expression operator<=(const query_craft::ast::expression& left, const T& value_expr)
{
    return query_craft::ast::expression::make_binary(query_craft::ast::binary_operator::less_or_equals, left, value(value_expr));
}

/**
 * @brief Перегрузка оператора > для query_craft::ast::expression.
 * @param left Левый операнд.
 * @param right Правый операнд.
 * @return Выражение типа binary.
 */
query_craft::ast::expression operator>(const query_craft::ast::expression& left, const query_craft::ast::expression& right);

/**
 * @brief Перегрузка оператора > для query_craft::ast::expression.
 * @param left Левый операнд.
 * @param value_expr Значение.
 * @tparam T Тип значения.
 * @return Выражение типа binary.
 */
template<typename T>
query_craft::ast::expression operator>(const query_craft::ast::expression& left, const T& value_expr)
{
    return query_craft::ast::expression::make_binary(query_craft::ast::binary_operator::greater, left, value(value_expr));
}

/**
 * @brief Перегрузка оператора >= для query_craft::ast::expression.
 * @param left Левый операнд.
 * @param right Правый операнд.
 * @return Выражение типа binary.
 */
query_craft::ast::expression operator>=(const query_craft::ast::expression& left, const query_craft::ast::expression& right);

/**
 * @brief Перегрузка оператора >= для query_craft::ast::expression.
 * @param left Левый операнд.
 * @param value_expr Значение.
 * @tparam T Тип значения.
 * @return Выражение типа binary.
 */
template<typename T>
query_craft::ast::expression operator>=(const query_craft::ast::expression& left, const T& value_expr)
{
    return query_craft::ast::expression::make_binary(query_craft::ast::binary_operator::greater_or_equals, left, value(value_expr));
}
