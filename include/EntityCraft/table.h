#pragma once

#include "requestcallback.h"

#include <ReflectionApi/entity.h>

#include <QueryCraft/conditiongroup.h>
#include <QueryCraft/table.h>

namespace entity_craft {

template<typename ClassType, typename... Columns>
class table
{
public:
    static ClassType empty_entity()
    {
        return ClassType();
    }

public:
    explicit table(std::string table_name, std::string scheme, Columns... properties)
        : _table_info(std::move(table_name), std::move(scheme))
        , _columns(std::make_tuple<Columns...>(std::move(properties)...))
    {
        for_each([this](auto& column) {
            try {
                // При отношениях one to many/one to one inverted может происходить дублирования колонок при
                _table_info.add_column(column.mutable_column_info());
            } catch(const std::exception& /*e*/) {
                _duplicate_column.emplace_back(column.column_info());
            }
        });
    }

    table(const table& other) = default;
    table(table&& other) noexcept = default;

    ~table() = default;

    table& operator=(const table& other) = default;
    table& operator=(table&& other) noexcept = default;

    template<typename Action_>
    void visit_property(const std::string& property_name, Action_&& action)
    {
        reflection_api::helper::perform_if(
            _columns,
            [&](const auto& column) {
                return column.name() == property_name;
            },
            std::forward<Action_>(action));
    }

    template<typename Action_>
    void for_each(Action_&& action)
    {
        reflection_api::helper::for_each(
            _columns,
            std::forward<Action_>(action));
    }

    query_craft::table table_info() const
    {
        return _table_info;
    }

    std::vector<query_craft::column_info> duplicate_column() const
    {
        return _duplicate_column;
    }

    std::shared_ptr<IRequestCallback<ClassType>> reques_callback() const
    {
        return _reques_callback;
    }

    table& set_reques_callback(const std::shared_ptr<IRequestCallback<ClassType>>& reques_callback)
    {
        _reques_callback = reques_callback;
        return *this;
    }

    bool has_reques_callback() const
    {
        return _reques_callback != nullptr;
    }

private:
    query_craft::table _table_info;
    std::tuple<Columns...> _columns = {};
    std::vector<query_craft::column_info> _duplicate_column;

    std::shared_ptr<IRequestCallback<ClassType>> _reques_callback = nullptr;
};

template<typename ClassType, typename... Properties>
auto make_table(std::string scheme, std::string table_name, Properties&&... properties)
{
    return table<ClassType, Properties...>(
        std::move(table_name),
        std::move(scheme),
        std::move(properties)...);
}

} // namespace entity_craft