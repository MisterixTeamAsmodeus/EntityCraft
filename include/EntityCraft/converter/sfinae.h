#pragma once
#include <type_traits>

namespace EntityCraft {
namespace SFINAE {
// Структура для проверки наличия оператора emplace_back
template<typename Container, typename T, typename = void>
struct has_push_back : std::false_type
{
};

template<typename Container, typename T>
struct has_push_back<Container, T, std::void_t<decltype((std::declval<Container&>().push_back(std::declval<T>())))>>
    : std::true_type
{
};

template<typename Container, typename T>
constexpr bool has_push_back_v = has_push_back<Container, T>::value;

} // namespace SFINAE
} // namespace EntityCraft