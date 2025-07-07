#pragma once

#include <cstddef>
#include <tuple>
#include <utility>

namespace query_craft {
namespace helper {
namespace impl {

/**
 * @brief Рекурсивная реализация цикла for_each для кортежей.
 *
 * @tparam Tuple Тип кортежа.
 * @tparam F Тип функции обратного вызова.
 * @tparam Indices Последовательность индексов для раскрытия кортежа.
 *
 * @param tuple Кортеж, для которого выполняется цикл for_each.
 * @param f Функция обратного вызова, которая вызывается для каждого элемента кортежа.
 */
template<typename Tuple, typename F, std::size_t... Indices>
void for_each_impl(Tuple&& tuple, F&& f, std::index_sequence<Indices...>)
{
    using swallow = int[];
    (void)swallow {
        1, (f(std::get<Indices>(std::forward<Tuple>(tuple))), void(), int {})...
    };
}

} // namespace impl

/**
 * @brief Выполняет цикл for_each для кортежа.
 *
 * @tparam F Тип функции обратного вызова.
 * @tparam Args Типы элементов кортежа.
 *
 * @param tuple Кортеж, для которого выполняется цикл for_each.
 * @param f Функция обратного вызова, которая вызывается для каждого элемента кортежа.
 */
template<typename F, typename... Args>
void for_each(const std::tuple<Args...>& tuple, F&& f)
{
    impl::for_each_impl(tuple, std::forward<F>(f), std::index_sequence_for<Args...> {});
}

} // namespace helper
} // namespace query_craft