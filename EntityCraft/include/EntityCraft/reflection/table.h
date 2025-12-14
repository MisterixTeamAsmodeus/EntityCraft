#pragma once

#include "EntityCraft/visitor/columnvisitor.hpp"
#include "EntityCraft/utils/table_utils.hpp"
#include "ReflectionApi/entity.hpp"

#include <type_traits>
#include <utility>

namespace entity_craft {

template<typename ClassType, typename... Columns>
class table : public reflection_api::entity<ClassType, Columns...>
{
public:
    using class_type = ClassType;
public:
    explicit table(std::string table_name, std::string scheme, Columns... properties)
        : reflection_api::entity<ClassType, Columns...>(std::move(properties)...)
        , _scheme(std::move(scheme))
        , _table_name(std::move(table_name))
    {

        for_each(visitor::make_column_visitor([this](const auto& column) {
            _columns_name.push_back(column.name());

            const auto settings = column.settings();

            if(has_setting(settings, column_settings::primary_key) && _primary_key_column_name.empty()) {
                _primary_key_column_name = column.name();
            }

            if(has_setting(settings, column_settings::auto_increment)) {
                _auto_increment_column_names.push_back(column.name());
                _has_auto_inc = true;
            } else {
                _column_names_without_auto_increment.push_back(column.name());
            }
        }));
    }

    table(const table& other) = default;
    table(table&& other) noexcept = default;

    ~table() = default;

    table& operator=(const table& other) = default;
    table& operator=(table&& other) noexcept = default;

    std::string scheme() const
    {
        return _scheme;
    }

    std::string table_name() const
    {
        return _table_name;
    }

    std::string column_alias(const std::string& column_name) const
    {
        return (_scheme.empty() ? "" : _scheme + "_") + _table_name + "_" + column_name;
    }

    std::vector<std::string> columns_name() const
    {
        return _columns_name;
    }

    std::string primary_key_column_name() const
    {
        return _primary_key_column_name;
    }

    template<typename TargetType>
    void append_property_value(ClassType& obj, TargetType&& item, const std::string& property_name) const
    {
        helper::append_value(
            reflection_api::entity<ClassType, Columns...>::_properties,
            obj,
            std::forward<TargetType>(item),
            [&](const auto& column) {
                return column.name() == property_name;
            });
    }

    std::string primary_key_column_value(const ClassType& entity)
    {
        std::string value;
        for_each([this, &entity, &value](const auto& column) {
            if(column.name() == _primary_key_column_name) {
                value = column.to_string(entity);
            }
        });
        return value;
    }

    bool is_primary_key_column_value_null(const ClassType& entity)
    {
        bool is_null = true;
        for_each([this, &entity, &is_null](const auto& column) {
            if(column.name() == _primary_key_column_name) {
                is_null = column.is_null_value(entity);
            }
        });
        return is_null;
    }

    std::vector<std::string> column_names_without_auto_increment()
    {
        return _column_names_without_auto_increment;
    }

    std::vector<std::string> auto_increment_column_names()
    {
        return _auto_increment_column_names;
    }

    bool has_auto_increment() const
    {
        return _has_auto_inc;
    }

private:
    std::string _scheme;
    std::string _table_name;

    std::vector<std::string> _columns_name;

    std::vector<std::string> _column_names_without_auto_increment;
    std::vector<std::string> _auto_increment_column_names;

    std::string _primary_key_column_name;
    bool _has_auto_inc = false;
};

/**
 * @brief Функция-помощник для создания table с автоматическим выводом типов колонок
 * @param table_name Имя таблицы
 * @param scheme Схема таблицы
 * @param properties Колонки таблицы
 * @return Экземпляр table с выведенными типами
 */
template<typename ClassType, typename... Columns>
table<ClassType, Columns...> make_table(
    const std::string& table_name,
    const std::string& scheme,
    Columns... properties)
{
    return table<ClassType, std::decay_t<Columns>...>(table_name, scheme, std::move(properties)...);
}

} // namespace entity_craft