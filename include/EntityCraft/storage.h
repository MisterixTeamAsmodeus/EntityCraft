#pragma once

#include "QueryCraft/sqltable.h"
#include "table.h"

#include <DatabaseAdapter/databaseadapter.h>

#include <iostream>
#include <memory>

namespace EntityCraft {

template<typename ClassType, typename... Columns>
class Storage
{
public:
    Storage(std::shared_ptr<DatabaseAdapter::IDataBaseDriver> database, Table<ClassType, Columns...> dto)
        : _database(std::move(database))
        , _dto(std::move(dto))
    {
    }

    ~Storage()
    {
        if(_open_transaction != nullptr)
            _open_transaction->commit();

        _database->disconnect();
    }

    auto condition_group(const QueryCraft::ConditionGroup& condition_group)
    {
        _condition_group = condition_group;
        return *this;
    }

    auto sort_columns(const std::vector<QueryCraft::SortColumn>& sort_columns)
    {
        _sortColumns = sort_columns;
        return *this;
    }

    auto limit(const size_t limit)
    {
        _limit = limit;
        return *this;
    }

    auto offset(const size_t offset)
    {
        _offset = offset;
        return *this;
    }

    void transaction(const int type = -1)
    {
        _open_transaction = _database->open_transaction(type);
    }

    bool commit() const
    {
        if(_open_transaction != nullptr)
            return _open_transaction->commit();

        return false;
    }

    bool rollback() const
    {
        if(_open_transaction != nullptr)
            return _open_transaction->rollback();

        return false;
    }

    std::vector<ClassType> select()
    {
        const QueryCraft::SqlTable sql_table(_dto.table_info());

        const auto sql = sql_table.selectRowsSql(join_columns(),
            _condition_group,
            _sortColumns,
            _limit,
            _offset);

        clear_select_settings();

        std::cout << std::endl;
        std::cout << sql << std::endl;
        std::cout << std::endl;

        const auto result = exec(sql);

        if(result.empty()) {
            return {};
        }

        std::vector<ClassType> res;
        for(const auto& row : result.data()) {
            res.emplace_back(fill_class_by_sql(row));
        }

        return res;
    }

    std::shared_ptr<ClassType> get(const QueryCraft::ConditionGroup& condition_group)
    {
        const QueryCraft::SqlTable sql_table(_dto.table_info());

        clear_select_settings();
        _condition_group = condition_group;
        _limit = 1;

        const auto res = select();
        if(res.empty()) {
            return nullptr;
        }

        return std::make_shared<ClassType>(res.front());
    }

    template<typename IdType>
    std::shared_ptr<ClassType> get_by_id(const IdType& id)
    {
        return get(primary_key_column() == id);
    }

    void insert(const ClassType& value)
    {
        std::vector<ClassType> data = { value };

        insert(data.begin(), data.end());
    }

    template<typename Begin, typename End>
    void insert(const Begin& begin, const End& end)
    {
        QueryCraft::SqlTable sql_table(_dto.table_info());

        std::for_each(begin, end, [this, &sql_table](const auto& value) {
            QueryCraft::SqlTable::Row row;
            _dto.for_each([&row, &value](auto& column) {
                auto property = column.property();

                const auto property_value = property.value(value);
                row.emplace_back(property.converter()->convertToString(property_value));
            });

            sql_table.addRow(row);
        });

        const auto sql = sql_table.insertRowSql();
        std::cout << std::endl;
        std::cout << sql << std::endl;
        std::cout << std::endl;
        exec(sql);
    }

    void update(const ClassType& value)
    {
        QueryCraft::SqlTable sql_table(_dto.table_info());

        QueryCraft::ConditionGroup condition_for_update;
        std::vector<QueryCraft::ColumnInfo> columns_for_update;
        QueryCraft::SqlTable::Row row;

        _dto.for_each([&row, &value, &condition_for_update, &columns_for_update](auto& column) {
            const auto column_info = column.column_info();

            auto property = column.property();

            const auto string_property_value = property.converter()
                                                   ->convertToString(property.value(value));

            if(column_info.hasSettings(QueryCraft::ColumnSettings::PRIMARY_KEY)) {
                condition_for_update = column_info == string_property_value;
            } else {
                columns_for_update.emplace_back(column_info);
                row.emplace_back(string_property_value);
            }
        });

        sql_table.addRow(row);

        const auto sql = sql_table.updateRowSql(condition_for_update, columns_for_update);
        std::cout << std::endl;
        std::cout << sql << std::endl;
        std::cout << std::endl;
        exec(sql);
    }

    template<typename Begin, typename End>
    void update(const Begin& begin, const End& end)
    {
        std::for_each(begin, end, [](const auto& value) {
            update(value);
        });
    }

    void remove(const ClassType& value)
    {
        std::vector<ClassType> data = { value };

        remove(data.begin(), data.end());
    }

    template<typename Begin, typename End>
    void remove(const Begin& begin, const End& end)
    {
        QueryCraft::SqlTable sql_table(_dto.table_info());

        QueryCraft::ConditionGroup condition_for_remove;

        std::for_each(begin, end, [this, &condition_for_remove](const auto& value) {
            _dto.for_each([&value, &condition_for_remove](auto& column) {
                auto column_info = column.column_info();

                auto property = column.property();

                const auto string_property_value = property.converter()
                                                       ->convertToString(property.value(value));

                if(column_info.hasSettings(QueryCraft::ColumnSettings::PRIMARY_KEY)) {
                    condition_for_remove = column_info == string_property_value;
                }
            });
        });

        const auto sql = sql_table.removeRowSql(condition_for_remove);
        std::cout << std::endl;
        std::cout << sql << std::endl;
        std::cout << std::endl;
        exec(sql);
    }

    void removeAll()
    {
        std::vector<ClassType> data;
        remove(data.begin(), data.end());
    }

private:
    void clear_select_settings()
    {
        _condition_group = {};
        _limit = 0;
        _offset = 0;
        _sortColumns = {};
    }

    QueryCraft::ColumnInfo primary_key_column()
    {
        QueryCraft::ColumnInfo primary_key;

        _dto.for_each([&primary_key](const auto& column) {
            auto column_info = column.column_info();
            if(column_info.hasSettings(QueryCraft::ColumnSettings::PRIMARY_KEY))
                primary_key = column_info;
        });

        return primary_key;
    }

    ClassType fill_class_by_sql(const DatabaseAdapter::Models::QueryResult::ResultRow& query_result)
    {
        auto entity = _dto.empty_entity();

        _dto.for_each([&entity, &query_result](auto& column) {
            const auto column_info = column.column_info();

            auto property = column.property();

            const auto it = query_result.find(column_info.alias());
            if(it == query_result.end())
                return;

            auto property_value = property.empty_property();
            property.converter()->fillFromString(property_value, it->second);
            property.set_value(entity, property_value);
        });

        return entity;
    }

    std::vector<QueryCraft::JoinColumn> join_columns() const
    {
        // TODO добавить обработку когда появятся отношения one to many и тд.
        return {};
    }

    DatabaseAdapter::Models::QueryResult exec(const std::string& sql) const
    {
        return _open_transaction != nullptr ? _open_transaction->exec(sql) : _database->exec(sql);
    }

private:
    std::shared_ptr<DatabaseAdapter::IDataBaseDriver> _database;
    std::shared_ptr<DatabaseAdapter::ITransaction> _open_transaction;
    Table<ClassType, Columns...> _dto;

    // Настройки для select
    QueryCraft::ConditionGroup _condition_group;
    std::vector<QueryCraft::SortColumn> _sortColumns;
    size_t _limit = 0;
    size_t _offset = 0;
};

template<typename ClassType, typename... Columns>
auto make_storage(std::shared_ptr<DatabaseAdapter::IDataBaseDriver> database, Table<ClassType, Columns...> dto)
{
    return Storage<ClassType, Columns...>(database, dto);
}

} // namespace EntityCraft
