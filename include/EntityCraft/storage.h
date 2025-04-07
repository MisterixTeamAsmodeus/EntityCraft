#pragma once

#include "converterhelper.h"
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
        // TODO подумать что делать если транзакиция уже открыта

        _open_transaction = _database->open_transaction(type);
    }

    bool commit() const
    {
        // TODO добавить ошибку если транзакции нет

        if(_open_transaction != nullptr)
            return _open_transaction->commit();

        return false;
    }

    bool rollback() const
    {
        // TODO добавить ошибку если транзакции нет

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

        std::cout << sql << std::endl;

        const auto result = exec(sql);

        _condition_group = {};
        _limit = 0;
        _offset = 0;
        _sortColumns = {};

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

        _condition_group = condition_group;
        _limit = 1;
        _offset = 0;
        _sortColumns = {};

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
        QueryCraft::SqlTable sql_table(_dto.table_info());

        QueryCraft::SqlTable::Row row;
        _dto.for_each([&row, &value](auto& column) {
            auto property = column.property();

            const auto property_value = property.value(value);
            // TODO перенести получение конвертера из ReflectionApi::Property
            row.emplace_back(Converter<decltype(property_value)>().convertToString(property_value));
        });

        sql_table.addRow(row);
        const auto sql = sql_table.insertRowSql();
        std::cout << sql << std::endl;
        exec(sql);
    }

private:
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
            // TODO перенести получение конвертера из ReflectionApi::Property
            Converter<decltype(property_value)>().fillFromString(property_value, it->second);
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
