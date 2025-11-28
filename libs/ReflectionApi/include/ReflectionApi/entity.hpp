#pragma once

#include "helper/entityalgorithm.hpp"
#include "helper/tuplealgoritm.hpp"

#include <cstddef>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace reflection_api {

/**
 * Класс отвечающий за представление обобщенного доступа к любой структуре данных
 * @note Для использования необходимо, чтобы у объекта был конструктор по умолчанию
 * @tparam ClassType Тип объекта
 * @tparam Properties Список пропертей объекта
 */
template<typename ClassType, typename... Properties>
class entity
{
public:
    /**
     * @brief Создать объект сущности
     * @return Пустой объект типа ClassType
     */
    static constexpr ClassType empty_entity()
    {
        return ClassType();
    }

public:
    /**
     * @brief Конструктор сущности
     * @param properties Список пропертей
     */
    explicit entity(Properties... properties)
        : _properties(std::make_tuple<Properties...>(std::forward<Properties>(properties)...))
    {
    }

    entity(const entity& other) = default;
    entity(entity&& other) noexcept = default;

    ~entity() = default;

    entity& operator=(const entity& other) = default;
    entity& operator=(entity&& other) noexcept = default;

    /**
     * @brief Получить значение свойства из объекта
     * @tparam TargetType Тип значения свойства
     * @param obj Объект
     * @param target Значение свойства
     * @param property_name Имя свойства
     */
    template<typename TargetType>
    void get_property_value(const ClassType& obj, TargetType& target, const std::string& property_name) const
    {
        helper::get_value(
            _properties,
            obj,
            target,
            [&](const auto& column) {
                return column.name() == property_name;
            });
    }

    /**
     * @brief Установить значение свойства в объекте
     * @tparam TargetType Тип значения свойства
     * @param obj Объект
     * @param target Значение свойства
     * @param property_name Имя свойства
     */
    template<typename TargetType>
    void set_property_value(ClassType& obj, TargetType&& target, const std::string& property_name) const
    {
        helper::set_value(
            _properties,
            obj,
            std::forward<TargetType>(target),
            [&](const auto& column) {
                return column.name() == property_name;
            });
    }

    /**
     * @brief Получить имена всех свойств в объекте
     * @return Имена всех свойств
     */
    std::vector<std::string> get_property_names() const
    {
        std::vector<std::string> property_names;
        helper::for_each(_properties, [&](const auto& prop) {
            property_names.push_back(prop.name());
        });
        return property_names;
    }

    /**
     * @brief Выполнить действия над всеми проперти в объекте
     * @param action Действие которое необходимо выполнить для каждой проперти
     */
    template<typename Action>
    void for_each(Action&& action) const
    {
        helper::for_each(
            _properties,
            std::forward<Action>(action));
    }

    /**
     * @brief Получить количество свойств в entity
     * @return Количество свойств
     */
    constexpr std::size_t property_count() const noexcept
    {
        return std::tuple_size<decltype(_properties)>::value;
    }

    /**
     * @brief Проверить наличие свойства с заданным именем
     * @param property_name Название свойства
     * @return True если свойство существует, false иначе
     */
    bool has_property(const std::string& property_name) const noexcept
    {
        bool found = false;
        helper::for_each(_properties, [&](const auto& prop) {
            if(!found && prop.name() == property_name) {
                found = true;
            }
        });
        return found;
    }

private:
    /// Список проперти в сущности
    std::tuple<Properties...> _properties = {};
};

/**
 * @brief Создание объекта сущности
 * @tparam ClassType Тип объекта
 * @tparam Properties Список пропертей
 * @param properties Список пропертей
 * @return Объект сущности
 */
template<typename ClassType, typename... Properties>
constexpr auto make_entity(const Properties&... properties)
{
    return entity<ClassType, std::decay_t<Properties>...>(properties...);
}

} // namespace reflection_api
