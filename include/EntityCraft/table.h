#pragma once

#include "QueryCraft/conditiongroup.h"
#include "QueryCraft/table.h"
#include "ReflectionApi/entity.h"

namespace EntityCraft {

template<typename ClassType, typename... Columns>
class Table
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
            try {
                // При отношениях one to many может происходить дублирования колонок при
                // работе storage они автоматически удаляются и не играют никакой роли
                _table_info.add_column(column.mutable_column_info());
            } catch(...) {
            }
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
        reflection_api::helper::perform_if(
            _columns,
            [&](const auto& column) {
                return column.name() == property_name;
            },
            std::forward<Action>(action));
    }

    template<typename Action>
    void for_each(Action&& action)
    {
        reflection_api::helper::for_each(
            _columns,
            std::forward<Action>(action));
    }

    query_craft::table table_info() const
    {
        return _table_info;
    }

private:
    query_craft::table _table_info;
    std::tuple<Columns...> _columns = {};
};

template<typename ClassType, typename... Properties>
auto make_table(std::string scheme, std::string table_name, Properties&&... properties)
{
    return Table<ClassType, Properties...>(std::move(table_name), std::move(scheme), std::move(properties)...);
}

} // namespace EntityCraft