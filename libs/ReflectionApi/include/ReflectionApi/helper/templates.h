#pragma once

namespace reflection_api {
namespace helper {

/**
 * @brief Шаблонные псевдонимы типа для указателей на члены-функции, возвращающие значения.
 *
 * @tparam ClassType Тип класса, к которому принадлежат члены-функции.
 * @tparam PropertyType Тип значения членов-функции.
 * @note Описание константного геттера.
 */
template<typename ClassType, typename PropertyType>
using ConstGetter_t = PropertyType (ClassType::*)() const;

/**
 * @brief Шаблонные псевдонимы типа для указателей на члены-функции, возвращающие значения.
 *
 * @tparam ClassType Тип класса, к которому принадлежат члены-функции.
 * @tparam PropertyType Тип значения членов-функции.
 * @note Описание mutable геттера.
 */
template<typename ClassType, typename PropertyType>
using MutableGetter_t = PropertyType& (ClassType::*)();

/**
 * @brief Шаблонные псевдонимы типа для указателей на члены-функции, возвращающие значения.
 *
 * @tparam ClassType Тип класса, к которому принадлежат члены-функции.
 * @tparam PropertyType Тип значения членов-функции.
 * @note Описание геттера с изменяемым значением.
 */
template<typename ClassType, typename PropertyType>
using Getter_t = PropertyType (ClassType::*)();

/**
 * @brief Шаблонные псевдонимы типа для указателей на члены-функции, устанавливающие значение.
 *
 * @tparam ClassType Тип класса, к которому принадлежат члены-функции.
 * @tparam PropertyType Тип значения членов-функции.
 * @tparam Params Тип параметра, который принимает член-функция. По умолчанию это const ColumnType &.
 */
template<typename ClassType, typename PropertyType, typename Params = const PropertyType&>
using Setter_t = void (ClassType::*)(const Params&);

/**
 * @brief Шаблонные псевдонимы типа для указателей на члены-функции, устанавливающие значение.
 *
 * @tparam ClassType Тип класса, к которому принадлежат члены-функции.
 * @tparam PropertyType Тип значения членов-функции.
 * @tparam Params Тип параметра, который принимает член-функция. По умолчанию это ColumnType.
 */
template<typename ClassType, typename PropertyType, typename Params = PropertyType>
using BaseSetter_t = void (ClassType::*)(Params);

/**
 * @brief Шаблонные псевдонимы типа для указателей на члены-переменные.
 *
 * @tparam ClassType Тип класса, к которому принадлежат члены-переменные.
 * @tparam PropertyType Тип значения членов-переменных.
 */
template<typename ClassType, typename PropertyType>
using Variable_t = PropertyType ClassType::*;

} // namespace Helper
} // namespace reflection_api
