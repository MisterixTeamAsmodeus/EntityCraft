#pragma once

namespace type_converter_api {
namespace sfinae {
template<typename... Ts>
struct make_void
{
    typedef void type;
};

template<typename... Ts>
using void_t = typename make_void<Ts...>::type;
} // namespace sfinae
} // namespace type_converter_api