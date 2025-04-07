#pragma once

#include <sstream>
#include <string>

namespace EntityCraft {

template<typename T>
class Converter
{
public:
    virtual ~Converter() = default;

    virtual void fillFromString(T& value, const std::string& str) const
    {
        std::stringstream stream;
        stream << str;
        stream >> value;
    }

    virtual std::string convertToString(T& value) const
    {
        std::stringstream stream;
        stream << value;

        return stream.str();
    }
};

} // namespace EntityCraft