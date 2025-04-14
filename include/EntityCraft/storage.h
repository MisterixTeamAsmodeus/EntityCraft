#pragma once

#include "QueryCraft/sqltable.h"
#include "table.h"
#include "visitor/anycolumnvisitor.h"
#include "visitor/columnvisitor.h"
#include "visitor/referencecolumnvisitor.h"

#include <DatabaseAdapter/databaseadapter.h>

#include <iostream>
#include <memory>

namespace EntityCraft {

#define storage_type(dto) decltype(make_storage(nullptr, dto));

template<typename ClassType, typename... Columns>
class Storage
{
public:
    Storage(const std::shared_ptr<DatabaseAdapter::IDataBaseDriver>& database, Table<ClassType, Columns...> dto)
        : _database(database)
        , _dto(std::move(dto))
    {
        if(!_database->is_open())
            _database->connect();
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

    auto without_relation_entity(const bool without_relation_entity = true)
    {
        _without_relation_entity = without_relation_entity;
        return *this;
    }

    void transaction(const int type = -1)
    {
        _open_transaction = _database->open_transaction(type);
    }

    void set_transaction(const std::shared_ptr<DatabaseAdapter::ITransaction>& transaction)
    {
        _open_transaction = transaction;
    }

    std::shared_ptr<DatabaseAdapter::IDataBaseDriver> database() const
    {
        return _database;
    }

    bool commit()
    {
        if(_open_transaction != nullptr) {
            const auto res = _open_transaction->commit();
            _open_transaction = nullptr;
            return res;
        }

        return false;
    }

    bool rollback()
    {
        if(_open_transaction != nullptr) {
            const auto res = _open_transaction->rollback();
            _open_transaction = nullptr;
            return res;
        }

        return false;
    }

    std::vector<ClassType> select()
    {
        const QueryCraft::SqlTable sql_table(_dto.table_info());

        std::vector<QueryCraft::ColumnInfo> columns = sql_table.columns();

        _dto.for_each(Visitor::make_reference_column_visitor([&columns](auto& reference_column) {
            columns.erase(std::remove(columns.begin(), columns.end(), reference_column.column_info()));
        }));

        if(!_without_relation_entity) {
            append_join_columns(columns, _dto);
        }

        const auto sql = sql_table.selectRowsSql(
            _without_relation_entity ? std::vector<QueryCraft::JoinColumn> {} : join_columns(_dto),
            _condition_group,
            _sortColumns,
            _limit,
            _offset,
            columns);

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
            res.emplace_back(fill_class_by_sql(_dto, row, _database, _open_transaction));
        }

        return res;
    }

    size_t count()
    {
        const auto temp = _condition_group;
        clear_select_settings();
        _condition_group = temp;

        return select().size();
    }

    std::shared_ptr<ClassType> get()
    {
        const auto t = _condition_group;
        const auto t1 = _without_relation_entity;

        clear_select_settings();
        _condition_group = t;
        _without_relation_entity = t1;
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
        return get(primary_key_column(_dto) == id);
    }

    bool find(const ClassType& value)
    {
        clear_select_settings();

        _dto.for_each([this, &value](const auto& column) {
            auto column_info = column.column_info();
            if(!column_info.hasSettings(QueryCraft::ColumnSettings::PRIMARY_KEY)) {
                return;
            }

            auto property = column.property();

            const auto string_property_value = property.converter()
                                                   ->convertToString(property.value(value));
            _condition_group = column_info == string_property_value;
        });

        _without_relation_entity = true;

        return get() != nullptr;
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

        const bool has_transactional = _open_transaction != nullptr;

        if(!has_transactional) {
            transaction();
        }

        std::vector<QueryCraft::ColumnInfo> columns_for_insert;

        std::for_each(begin, end, [this, &sql_table, &columns_for_insert](const auto& value) {
            bool need_update_column_info = columns_for_insert.empty();
            QueryCraft::SqlTable::Row row;
            _dto.for_each(Visitor::make_any_column_visitor(
                [&row, &value, &need_update_column_info, &columns_for_insert](auto& column) {
                    if(need_update_column_info) {
                        columns_for_insert.emplace_back(column.column_info());
                    }
                    action_fill_to_insert()(column, row, value);
                },
                [this, &value, &row, &need_update_column_info, &columns_for_insert](auto& reference_column) {
                    switch(reference_column.type()) {
                        case RelationType::ONE_TO_ONE: {
                            columns_for_insert.emplace_back(reference_column.column_info());
                            action_insert_one_to_one()(reference_column, row, value);
                            break;
                        }
                        case RelationType::ONE_TO_MANY: {
                            auto reference_storage = make_storage(_database, reference_column.reference_table());
                            reference_storage.set_transaction(_open_transaction);
                            const auto property_value = reference_column.property().value(value);
                            reference_storage.upsert(property_value);

                            reference_storage.set_transaction(nullptr);
                            break;
                        }
                    }
                }));

            sql_table.addRow(row);
        });

        const auto sql = sql_table.insertRowSql(columns_for_insert);
        std::cout << std::endl;
        std::cout << sql << std::endl;
        std::cout << std::endl;
        exec(sql);

        if(!has_transactional) {
            commit();
        }
    }

    template<typename Container>
    void insert(const Container& value)
    {
        insert(value.begin(), value.end());
    }

    void update(const ClassType& value)
    {
        QueryCraft::SqlTable sql_table(_dto.table_info());

        QueryCraft::ConditionGroup condition_for_update;
        std::vector<QueryCraft::ColumnInfo> columns_for_update;
        QueryCraft::SqlTable::Row row;

        const auto has_transactional = _open_transaction != nullptr;

        if(!has_transactional) {
            transaction();
        }

        _dto.for_each(Visitor::make_any_column_visitor(
            [&row, &condition_for_update, &columns_for_update, &value](auto& column) {
                action_fill_to_update()(column, value, condition_for_update, columns_for_update, row);
            },
            [this, &value, &row, &columns_for_update](auto& reference_column) {
                switch(reference_column.type()) {
                    case RelationType::ONE_TO_ONE: {
                        action_update_one_to_one()(reference_column, value, row, columns_for_update);
                        break;
                    }
                    case RelationType::ONE_TO_MANY: {
                        auto reference_storage = make_storage(_database, reference_column.reference_table());
                        reference_storage.set_transaction(_open_transaction);
                        const auto property_value = reference_column.property().value(value);
                        reference_storage.upsert(property_value);

                        reference_storage.set_transaction(nullptr);
                        break;
                    }
                }
            }));

        sql_table.addRow(row);

        const auto sql = sql_table.updateRowSql(condition_for_update, columns_for_update);
        std::cout << std::endl;
        std::cout << sql << std::endl;
        std::cout << std::endl;
        exec(sql);

        if(!has_transactional) {
            commit();
        }
    }

    template<typename Begin, typename End>
    void update(const Begin& begin, const End& end)
    {
        std::for_each(begin, end, [](const auto& value) {
            update(value);
        });
    }

    template<typename Container>
    void update(const Container& value)
    {
        update(value.begin(), value.end());
    }

    void upsert(const ClassType& value)
    {
        if(find(value)) {
            update(value);
        } else {
            insert(value);
        }
    }

    template<typename Begin, typename End>
    void upsert(const Begin& begin, const End& end)
    {
        std::for_each(begin, end, [this](const auto& value) {
            upsert(value);
        });
    }

    template<typename Container>
    void upsert(const Container& value)
    {
        upsert(value.begin(), value.end());
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
            _dto.for_each(Visitor::make_column_visitor([&value, &condition_for_remove](auto& column) {
                auto column_info = column.column_info();

                if(!column_info.hasSettings(QueryCraft::ColumnSettings::PRIMARY_KEY))
                    return;

                auto property = column.property();

                const auto string_property_value = property.converter()
                                                       ->convertToString(property.value(value));
                condition_for_remove = column_info == string_property_value;
            }));
        });

        const auto sql = sql_table.removeRowSql(condition_for_remove);
        std::cout << std::endl;
        std::cout << sql << std::endl;
        std::cout << std::endl;
        exec(sql);
    }

    template<typename Container>
    void remove(const Container& value)
    {
        remove(value.begin(), value.end());
    }

    void remove()
    {
        std::vector<ClassType> data;
        remove(data.begin(), data.end());
    }

private:
    static auto action_fill_to_insert()
    {
        return [](auto& column, QueryCraft::SqlTable::Row& row, const auto& value) {
            auto property = column.property();

            const auto property_value = property.value(value);
            const auto column_info = column.column_info();
            if(!column_info.hasSettings(QueryCraft::ColumnSettings::PRIMARY_KEY) && !column_info.hasSettings(QueryCraft::ColumnSettings::NOT_NULL) && column.null_cheker()->isNull(property_value)) {
                row.emplace_back(QueryCraft::ColumnInfo::nullValue());
            } else {
                row.emplace_back(property.converter()->convertToString(property_value));
            }
        };
    }

    static auto action_fill_to_update()
    {
        return [](auto& column, auto& value, QueryCraft::ConditionGroup& condition_for_update, std::vector<QueryCraft::ColumnInfo>& columns_for_update, QueryCraft::SqlTable::Row& row) {
            const auto column_info = column.column_info();

            auto property = column.property();

            const auto propery_value = property.value(value);

            if(column_info.hasSettings(QueryCraft::ColumnSettings::PRIMARY_KEY)) {
                condition_for_update = column_info == property.converter()->convertToString(propery_value);
            } else {
                columns_for_update.emplace_back(column_info);
                if(!column_info.hasSettings(QueryCraft::ColumnSettings::NOT_NULL) && column.null_cheker()->isNull(propery_value)) {
                    row.emplace_back(QueryCraft::ColumnInfo::nullValue());
                } else {
                    row.emplace_back(property.converter()->convertToString(propery_value));
                }
            }
        };
    }

    static auto action_fill_property()
    {
        return [](auto& column, const DatabaseAdapter::Models::QueryResult::ResultRow& query_result, auto& entity) {
            const auto column_info = column.column_info();

            auto property = column.property();

            const auto it = query_result.find(column_info.alias());
            if(it == query_result.end() || it->second == QueryCraft::ColumnInfo::nullValue())
                return;

            auto property_value = property.empty_property();
            property.converter()->fillFromString(property_value, it->second);
            property.set_value(entity, property_value);
        };
    }

    template<typename JoinClassType, typename... JoinClassColumn>
    static JoinClassType fill_class_by_sql(Table<JoinClassType, JoinClassColumn...>& dto,
        const DatabaseAdapter::Models::QueryResult::ResultRow& query_result,
        const std::shared_ptr<DatabaseAdapter::IDataBaseDriver>& database,
        const std::shared_ptr<DatabaseAdapter::ITransaction>& open_transaction)
    {
        auto entity = dto.empty_entity();

        dto.for_each(Visitor::make_any_column_visitor(
            [&entity, &query_result](auto& column) {
                action_fill_property()(column, query_result, entity);
            },
            [&entity, &query_result, &database, &open_transaction, &dto](auto& reference_column) {
                auto reference_propery = reference_column.property();
                auto reference_table = reference_column.reference_table();

                switch(reference_column.type()) {
                    case RelationType::ONE_TO_ONE: {
                        auto reference_entity = fill_class_by_sql(reference_table, query_result, database, open_transaction);

                        reference_propery.set_value(entity, reference_entity);
                        break;
                    }
                    case RelationType::ONE_TO_MANY: {
                        auto reference_storage = make_storage(database, reference_table);
                        reference_storage.set_transaction(open_transaction);

                        auto mapped_column = reference_table.table_info().column(reference_column.column_info().name());
                        QueryCraft::ConditionGroup condition;
                        dto.for_each(Visitor::make_column_visitor([&condition, &reference_column, &entity, &mapped_column](auto& column) {
                            if(column.column_info().hasSettings(QueryCraft::ColumnSettings::PRIMARY_KEY)) {
                                auto property = column.property();
                                auto id_value = property.converter()->convertToString(property.value(entity));
                                condition = mapped_column == id_value;
                            }
                        }));

                        reference_storage.condition_group(condition);

                        auto result = reference_storage.select();

                        auto property_value = reference_column.empty_property();
                        auto inserter = reference_column.inserter();
                        inserter.insertInRelationProperty(property_value, result);

                        reference_propery.set_value(entity, property_value);

                        break;
                    }
                }
            }));

        return entity;
    }

    template<typename JoinClassType, typename... JoinClassColumn>
    static QueryCraft::ColumnInfo primary_key_column(Table<JoinClassType, JoinClassColumn...>& dto)
    {
        QueryCraft::ColumnInfo primary_key;

        dto.for_each([&primary_key](const auto& column) {
            auto column_info = column.column_info();
            if(column_info.hasSettings(QueryCraft::ColumnSettings::PRIMARY_KEY))
                primary_key = column_info;
        });

        return primary_key;
    }

    template<typename JoinClassType, typename... JoinClassColumn>
    static std::vector<QueryCraft::JoinColumn> join_columns(Table<JoinClassType, JoinClassColumn...>& dto)
    {
        std::vector<QueryCraft::JoinColumn> joined_columns;

        dto.for_each(Visitor::make_reference_column_visitor([&joined_columns](auto& reference_column) {
            if(reference_column.type() != RelationType::ONE_TO_ONE) {
                return;
            }

            auto reference_table = reference_column.reference_table();

            QueryCraft::JoinColumn join_column;
            join_column.joinType = QueryCraft::JoinColumn::Type::LEFT;
            join_column.joinedTable = reference_table.table_info();
            join_column.condition = reference_column.column_info().equals(primary_key_column(reference_table));

            joined_columns.emplace_back(join_column);

            auto reference_joined_columns = join_columns(reference_table);

            joined_columns.insert(joined_columns.end(), reference_joined_columns.begin(), reference_joined_columns.end());
        }));

        return joined_columns;
    }

    template<typename JoinClassType, typename... JoinClassColumn>
    static void append_join_columns(std::vector<QueryCraft::ColumnInfo>& columns, Table<JoinClassType, JoinClassColumn...>& dto)
    {
        dto.for_each(Visitor::make_reference_column_visitor([&columns](auto& reference_column) {
            switch(reference_column.type()) {
                case RelationType::ONE_TO_ONE: {
                    auto reference_table = reference_column.reference_table();
                    reference_table.for_each([&columns](const auto& column) {
                        auto column_info = column.column_info();
                        columns.emplace_back(column_info);
                    });

                    append_join_columns(columns, reference_table);

                    break;
                }
            }
        }));
    }

private:
    auto action_insert_one_to_one()
    {
        return [this](auto& reference_column, QueryCraft::SqlTable::Row& row, const auto& value) {
            auto property = reference_column.property();

            const auto reference_property_value = property.value(value);

            auto reference_table = reference_column.reference_table();
            reference_table.for_each(Visitor::make_column_visitor([&row, &reference_property_value, &reference_column](auto& column) {
                auto column_info = column.column_info();
                if(column_info.hasSettings(QueryCraft::ColumnSettings::PRIMARY_KEY)) {
                    auto property = column.property();

                    const auto property_value = property.value(reference_property_value);
                    if(!reference_column.column_info().hasSettings(QueryCraft::ColumnSettings::NOT_NULL) && column.null_cheker()->isNull(property_value)) {
                        row.emplace_back(QueryCraft::ColumnInfo::nullValue());
                    } else {
                        row.emplace_back(property.converter()->convertToString(property_value));
                    }
                }
            }));

            auto reference_storage = make_storage(_database, reference_table);
            reference_storage.set_transaction(_open_transaction);

            if(row.back() != QueryCraft::ColumnInfo::nullValue())
                reference_storage.upsert(reference_property_value);

            // Нужно потому что в деструткоре вызывается commit если есть активная транзакция
            reference_storage.set_transaction(nullptr);
        };
    }

    auto action_update_one_to_one()
    {
        return [this](auto& reference_column, auto& value, QueryCraft::SqlTable::Row& row, std::vector<QueryCraft::ColumnInfo>& columns_for_update) {
            columns_for_update.emplace_back(reference_column.column_info());
            auto property = reference_column.property();

            const auto reference_property_value = property.value(value);

            auto reference_table = reference_column.reference_table();
            reference_table.for_each(Visitor::make_column_visitor([&row, &reference_property_value, &reference_column](auto& column) {
                auto column_info = column.column_info();
                if(column_info.hasSettings(QueryCraft::ColumnSettings::PRIMARY_KEY)) {
                    auto property = column.property();

                    const auto property_value = property.value(reference_property_value);
                    if(!reference_column.column_info().hasSettings(QueryCraft::ColumnSettings::NOT_NULL) && column.null_cheker()->isNull(property_value)) {
                        row.emplace_back(QueryCraft::ColumnInfo::nullValue());
                    } else {
                        row.emplace_back(property.converter()->convertToString(property_value));
                    }
                }
            }));

            auto reference_storage = make_storage(_database, reference_table);
            reference_storage.set_transaction(_open_transaction);

            if(row.back() != QueryCraft::ColumnInfo::nullValue())
                reference_storage.upsert(reference_property_value);

            // Нужно потому что в деструткоре вызывается commit если есть активная транзакция
            reference_storage.set_transaction(nullptr);
        };
    }

    void clear_select_settings()
    {
        _condition_group = {};
        _limit = 0;
        _offset = 0;
        _sortColumns = {};
        _without_relation_entity = false;
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
    bool _without_relation_entity = false;
};

template<typename ClassType, typename... Columns>
auto make_storage(const std::shared_ptr<DatabaseAdapter::IDataBaseDriver>& database, Table<ClassType, Columns...> dto)
{
    return Storage<ClassType, Columns...>(database, std::move(dto));
}

} // namespace EntityCraft
