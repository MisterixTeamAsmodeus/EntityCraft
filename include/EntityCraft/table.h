#pragma once

#include "entitycraft_global.h"
#include "QueryCraft/conditiongroup.h"
#include "QueryCraft/table.h"
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
    explicit Table(std::string table_name, std::string scheme, Columns... properties)
        : _table_info(std::move(table_name), std::move(scheme))
        , _columns(std::make_tuple<Columns...>(std::move(properties)...))
    {
        for_each([this](auto& column) {
            _table_info.addColumn(column.mutable_column_info());
        });
    }

    Table(const Table& other)
        : _table_info(other._table_info)
        , _columns(other._columns)
    {
    }

    Table(Table&& other) noexcept
        : _table_info(std::move(other._table_info))
        , _columns(std::move(other._columns))
    {
    }

    Table& operator=(const Table& other)
    {
        if(this == &other)
            return *this;
        _table_info = other._table_info;
        _columns = other._columns;
        return *this;
    }

    Table& operator=(Table&& other) noexcept
    {
        if(this == &other)
            return *this;

        _table_info = std::move(other._table_info);
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

    QueryCraft::Table table_info() const
    {
        return _table_info;
    }

private:
    QueryCraft::Table _table_info;
    std::tuple<Columns...> _columns = {};
};

template<typename ClassType, typename... Properties>
ENTITYCRAFT_EXPORT auto make_table(std::string table_name, std::string scheme, Properties&&... properties)
{
    return Table<ClassType, Properties...>(std::move(scheme), std::move(table_name), std::move(properties)...);
}

}