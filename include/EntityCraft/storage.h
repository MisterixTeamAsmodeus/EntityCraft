#pragma once

#include "QueryCraft/sqltable.h"
#include "table.h"
#include "visitor/anycolumnvisitor.h"
#include "visitor/columnvisitor.h"
#include "visitor/referencecolumnvisitor.h"

#include <DatabaseAdapter/databaseadapter.h>

#include <iostream>
#include <memory>

namespace entity_craft {

#define storage_type(dto) decltype(make_storage(nullptr, dto));

template<typename ClassType, typename... Columns>
class storage
{
public:
    storage(const std::shared_ptr<database_adapter::IDataBaseDriver>& database, table<ClassType, Columns...> dto, const bool auto_commit = true)
        : _database(database)
        , _dto(std::move(dto))
        , _auto_commit(auto_commit)
    {
        if(!_database->is_open())
            _database->connect();
    }

    ~storage()
    {
        if(_open_transaction != nullptr && _auto_commit)
            _open_transaction->commit();

        _database->disconnect();
    }

    auto condition_group(const query_craft::condition_group& condition_group)
    {
        _condition_group = condition_group;
        return *this;
    }

    auto sort_columns(const std::vector<query_craft::sort_column>& sort_columns)
    {
        _sortColumns = sort_columns;
        return *this;
    }

    auto sort_column(const query_craft::sort_column& sort_column)
    {
        _sortColumns = { sort_column };
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
        _auto_commit = true;
        _open_transaction = _database->open_transaction(type);
    }

    std::shared_ptr<database_adapter::ITransaction> get_transaction() const
    {
        return _open_transaction;
    }

    void set_transaction(const std::shared_ptr<database_adapter::ITransaction>& transaction)
    {
        _auto_commit = false;
        _open_transaction = transaction;
    }

    std::shared_ptr<database_adapter::IDataBaseDriver> database() const
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
        const query_craft::sql_table sql_table(_dto.table_info());

        std::vector<query_craft::column_info> columns = sql_table.columns();

        _dto.for_each(visitor::make_reference_column_visitor([&columns](auto& reference_column) {
            auto it = std::remove(columns.begin(), columns.end(), reference_column.column_info());
            if(it != columns.end())
                columns.erase(it);
        }));

        if(!_without_relation_entity) {
            append_join_columns(columns, _dto);
        }

        const auto sql = sql_table.select_sql(
            _without_relation_entity ? std::vector<query_craft::join_column> {} : join_columns(_dto),
            _condition_group,
            _sortColumns,
            _limit,
            _offset,
            columns);

        clear_select_settings();

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
        _without_relation_entity = true;

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
        _condition_group = primary_key_column(_dto) == id;
        return get();
    }

    bool contains(const ClassType& value)
    {
        clear_select_settings();

        _dto.for_each([this, &value](const auto& column) {
            auto column_info = column.column_info();
            if(!column_info.has_settings(query_craft::column_settings::primary_key)) {
                return;
            }

            auto property = column.property();

            const auto string_property_value = property.property_converter()
                                                   ->convert_to_string(property.value(value));
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
        query_craft::sql_table sql_table(_dto.table_info());

        const bool has_transactional = _open_transaction != nullptr;

        if(!has_transactional) {
            transaction();
        }

        std::vector<query_craft::column_info> columns_for_insert;

        std::for_each(begin, end, [this, &sql_table, &columns_for_insert](const auto& value) {
            // Переменная чтобы только один раз записать названия колонок для вставки
            bool need_update_column_info = columns_for_insert.empty();
            query_craft::sql_table::row row;
            _dto.for_each(visitor::make_any_column_visitor(
                [&row, &value, &need_update_column_info, &columns_for_insert](auto& column) {
                    if(need_update_column_info) {
                        columns_for_insert.emplace_back(column.column_info());
                    }
                    action_fill_to_insert()(column, row, value);
                },
                [this, &value, &row, &columns_for_insert](auto& reference_column) {
                    switch(reference_column.type()) {
                        case relation_type::many_to_one:
                        case relation_type::one_to_one: {
                            columns_for_insert.emplace_back(reference_column.column_info());
                            action_insert()(reference_column, row, value);
                            break;
                        }
                        case relation_type::one_to_one_inverted:
                        case relation_type::one_to_many: {
                            auto reference_storage = make_storage(_database, reference_column.reference_table());
                            reference_storage.set_transaction(_open_transaction);
                            const auto property_value = reference_column.property().value(value);
                            reference_storage.upsert(property_value);
                            break;
                        }
                    }
                }));

            sql_table.add_row(row);
        });

        const auto sql = sql_table.insert_sql(columns_for_insert);

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
        query_craft::sql_table sql_table(_dto.table_info());

        query_craft::condition_group condition_for_update;
        std::vector<query_craft::column_info> columns_for_update;
        query_craft::sql_table::row row;

        const auto has_transactional = _open_transaction != nullptr;

        if(!has_transactional) {
            transaction();
        }

        _dto.for_each(visitor::make_any_column_visitor(
            [&row, &condition_for_update, &columns_for_update, &value](auto& column) {
                action_fill_to_update()(column, value, condition_for_update, columns_for_update, row);
            },
            [this, &value, &row, &columns_for_update](auto& reference_column) {
                switch(reference_column.type()) {
                    case relation_type::many_to_one:
                    case relation_type::one_to_one: {
                        action_update()(reference_column, value, row, columns_for_update);
                        break;
                    }
                    case relation_type::one_to_one_inverted:
                    case relation_type::one_to_many: {
                        auto reference_storage = make_storage(_database, reference_column.reference_table());
                        reference_storage.set_transaction(_open_transaction);
                        const auto property_value = reference_column.property().value(value);
                        reference_storage.upsert(property_value);
                        break;
                    }
                }
            }));

        sql_table.add_row(row);

        const auto sql = sql_table.update_sql(condition_for_update, columns_for_update);

        exec(sql);

        if(!has_transactional) {
            commit();
        }
    }

    template<typename Begin, typename End>
    void update(const Begin& begin, const End& end)
    {
        std::for_each(begin, end, [this](const auto& value) {
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
        if(contains(value)) {
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

    void remove(const query_craft::condition_info& condition)
    {
        remove(static_cast<query_craft::condition_group>(condition));
    }

    void remove(const query_craft::condition_group& condition)
    {
        const query_craft::sql_table sql_table(_dto.table_info());

        const auto sql = sql_table.remove_sql(condition);

        exec(sql);
    }

    void remove(const ClassType& value)
    {
        std::vector<ClassType> data = { value };

        remove(data.begin(), data.end());
    }

    template<typename Begin, typename End>
    void remove(const Begin& begin, const End& end)
    {
        query_craft::sql_table sql_table(_dto.table_info());

        query_craft::condition_group condition_for_remove;

        std::for_each(begin, end, [this, &condition_for_remove](const auto& value) {
            _dto.for_each(visitor::make_column_visitor([&value, &condition_for_remove](auto& column) {
                auto column_info = column.column_info();

                if(!column_info.has_settings(query_craft::column_settings::primary_key))
                    return;

                auto property = column.property();

                const auto string_property_value = property.property_converter()
                                                       ->convert_to_string(property.value(value));
                condition_for_remove = column_info == string_property_value;
            }));
        });

        const auto sql = sql_table.remove_sql(condition_for_remove);

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
        return [](auto& column, query_craft::sql_table::row& row, const auto& value) {
            auto property = column.property();

            const auto property_value = property.value(value);
            const auto column_info = column.column_info();
            if(!column_info.has_settings(query_craft::column_settings::primary_key) && !column_info.has_settings(query_craft::column_settings::not_null) && column.null_cheker()->is_null(property_value)) {
                row.emplace_back(query_craft::column_info::null_value());
            } else {
                row.emplace_back(property.property_converter()->convert_to_string(property_value));
            }
        };
    }

    static auto action_fill_to_update()
    {
        return [](auto& column, auto& value, query_craft::condition_group& condition_for_update, std::vector<query_craft::column_info>& columns_for_update, query_craft::sql_table::row& row) {
            const auto column_info = column.column_info();

            auto property = column.property();

            const auto propery_value = property.value(value);

            if(column_info.has_settings(query_craft::column_settings::primary_key)) {
                condition_for_update = column_info == property.property_converter()->convert_to_string(propery_value);
            } else {
                columns_for_update.emplace_back(column_info);
                if(!column_info.has_settings(query_craft::column_settings::not_null) && column.null_cheker()->is_null(propery_value)) {
                    row.emplace_back(query_craft::column_info::null_value());
                } else {
                    row.emplace_back(property.property_converter()->convert_to_string(propery_value));
                }
            }
        };
    }

    static auto action_fill_property()
    {
        return [](auto& column, const database_adapter::models::query_result::result_row& query_result, auto& entity) {
            const auto column_info = column.column_info();

            auto property = column.property();

            const auto it = query_result.find(column_info.alias());
            if(it == query_result.end() || it->second == query_craft::column_info::null_value())
                return;

            auto property_value = property.empty_property();
            property.property_converter()->fill_from_string(property_value, it->second);
            property.set_value(entity, property_value);
        };
    }

    template<typename JoinClassType, typename... JoinClassColumn>
    static JoinClassType fill_class_by_sql(table<JoinClassType, JoinClassColumn...>& dto,
        const database_adapter::models::query_result::result_row& query_result,
        const std::shared_ptr<database_adapter::IDataBaseDriver>& database,
        const std::shared_ptr<database_adapter::ITransaction>& open_transaction)
    {
        auto entity = dto.empty_entity();

        dto.for_each(visitor::make_any_column_visitor(
            [&entity, &query_result](auto& column) {
                action_fill_property()(column, query_result, entity);
            },
            [&entity, &query_result, &database, &open_transaction, &dto](auto& reference_column) {
                auto reference_propery = reference_column.property();
                auto reference_table = reference_column.reference_table();

                switch(reference_column.type()) {
                    case relation_type::many_to_one:
                    case relation_type::one_to_one_inverted:
                    case relation_type::one_to_one: {
                        auto reference_entity = fill_class_by_sql(reference_table, query_result, database, open_transaction);

                        reference_propery.set_value(entity, reference_entity);
                        break;
                    }
                    case relation_type::one_to_many: {
                        auto reference_storage = make_storage(database, reference_table);
                        reference_storage.set_transaction(open_transaction);

                        auto mapped_column = reference_table.table_info().column(reference_column.column_info().name());
                        query_craft::condition_group condition;
                        dto.for_each(visitor::make_column_visitor([&condition, &reference_column, &entity, &mapped_column](auto& column) {
                            if(column.column_info().has_settings(query_craft::column_settings::primary_key)) {
                                auto property = column.property();
                                auto id_value = property.property_converter()->convert_to_string(property.value(entity));
                                condition = mapped_column == id_value;
                            }
                        }));

                        reference_storage.condition_group(condition);

                        auto result = reference_storage.select();

                        auto property_value = reference_column.empty_property();
                        auto inserter = reference_column.inserter();
                        inserter.convert_to_target(property_value, result);

                        reference_propery.set_value(entity, property_value);

                        break;
                    }
                }
            }));

        return entity;
    }

    template<typename JoinClassType, typename... JoinClassColumn>
    static query_craft::column_info primary_key_column(table<JoinClassType, JoinClassColumn...>& dto)
    {
        query_craft::column_info primary_key;

        dto.for_each([&primary_key](const auto& column) {
            auto column_info = column.column_info();
            if(column_info.has_settings(query_craft::column_settings::primary_key))
                primary_key = column_info;
        });

        return primary_key;
    }

    template<typename JoinClassType, typename... JoinClassColumn>
    static std::vector<query_craft::join_column> join_columns(table<JoinClassType, JoinClassColumn...>& dto)
    {
        std::vector<query_craft::join_column> joined_columns;

        dto.for_each(visitor::make_reference_column_visitor([&joined_columns, &dto](auto& reference_column) {
            auto reference_table = reference_column.reference_table();

            query_craft::join_column join_column;
            join_column.join_type = query_craft::join_column::type::left;
            join_column.joined_table = reference_table.table_info();
            switch(reference_column.type()) {
                case relation_type::one_to_one:
                case relation_type::many_to_one: {
                    join_column.condition = reference_column.column_info().equals(primary_key_column(reference_table));
                    break;
                }
                case relation_type::one_to_one_inverted: {
                    join_column.condition = primary_key_column(dto).equals(reference_table.table_info().column(reference_column.column_info().name()));
                    break;
                }
                default:
                    return;
            }

            joined_columns.emplace_back(join_column);

            auto reference_joined_columns = join_columns(reference_table);

            joined_columns.insert(joined_columns.end(), reference_joined_columns.begin(), reference_joined_columns.end());
        }));

        return joined_columns;
    }

    template<typename JoinClassType, typename... JoinClassColumn>
    static void append_join_columns(std::vector<query_craft::column_info>& columns, table<JoinClassType, JoinClassColumn...>& dto)
    {
        dto.for_each(visitor::make_reference_column_visitor([&columns](auto& reference_column) {
            switch(reference_column.type()) {
                case relation_type::many_to_one:
                case relation_type::one_to_one_inverted:
                case relation_type::one_to_one: {
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
    auto action_insert()
    {
        return [this](auto& reference_column, query_craft::sql_table::row& row, const auto& value) {
            auto property = reference_column.property();

            const auto reference_property_value = property.value(value);

            auto reference_table = reference_column.reference_table();
            reference_table.for_each(visitor::make_column_visitor([&row, &reference_property_value, &reference_column](auto& column) {
                auto column_info = column.column_info();
                if(column_info.has_settings(query_craft::column_settings::primary_key)) {
                    auto property = column.property();

                    const auto property_value = property.value(reference_property_value);
                    if(!reference_column.column_info().has_settings(query_craft::column_settings::not_null) && column.null_cheker()->is_null(property_value)) {
                        row.emplace_back(query_craft::column_info::null_value());
                    } else {
                        row.emplace_back(property.property_converter()->convert_to_string(property_value));
                    }
                }
            }));

            auto reference_storage = make_storage(_database, reference_table);
            reference_storage.set_transaction(_open_transaction);

            if(row.back() != query_craft::column_info::null_value())
                reference_storage.upsert(reference_property_value);
        };
    }

    auto action_update()
    {
        return [this](auto& reference_column, auto& value, query_craft::sql_table::row& row, std::vector<query_craft::column_info>& columns_for_update) {
            columns_for_update.emplace_back(reference_column.column_info());
            auto property = reference_column.property();

            const auto reference_property_value = property.value(value);

            auto reference_table = reference_column.reference_table();
            reference_table.for_each(visitor::make_column_visitor([&row, &reference_property_value, &reference_column](auto& column) {
                auto column_info = column.column_info();
                if(column_info.has_settings(query_craft::column_settings::primary_key)) {
                    auto property = column.property();

                    const auto property_value = property.value(reference_property_value);
                    if(!reference_column.column_info().has_settings(query_craft::column_settings::not_null) && column.null_cheker()->is_null(property_value)) {
                        row.emplace_back(query_craft::column_info::null_value());
                    } else {
                        row.emplace_back(property.property_converter()->convert_to_string(property_value));
                    }
                }
            }));

            auto reference_storage = make_storage(_database, reference_table);
            reference_storage.set_transaction(_open_transaction);

            if(row.back() != query_craft::column_info::null_value())
                reference_storage.upsert(reference_property_value);
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

    database_adapter::models::query_result exec(const std::string& sql) const
    {
        return _open_transaction != nullptr ? _open_transaction->exec(sql) : _database->exec(sql);
    }

private:
    std::shared_ptr<database_adapter::IDataBaseDriver> _database;
    std::shared_ptr<database_adapter::ITransaction> _open_transaction;
    table<ClassType, Columns...> _dto;
    bool _auto_commit;

    // Настройки для select
    query_craft::condition_group _condition_group;
    std::vector<query_craft::sort_column> _sortColumns;
    size_t _limit = 0;
    size_t _offset = 0;
    bool _without_relation_entity = false;
};

template<typename ClassType, typename... Columns>
auto make_storage(const std::shared_ptr<database_adapter::IDataBaseDriver>& database, table<ClassType, Columns...> dto, const bool auto_commit = true)
{
    return storage<ClassType, Columns...>(database, std::move(dto), auto_commit);
}

} // namespace entity_craft
