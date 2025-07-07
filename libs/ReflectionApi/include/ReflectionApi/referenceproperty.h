#pragma once

#include "entity.h"
#include "helper/templates.h"
#include "property.h"

namespace reflection_api {

/**
 * Сущность отвечающая за представление ссылочной проперти в классе
 * @tparam ClassType Тип класса в котором находится проперти
 * @tparam PropertyType Тип проперти
 */
template<typename ClassType,
    typename PropertyType,
    typename Setter,
    typename Getter,
    typename... ReferenceProperties>
class reference_property final : public property<ClassType, PropertyType, Setter, Getter>
{
public:
    explicit reference_property(
        std::string name,
        const helper::Variable_t<ClassType, PropertyType>& variable,
        const entity<PropertyType, ReferenceProperties...>& entity)
        : property<ClassType, PropertyType, Setter, Getter>(name, variable)
        , _reference_entity(entity)
    {
    }

    explicit reference_property(
        std::string name,
        Setter setter,
        Getter getter,
        const entity<PropertyType, ReferenceProperties...>& entity)
        : property<ClassType, PropertyType, Setter, Getter>(name, setter, getter)
        , _reference_entity(entity)
    {
    }

    reference_property(const reference_property& other) = default;
    reference_property(reference_property&& other) noexcept = default;

    ~reference_property() override = default;

    reference_property& operator=(const reference_property& other) = default;
    reference_property& operator=(reference_property&& other) noexcept = default;

    /// Получить связанную сущность
    entity<PropertyType, ReferenceProperties...> reference_entity() const
    {
        return _reference_entity;
    }

private:
    /// Связанная сущность
    entity<PropertyType, ReferenceProperties...> _reference_entity;
};

template<typename ClassType,
    typename PropertyType,
    typename... ReferenceProperties>
auto make_reference_property(
    std::string name,
    helper::Variable_t<ClassType, PropertyType> variable,
    entity<PropertyType, ReferenceProperties...> reference_entity)
{
    return reference_property<ClassType,
        PropertyType,
        helper::Setter_t<ClassType, PropertyType>,
        helper::ConstGetter_t<ClassType, PropertyType>,
        ReferenceProperties...>(
        std::move(name),
        variable,
        std::move(reference_entity));
}

template<typename ClassType,
    typename PropertyType,
    typename... ReferenceProperties>
auto make_reference_property(
    std::string name,
    helper::Setter_t<ClassType, PropertyType> setter,
    helper::ConstGetter_t<ClassType, PropertyType> getter,
    entity<PropertyType, ReferenceProperties...> reference_entity)
{
    return reference_property<ClassType,
        PropertyType,
        helper::Setter_t<ClassType, PropertyType>,
        helper::ConstGetter_t<ClassType, PropertyType>,
        ReferenceProperties...>(
        std::move(name),
        setter,
        getter,
        std::move(reference_entity));
}

template<typename ClassType,
    typename PropertyType,
    typename... ReferenceProperties>
auto make_reference_property(
    std::string name,
    helper::Setter_t<ClassType, PropertyType> setter,
    helper::MutableGetter_t<ClassType, PropertyType> getter,
    entity<PropertyType, ReferenceProperties...> reference_entity)
{
    return reference_property<ClassType,
        PropertyType,
        helper::Setter_t<ClassType, PropertyType>,
        helper::MutableGetter_t<ClassType, PropertyType>,
        ReferenceProperties...>(
        std::move(name),
        setter,
        getter,
        std::move(reference_entity));
}

template<typename ClassType,
    typename PropertyType,
    typename... ReferenceProperties>
auto make_reference_property(
    std::string name,
    helper::Setter_t<ClassType, PropertyType> setter,
    helper::Getter_t<ClassType, PropertyType> getter,
    entity<PropertyType, ReferenceProperties...> reference_entity)
{
    return reference_property<ClassType,
        PropertyType,
        helper::Setter_t<ClassType, PropertyType>,
        helper::Getter_t<ClassType, PropertyType>,
        ReferenceProperties...>(
        std::move(name),
        setter,
        getter,
        std::move(reference_entity));
}

//-----------------------------Перегрузки для примитивов, где параметры в setter не по const &-----------------------------------

template<typename ClassType,
    typename PropertyType,
    typename... ReferenceProperties>
auto make_reference_property(
    std::string name,
    helper::BaseSetter_t<ClassType, PropertyType> setter,
    helper::ConstGetter_t<ClassType, PropertyType> getter,
    entity<PropertyType, ReferenceProperties...> reference_entity)
{
    return reference_property<ClassType,
        PropertyType,
        helper::BaseSetter_t<ClassType, PropertyType>,
        helper::ConstGetter_t<ClassType, PropertyType>,
        ReferenceProperties...>(
        std::move(name),
        setter,
        getter,
        std::move(reference_entity));
}

template<typename ClassType,
    typename PropertyType,
    typename... ReferenceProperties>
auto make_reference_property(
    std::string name,
    helper::BaseSetter_t<ClassType, PropertyType> setter,
    helper::MutableGetter_t<ClassType, PropertyType> getter,
    entity<PropertyType, ReferenceProperties...> reference_entity)
{
    return reference_property<ClassType,
        PropertyType,
        helper::BaseSetter_t<ClassType, PropertyType>,
        helper::MutableGetter_t<ClassType, PropertyType>,
        ReferenceProperties...>(
        std::move(name),
        setter,
        getter,
        std::move(reference_entity));
}

template<typename ClassType,
    typename PropertyType,
    typename... ReferenceProperties>
auto make_reference_property(
    std::string name,
    helper::BaseSetter_t<ClassType, PropertyType> setter,
    helper::Getter_t<ClassType, PropertyType> getter,
    entity<PropertyType, ReferenceProperties...> reference_entity)
{
    return reference_property<ClassType,
        PropertyType,
        helper::BaseSetter_t<ClassType, PropertyType>,
        helper::Getter_t<ClassType, PropertyType>,
        ReferenceProperties...>(
        std::move(name),
        setter,
        getter,
        std::move(reference_entity));
}

} // namespace reflection_api
