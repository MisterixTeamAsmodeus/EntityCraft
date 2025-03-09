#pragma once

#include "entitycraft_global.h"
#include "ReflectionApi/entity.h"

namespace EntityCraft {

template<typename ClassType, typename... Columns>
class ENTITYCRAFT_EXPORT Table
{
public:
    static ClassType empty_entity()
    {
        return ClassType();
    }

public:
    explicit Table(std::string scheme, std::string table_name, Columns... properties)
        : _scheme(std::move(scheme))
        , _table_name(std::move(table_name))
        , _columns(std::make_tuple<Columns...>(std::move(properties)...))
    {
    }

    Table(const Table& other)
        : _scheme(other._scheme)
        , _table_name(other._table_name)
        , _columns(other._columns)
    {
    }

    Table(Table&& other) noexcept
        : _scheme(std::move(other._scheme))
        , _table_name(std::move(other._table_name))
        , _columns(std::move(other._columns))
    {
    }

    Table& operator=(const Table& other)
    {
        if(this == &other)
            return *this;
        _scheme = other._scheme;
        _table_name = other._table_name;
        _columns = other._columns;
        return *this;
    }

    Table& operator=(Table&& other) noexcept
    {
        if(this == &other)
            return *this;

        _scheme = std::move(other._scheme);
        _table_name = std::move(other._table_name);
        _columns = std::move(other._columns);

        return *this;
    }

    template<typename Action>
    void visit_property(const std::string& property_name, Action&& action)
    {
        ReflectionApi::Helper::perform_if(
            _columns,
            [&](const auto& column) {
                return column.name() == property_name;
            },
            std::forward<Action>(action));
    }

    template<typename Action>
    void for_each(Action&& action)
    {
        ReflectionApi::Helper::for_each(
            _columns,
            std::forward<Action>(action));
    }

    std::string scheme() const
    {
        return _scheme;
    }

    std::string table_name() const
    {
        return _table_name;
    }

private:
    std::string _scheme;
    std::string _table_name;
    std::tuple<Columns...> _columns = {};
};

template<typename ClassType, typename... Properties>
auto ENTITYCRAFT_EXPORT make_table(std::string scheme, std::string table_name, Properties&&... properties)
{
    return Table<ClassType, Properties...>(std::move(scheme), std::move(table_name), std::move(properties)...);
}

}