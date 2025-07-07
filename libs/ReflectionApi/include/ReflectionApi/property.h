#pragma once

#include "helper/templates.h"

#include <TypeConverterApi/typeconverterapi.h>

#include <stdexcept>
#include <string>

namespace reflection_api {

/**
 * Сущность отвечающая за представление проперти в классе
 * @tparam ClassType Тип класса в котором находится проперти
 * @tparam PropertyType Тип проперти
 */
template<typename ClassType,
    typename PropertyType,
    typename Setter = helper::Setter_t<ClassType, PropertyType>,
    typename Getter = helper::ConstGetter_t<ClassType, PropertyType>>
class property
{
public:
    /// Создать объект с типом данных находящейся в сущности
    static PropertyType empty_property()
    {
        return PropertyType();
    }

public:
    /**
     * @brief Конструктор, создает экземпляр переменной, связанной с указателем на член-переменную.
     *
     * @param variable Указатель на член-переменную.
     * @param name Имя переменной.
     */
    explicit property(std::string name, const helper::Variable_t<ClassType, PropertyType> variable)
        : _name(std::move(name))
        , _variable(variable)
    {
    }

    /**
     * @brief Конструктор, создает экземпляр переменной, связанной с указателями на члены-функции.
     *
     * @param name Имя переменной.
     * @param setter Указатель на член-функцию, устанавливающую значение переменной.
     * @param getter Указатель на член-функцию, получающую значение переменной.
     */
    explicit property(std::string name, Setter setter, Getter getter)
        : _name(std::move(name))
        , _getter(std::move(getter))
        , _setter(std::move(setter))
    {
    }

    property(const property& other) = default;
    property(property&& other) noexcept = default;

    virtual ~property() = default;

    property& operator=(const property& other) = default;
    property& operator=(property&& other) noexcept = default;

    /**
     * @brief Устанавливает значение переменной.
     *
     * @param classValue Объект, в котором находится переменная.
     * @param data Новое значение переменной.
     */
    void set_value(ClassType& classValue, const PropertyType& data)
    {
        if(_variable == nullptr)
            (classValue.*_setter)(data);
        else
            classValue.*_variable = data;
    }

    /**
     * @brief Заглушка для работы рефлексии
     */
    template<typename Type>
    void set_value(ClassType& /*classValue*/, const Type& /*data*/)
    {
        throw std::invalid_argument("type is not valid");
    }

    /**
     * @brief Получает значение переменной.
     *
     * @param classValue Объект, в котором находится переменная.
     * @return Значение переменной.
     */
    PropertyType value(const ClassType& classValue) const
    {
        return _variable == nullptr ? (classValue.*_getter)() : classValue.*_variable;
    }

    /**
     * @brief Заглушка для работы рефлексии
     */
    template<typename Type>
    PropertyType value(const Type& /*classValue*/) const
    {
        throw std::invalid_argument("type is not valid");
    }

    /**
     * @brief Получает имя переменной.
     *
     * @return Имя переменной.
     */
    std::string name() const
    {
        return _name;
    }

    /// Получить объект конвертирующий нужный тип данных из сторки и обратно
    std::shared_ptr<type_converter_api::type_converter<PropertyType>> property_converter() const
    {
        return _property_converter;
    }

    /// Установить объект конвертирующий нужный тип данных из сторки и обратно
    property set_converter(const std::shared_ptr<type_converter_api::type_converter<PropertyType>>& converter)
    {
        _property_converter = converter;
        return *this;
    }

private:
    /**
     * @brief Строка, хранящая имя переменной.
     */
    std::string _name {};

    /**
     * @brief Указатель на член-переменную, связанный с этой переменной.
     * Если указатель равен nullptr, то переменная связана с указателями на члены-функции.
     */
    helper::Variable_t<ClassType, PropertyType> _variable = nullptr;

    /**
     * @brief Указатель на член-функцию, получающий значение переменной.
     * Если указатель равен nullptr, то значение переменной получается через член-переменную.
     */
    Getter _getter = nullptr;

    /**
     * @brief Указатель на член-функцию, устанавливающий значение переменной.
     * Если указатель равен nullptr, то значение переменной устанавливается через член-переменную.
     */
    Setter _setter = nullptr;

    /// Объект конвертирующий нужный тип данных из сторки и обратно
    std::shared_ptr<type_converter_api::type_converter<PropertyType>> _property_converter = std::make_shared<type_converter_api::type_converter<PropertyType>>();
};

template<typename ClassType, typename PropertyType>
auto make_property(
    std::string name,
    helper::Variable_t<ClassType, PropertyType> variable)
{
    return property<ClassType, PropertyType>(
        std::move(name),
        variable);
}

template<typename ClassType, typename PropertyType>
auto make_property(
    std::string name,
    helper::Setter_t<ClassType, PropertyType> setter,
    helper::ConstGetter_t<ClassType, PropertyType> getter)
{
    return property<ClassType,
        PropertyType,
        helper::Setter_t<ClassType, PropertyType>,
        helper::ConstGetter_t<ClassType, PropertyType>>(
        std::move(name),
        setter,
        getter);
}

template<typename ClassType, typename PropertyType>
auto make_property(
    std::string name,
    helper::Setter_t<ClassType, PropertyType> setter,
    helper::MutableGetter_t<ClassType, PropertyType> getter)
{
    return property<ClassType,
        PropertyType,
        helper::Setter_t<ClassType, PropertyType>,
        helper::MutableGetter_t<ClassType, PropertyType>>(
        std::move(name),
        setter,
        getter);
}

template<typename ClassType, typename PropertyType>
auto make_property(
    std::string name,
    helper::Setter_t<ClassType, PropertyType> setter,
    helper::Getter_t<ClassType, PropertyType> getter)
{
    return property<ClassType,
        PropertyType,
        helper::Setter_t<ClassType, PropertyType>,
        helper::Getter_t<ClassType, PropertyType>>(
        std::move(name),
        setter,
        getter);
}

//-----------------------------Перегрузки для примитивов, где параметры в setter не по const &-----------------------------------

template<typename ClassType, typename PropertyType>
auto make_property(
    std::string name,
    helper::BaseSetter_t<ClassType, PropertyType> setter,
    helper::ConstGetter_t<ClassType, PropertyType> getter)
{
    return property<ClassType,
        PropertyType,
        helper::BaseSetter_t<ClassType, PropertyType>,
        helper::ConstGetter_t<ClassType, PropertyType>>(
        std::move(name),
        setter,
        getter);
}

template<typename ClassType, typename PropertyType>
auto make_property(
    std::string name,
    helper::BaseSetter_t<ClassType, PropertyType> setter,
    helper::MutableGetter_t<ClassType, PropertyType> getter)
{
    return property<ClassType,
        PropertyType,
        helper::BaseSetter_t<ClassType, PropertyType>,
        helper::MutableGetter_t<ClassType, PropertyType>>(
        std::move(name),
        setter,
        getter);
}

template<typename ClassType, typename PropertyType>
auto make_property(
    std::string name,
    helper::BaseSetter_t<ClassType, PropertyType> setter,
    helper::Getter_t<ClassType, PropertyType> getter)
{
    return property<ClassType,
        PropertyType,
        helper::BaseSetter_t<ClassType, PropertyType>,
        helper::Getter_t<ClassType, PropertyType>>(
        std::move(name),
        setter,
        getter);
}

} // namespace reflection_api