#pragma once

#include "column_settings.hpp"
#include "nullcheker.h"

#include <ReflectionApi/helper/templates.hpp>
#include <ReflectionApi/property.hpp>
#include <utility>

namespace entity_craft {

template<typename ClassType,
    typename PropertyType,
    typename Setter = reflection_api::helper::Setter_t<ClassType, PropertyType>,
    typename Getter = reflection_api::helper::ConstGetter_t<ClassType, PropertyType>>
class column : public reflection_api::property<ClassType, PropertyType, Setter, Getter>
{
public:
    explicit column(const std::string& name, reflection_api::helper::Variable_t<ClassType, PropertyType> variable, column_settings settings = column_settings::empty) noexcept;

    explicit column(const std::string& name, Setter setter, Getter getter, column_settings settings = column_settings::empty) noexcept;

    column(const column& other) = default;
    column(column&& other) noexcept = default;

    ~column() override = default;

    column& operator=(const column& other) = default;
    column& operator=(column&& other) noexcept = default;

    column_settings settings() const;

    std::shared_ptr<entity_craft::null_cheker<PropertyType>> null_checker() const;

    column set_null_checker(const std::shared_ptr<entity_craft::null_cheker<PropertyType>>& null_checker);

private:
    column_settings _settings;
    std::shared_ptr<entity_craft::null_cheker<PropertyType>> _null_checker = std::make_shared<entity_craft::null_cheker<PropertyType>>();
};

template<typename ClassType, typename PropertyType, typename Setter, typename Getter> column<ClassType, PropertyType, Setter, Getter>::column(const std::string& name, const reflection_api::helper::Variable_t<ClassType, PropertyType> variable, const column_settings settings) noexcept
    : reflection_api::property<ClassType, PropertyType, Setter, Getter>(std::move(name), variable)
    , _settings(settings)
{
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter> column<ClassType, PropertyType, Setter, Getter>::column(const std::string& name, Setter setter, Getter getter, const column_settings settings) noexcept
    : reflection_api::property<ClassType, PropertyType, Setter, Getter>(std::move(name), setter, getter)
    , _settings(settings)
{
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter> column_settings column<ClassType, PropertyType, Setter, Getter>::settings() const
{
    return _settings;
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter> std::shared_ptr<null_cheker<PropertyType>> column<ClassType, PropertyType, Setter, Getter>::null_checker() const
{
    return _null_checker;
}

template<typename ClassType, typename PropertyType, typename Setter, typename Getter> column<ClassType, PropertyType, Setter, Getter> column<ClassType, PropertyType, Setter, Getter>::set_null_checker(const std::shared_ptr<null_cheker<PropertyType>>& null_checker)
{
    _null_checker = null_checker;
    return *this;
}

/**
 * @brief Функция-помощник для создания column с автоматическим выводом типов
 * @param name Имя колонки
 * @param variable Указатель на член класса
 * @param settings Настройки колонки
 * @return Экземпляр column с выведенными типами
 */
template<typename ClassType, typename PropertyType>
column<ClassType, PropertyType> make_column(
    const std::string& name,
    reflection_api::helper::Variable_t<ClassType, PropertyType> variable,
    const column_settings settings = column_settings::empty) noexcept
{
    return column<ClassType, PropertyType>(name, variable, settings);
}

/**
 * @brief Функция-помощник для создания column с автоматическим выводом типов (перегрузка для setter/getter)
 * @param name Имя колонки
 * @param setter Setter для установки значения
 * @param getter Getter для получения значения
 * @param settings Настройки колонки
 * @return Экземпляр column с выведенными типами
 */
template<typename ClassType,
    typename PropertyType,
    typename Setter = reflection_api::helper::Setter_t<ClassType, PropertyType>,
    typename Getter = reflection_api::helper::ConstGetter_t<ClassType, PropertyType>>
column<ClassType, PropertyType, Setter, Getter> make_column(
    const std::string& name,
    Setter setter,
    Getter getter,
    const column_settings settings = column_settings::empty) noexcept
{
    return column<ClassType, PropertyType, Setter, Getter>(name, setter, getter, settings);
}

} // namespace entity_craft
