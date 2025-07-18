#include "TypeConverterApi/any.h"

namespace type_converter_api {
std::string any::to_string() const
{
    if(content == nullptr) {
        throw std::invalid_argument("content is null");
    }

    return content->to_string();
}

std::type_index any::type() const noexcept
{
    if(content == nullptr) {
        return typeid(void);
    }
    return content->type();
}
} // namespace type_converter_api