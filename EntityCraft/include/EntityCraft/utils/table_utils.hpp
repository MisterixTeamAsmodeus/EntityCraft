#pragma once

#include "TypeConverterApi/helper/void_t.hpp"

#include <ReflectionApi/helper/tuplealgoritm.hpp>
#include <type_traits>
#include <utility>

namespace entity_craft {
namespace helper {
namespace impl {

/**
 * @brief Проверка наличия метода append_value у проперти
 * @tparam Property Тип проперти (например, reference_column)
 * @tparam ClassType Тип класса сущности
 * @tparam TargetType Тип добавляемого значения
 */
template<typename Property, typename ClassType, typename TargetType, typename = void>
struct has_append_value : std::false_type
{
};

template<typename Property, typename ClassType, typename TargetType>
struct has_append_value<Property,
    ClassType,
    TargetType,
    type_converter_api::sfinae::void_t<decltype(std::declval<Property&>().append_value(std::declval<ClassType&>(), std::declval<TargetType&&>()))>> : std::true_type
{
};

/**
 * @brief Реализация добавления значения в проперти, если она поддерживает append_value
 *
 * Специализация активируется только для проперти, у которых есть метод append_value.
 * В остальных случаях используется перегрузка-заглушка ниже.
 */
template<typename ClassType, typename Property, typename TargetType,
    std::enable_if_t<has_append_value<Property, ClassType, TargetType>::value, bool> = true>
constexpr void append_value_impl(ClassType&& obj, Property&& property, TargetType&& item)
{
    property.append_value(std::forward<ClassType>(obj), std::forward<TargetType>(item));
}

/**
 * @brief Перегрузка-заглушка для типов, не имеющих метода append_value
 */
template<typename ClassType, typename Property, typename TargetType,
    std::enable_if_t<!has_append_value<Property, ClassType, TargetType>::value, bool> = true>
constexpr void append_value_impl(ClassType&&, Property&&, TargetType&&)
{
    // Ничего не делаем, если append_value недоступен для данного типа проперти
}

} // namespace impl

/**
 * @brief Добавляет значение в указанную проперти таблицы, если она поддерживает append_value
 * @tparam ClassType Тип сущности
 * @tparam Tuple Тип кортежа пропертей
 * @tparam TargetType Тип добавляемого значения
 * @tparam Predicate Тип предиката для выбора целевой проперти
 * @param tuple Кортеж пропертей
 * @param obj Объект сущности
 * @param item Добавляемое значение
 * @param predicate Предикат, определяющий целевую проперти
 */
template<typename ClassType, typename Tuple, typename TargetType, typename Predicate>
constexpr void append_value(Tuple&& tuple, ClassType&& obj, TargetType&& item, Predicate&& predicate)
{
    reflection_api::helper::for_each(std::forward<Tuple>(tuple), [&predicate, &item, &obj](auto&& value) {
        if(predicate(std::forward<decltype(value)>(value))) {
            impl::append_value_impl(std::forward<ClassType>(obj), std::forward<decltype(value)>(value), std::forward<TargetType>(item));
        }
    });
}

} // namespace helper
} // namespace entity_craft