#pragma once
#include <type_traits>

namespace EntityCraft {
namespace SFINAE {
// Структура для проверки наличия оператора emplace_back
template<typename Container, typename T, typename = void>
struct has_emplace_back : std::false_type
{
};

template<typename Container, typename T>
struct has_emplace_back<Container, T, std::void_t<decltype((std::declval<Container&>().emplace_back(std::declval<T>())))>>
    : std::true_type
{
};

template<typename Container, typename T>
constexpr bool has_emplace_back_v = has_emplace_back<Container, T>::value;

} // namespace SFINAE
} // namespace EntityCraft