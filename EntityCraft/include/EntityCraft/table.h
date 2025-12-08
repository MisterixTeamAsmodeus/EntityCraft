#pragma once

#include "ReflectionApi/entity.hpp"

#include <set>
#include <type_traits>
#include <utility>

namespace entity_craft {

template<typename ClassType, typename... Columns>
class table : public reflection_api::entity<ClassType, Columns...>
{
public:
    template<typename T>
    static ClassType empty_entity_with_id(T&& id)
    {
        return ClassType(std::forward<T>(id));
    }

public:
    explicit table(std::string table_name, std::string scheme, Columns... properties)
        : reflection_api::entity<ClassType, Columns...>(std::move(properties)...)
        , _scheme(std::move(scheme))
        , _table_name(std::move(table_name))
    {
        for_each([this](const auto& column) {
            auto it = _columns_name.find(column.name());
            if(it == _columns_name.end()) {
                _columns_name.insert(column.name());
            } else {
                _duplicate_columns_name.insert(column.name());
            }
        });
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

    std::set<std::string> columns_name() const
    {
        return _columns_name;
    }

    std::set<std::string> duplicate_columns_name() const
    {
        return _duplicate_columns_name;
    }

private:
    std::string _scheme;
    std::string _table_name;

    std::set<std::string> _columns_name;
    std::set<std::string> _duplicate_columns_name;
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