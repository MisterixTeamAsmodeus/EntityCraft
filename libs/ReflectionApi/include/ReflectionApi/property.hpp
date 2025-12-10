#pragma once

#include "helper/templates.hpp"

#include <TypeConverterApi/typeconverterapi.hpp>

#include <string>
#include <type_traits>

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
    /**
     * @brief Тип класса в котором находится проперти
     */
    using class_type = ClassType;
    /**
     * @brief Тип проперти
     */
    using property_type = PropertyType;

    /**
     * @brief Создать объект с типом данных находящейся в сущности
     * @return Объект с типом данных находящейся в сущности
     */
    static constexpr PropertyType empty_property();

public:
    /**
     * @brief Конструктор, создает экземпляр переменной, связанной с указателем на член-переменную.
     *
     * @param variable Указатель на член-переменную.
     * @param name Имя переменной.
     */
    explicit property(std::string name, helper::Variable_t<ClassType, PropertyType> variable) noexcept;

    /**
     * @brief Конструктор, создает экземпляр переменной, связанной с указателями на члены-функции.
     *
     * @param name Имя переменной.
     * @param setter Указатель на член-функцию, устанавливающую значение переменной.
     * @param getter Указатель на член-функцию, получающую значение переменной.
     */
    explicit property(std::string name, Setter setter, Getter getter) noexcept;

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
    void set_value(ClassType& classValue, const PropertyType& data) const;

    /**
     * @brief Получает значение переменной.
     *
     * @param classValue Объект, в котором находится переменная.
     * @return Значение переменной.
     */
    PropertyType value(const ClassType& classValue) const;

    /**
     * @brief Получает имя переменной.
     *
     * @return Имя переменной.
     */
    std::string name() const noexcept;

    /**
     * @brief Установить объект конвертирующий нужный тип данных из строки и обратно.
     *
     * @param converter Объект конвертирующий нужный тип данных из строки и обратно.
     * @return Ссылка на текущий объект для цепочки вызовов.
     */
    property set_converter(std::shared_ptr<type_converter_api::type_converter<property_type>> converter);

    /**
     * @brief Конвертирует значение переменной в строку.
     *
     * @param classValue Объект, в котором находится переменная.
     * @return Строковое представление значения переменной.
     */
    std::string to_string(const ClassType& classValue) const;

    /**
     * @brief Заполняет значение переменной из строки.
     *
     * @param classValue Объект, в котором находится переменная.
     * @param str Строковое представление значения переменной.
     */
    void from_string(ClassType& classValue, const std::string& str) const;

private:
    /**
     * @brief Строка, хранящая имя переменной.
     */
    std::string _name {};

    /**
     * @brief Указатель на член-переменную, связанный с этой переменной.
     * Если указатель равен nullptr, то переменная связана с указателями на члены-функции.
     */
    helper::Variable_t<class_type, property_type> _variable = nullptr;

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
    std::shared_ptr<type_converter_api::type_converter<property_type>> _property_converter;
};

template<typename ClassType, typename PropertyType, typename Setter, typename Getter> property<ClassType, PropertyType, Setter, Getter>::property(std::string name, const helper::Variable_t<ClassType, PropertyType> variable) noexcept
    : _name(std::move(name))
    , _variable(variable)
{
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter> property<ClassType, PropertyType, Setter, Getter>::property(std::string name, Setter setter, Getter getter) noexcept
    : _name(std::move(name))
    , _getter(std::move(getter))
    , _setter(std::move(setter))
{
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter> void property<ClassType, PropertyType, Setter, Getter>::set_value(ClassType& classValue, const PropertyType& data) const
{
    if(_variable == nullptr)
        (classValue.*_setter)(data);
    else
        classValue.*_variable = data;
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter> PropertyType property<ClassType, PropertyType, Setter, Getter>::value(const ClassType& classValue) const
{
    return _variable == nullptr ? (classValue.*_getter)() : classValue.*_variable;
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter> std::string property<ClassType, PropertyType, Setter, Getter>::name() const noexcept
{
    return _name;
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter> property<ClassType, PropertyType, Setter, Getter> property<ClassType, PropertyType, Setter, Getter>::set_converter(std::shared_ptr<type_converter_api::type_converter<property_type>> converter)
{
    _property_converter = std::move(converter);
    return *this;
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter> std::string property<ClassType, PropertyType, Setter, Getter>::to_string(const ClassType& classValue) const
{
    return _property_converter == nullptr ? type_converter_api::to_string(value(classValue)) : _property_converter->convert_to_string(value(classValue));
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter> void property<ClassType, PropertyType, Setter, Getter>::from_string(ClassType& classValue, const std::string& str) const
{
    if(_property_converter == nullptr) {
        set_value(classValue, type_converter_api::from_string<property_type>(str));
    } else {
        auto temp = empty_property();
        _property_converter->fill_from_string(temp, str);
        set_value(classValue, temp);
    }
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter> constexpr PropertyType
property<ClassType, PropertyType, Setter, Getter>::empty_property()
{
    return PropertyType();
}

/**
 * @brief Создание объекта проперти
 * @tparam ClassType Тип класса в котором находится проперти
 * @tparam PropertyType Тип проперти
 * @param name Имя переменной
 * @param variable Указатель на член-переменную
 * @return Объект проперти
 */
template<typename ClassType, typename PropertyType>
auto make_property(
    std::string name,
    helper::Variable_t<ClassType, PropertyType> variable)
{
    return property<ClassType, PropertyType>(
        std::move(name),
        variable);
}

/**
 * @brief Создание объекта проперти
 * @tparam ClassType Тип класса в котором находится проперти
 * @tparam PropertyType Тип проперти
 * @param name Имя переменной
 * @param setter Указатель на член-функцию, устанавливающую значение переменной
 * @param getter Указатель на член-функцию, получающую значение переменной
 * @return Объект проперти
 */
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

/**
 * @brief Создание объекта проперти
 * @tparam ClassType Тип класса в котором находится проперти
 * @tparam PropertyType Тип проперти
 * @param name Имя переменной
 * @param setter Указатель на член-функцию, устанавливающую значение переменной
 * @param getter Указатель на член-функцию, получающую значение переменной
 * @return Объект проперти
 */
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

/**
 * @brief Создание объекта проперти
 * @tparam ClassType Тип класса в котором находится проперти
 * @tparam PropertyType Тип проперти
 * @param name Имя переменной
 * @param setter Указатель на член-функцию, устанавливающую значение переменной
 * @param getter Указатель на член-функцию, получающую значение переменной
 * @return Объект проперти
 */
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

/**
 * @brief Создание объекта проперти
 * @tparam ClassType Тип класса в котором находится проперти
 * @tparam PropertyType Тип проперти
 * @param name Имя переменной
 * @param setter Указатель на член-функцию, устанавливающую значение переменной
 * @param getter Указатель на член-функцию, получающую значение переменной
 * @return Объект проперти
 */
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

/**
 * @brief Создание объекта проперти
 * @tparam ClassType Тип класса в котором находится проперти
 * @tparam PropertyType Тип проперти
 * @param name Имя переменной
 * @param setter Указатель на член-функцию, устанавливающую значение переменной
 * @param getter Указатель на член-функцию, получающую значение переменной
 * @return Объект проперти
 */
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

/**
 * @brief Создание объекта проперти
 * @tparam ClassType Тип класса в котором находится проперти
 * @tparam PropertyType Тип проперти
 * @param name Имя переменной
 * @param setter Указатель на член-функцию, устанавливающую значение переменной
 * @param getter Указатель на член-функцию, получающую значение переменной
 * @return Объект проперти
 */
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
