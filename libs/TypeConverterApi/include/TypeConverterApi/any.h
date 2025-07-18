#pragma once

#include "typeconverter.h"

#include <memory>
#include <string>
#include <typeindex>

namespace type_converter_api {
class any
{
private:
    class placeholder
    {
    public:
        virtual ~placeholder() = default;

    public:
        virtual std::string to_string() const = 0;
        virtual std::type_index type() const noexcept = 0;
        virtual placeholder* clone() const = 0;
    };

    template<typename ValueType>
    class holder final : public placeholder
    {
    public: // structors
        explicit holder(const ValueType& value)
            : held(value)
        {
        }

        explicit holder(ValueType&& value)
            : held(value)
        {
        }

    public:
        std::string to_string() const override
        {
            return type_converter_api::type_converter<ValueType>().convert_to_string(held);
        }

        std::type_index type() const noexcept override
        {
            return typeid(ValueType);
        }

        placeholder* clone() const override
        {
            return new holder(held);
        }

    public:
        ValueType held;

    private:
        holder& operator=(const holder&) = default;
        holder(const holder&) = default;
    };

public:
    any() = default;

    template<typename ValueType>
    explicit any(const ValueType& value)
        : content(new holder<ValueType>(value))
    {
    }

    template<typename ValueType>
    explicit any(ValueType&& value)
        : content(new holder<ValueType>(std::move(value)))
    {
    }

    any(const any& other) = default;
    any(any&& other) noexcept = default;
    any& operator=(const any& other) = default;
    any& operator=(any&& other) noexcept = default;

    bool empty() const
    {
        return content != nullptr;
    }

    template<typename ValueType>
    ValueType cast() const
    {
        if(can_cast<ValueType>()) {
            throw std::bad_cast();
        }

        return static_cast<holder<ValueType>*>(content)->held;
    }

    template<typename ValueType>
    std::shared_ptr<ValueType> unsafe_cast()
    {
        if(can_cast<ValueType>()) {
            return nullptr;
        }

        return std::make_shared<ValueType>(static_cast<holder<ValueType>*>(content)->held);
    }

    template<typename ValueType>
    bool can_cast() const noexcept
    {
        if(content == nullptr)
            return false;
        return type_info(ValueType()) == content->type();
    }

    std::string to_string() const;

    std::type_index type() const noexcept;

private:
    placeholder* content = nullptr;
};

} // namespace type_converter_api