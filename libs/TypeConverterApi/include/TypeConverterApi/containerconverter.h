#pragma once

#include "sfinae.h"

#include <sstream>
#include <typeinfo>
#include <vector>

namespace type_converter_api {
namespace impl {

#ifdef USE_TYPE_QT
template<typename TargetContainer, typename Type, typename CurrentContainer = std::vector<Type>,
    std::enable_if_t<sfinae::has_left_shift_container_operator_v<TargetContainer, Type>, bool> = true>
void convert_to_target(TargetContainer& relation_property, const CurrentContainer& result, int)
{
    for(const auto& value : result) {
        relation_property << value;
    }
}

#endif

template<typename TargetContainer, typename Type, typename CurrentContainer = std::vector<Type>,
    std::enable_if_t<sfinae::has_emplace_back_v<TargetContainer, Type>, bool> = true>
void convert_to_target(TargetContainer& relation_property, const CurrentContainer& result, int)
{
    for(const auto& value : result) {
        relation_property.emplace_back(value);
    }
}

template<typename TargetContainer, typename Type, typename CurrentContainer = std::vector<Type>>
void convert_to_target(TargetContainer&, const CurrentContainer&, ...)
{
    std::stringstream message;
    message << "convert_to_target not implemented from " << typeid(TargetContainer).name() << " to " << typeid(CurrentContainer).name();

    throw std::runtime_error(message.str());
}
} // namespace impl

template<typename TargetContainer, typename Type = typename TargetContainer::value_type>
class container_converter
{
public:
    template<typename CurrentContainer = std::vector<Type>>
    void convert_to_target(TargetContainer& relation_property, const CurrentContainer& result)
    {
        impl::convert_to_target<TargetContainer, Type, CurrentContainer>(relation_property, result, 0);
    }
};

} // namespace type_converter_api
