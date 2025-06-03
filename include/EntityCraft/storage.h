#pragma once

#include "table.h"
#include "visitor/anycolumnvisitor.h"
#include "visitor/columnvisitor.h"
#include "visitor/referencecolumnvisitor.h"

#include <DatabaseAdapter/databaseadapter.h>

#include <QueryCraft/sqltable.h>

#include <memory>
#include <set>

namespace entity_craft {

#define storage_type(dto) decltype(make_storage(nullptr, dto));

template<typename ClassType, typename... Columns>
class storage
{
public:
    using class_type = ClassType;

    storage(const std::shared_ptr<database_adapter::IDataBaseDriver>& database, table<ClassType, Columns...> dto, const bool auto_commit = true)
        : _database(database)
        , _dto(std::move(dto))
        , _auto_commit(auto_commit)
    {
        if(!_database->is_open())
            _database->connect();
    }

    storage(const storage& other) = default;
    storage(storage&& other) noexcept = default;
    storage& operator=(const storage& other) = default;
    storage& operator=(storage&& other) noexcept = default;

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

    database_adapter::models::query_result exec(const std::string& sql) const
    {
        return _open_transaction != nullptr ? _open_transaction->exec(sql) : _database->exec(sql);
    }

    auto dto() const
    {
        return _dto;
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

        const auto duplicate_column = _dto.duplicate_column();
        _dto.for_each(visitor::make_reference_column_visitor([&duplicate_column, &columns](auto& reference_column) {
            if(std::find(duplicate_column.begin(), duplicate_column.end(), reference_column.column_info()) != duplicate_column.end()) {
                return;
            }

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

        const auto result = exec(sql);

        if(result.empty()) {
            return {};
        }

        const bool has_transaction = _open_transaction != nullptr;
        if(!has_transaction) {
            transaction();
        }

        std::vector<ClassType> res;
        for(const auto& row : result.data()) {
            auto entity = parse_entity_from_sql(_dto, row, _without_relation_entity);
            if(_dto.has_reques_callback()) {
                _dto.reques_callback()->post_request_callback(entity, request_callback_type::select, _open_transaction);
            }
            res.emplace_back(entity);
        }

        if(!has_transaction) {
            commit();
        }

        clear_select_settings();

        return res;
    }

    template<typename Begin, typename End>
    std::vector<ClassType> select_by_ids(const Begin& begin, const End& end)
    {
        if(begin == end)
            return {};

        _condition_group = primary_key_column(_dto).in_list(begin, end);
        return select();
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

    void insert(ClassType& value)
    {
        std::vector<ClassType> data = { value };

        insert(data.begin(), data.end());
    }

    template<typename Begin, typename End>
    void insert(const Begin& begin, const End& end)
    {
        if(begin == end) {
            return;
        }

        query_craft::sql_table sql_table(_dto.table_info());

        const bool has_transactional = _open_transaction != nullptr;

        if(!has_transactional) {
            transaction();
        }

        std::vector<query_craft::column_info> columns_for_insert;

        std::for_each(begin, end, [this, &sql_table, &columns_for_insert](auto& value) {
            if(_dto.has_reques_callback()) {
                _dto.reques_callback()->pre_request_callback(value, request_callback_type::insert, _open_transaction);
            }
            this->prepare_to_insert(columns_for_insert, sql_table, value);
        });

        const auto sql = sql_table.insert_sql(columns_for_insert);

        exec(sql);

        // Вставка зависимых объектов должна происходить после вставки объекта на который происходит ссылка
        std::for_each(begin, end, [this](auto& value) {
            if(_dto.has_reques_callback()) {
                _dto.reques_callback()->post_request_callback(value, request_callback_type::insert, _open_transaction);
            }

            // Вставка зависимых объектов должна происходить после вставки объекта на который происходит ссылка
            _dto.for_each(visitor::make_reference_column_visitor([this, &value](auto& reference_column) {
                if(reference_column.type() != relation_type::one_to_one_inverted && reference_column.type() != relation_type::one_to_many) {
                    return;
                }

                // Если нет прав на каскадную вставку ничего не делаем
                if(!reference_column.has_cascade(cascade_type::all) && !reference_column.has_cascade(cascade_type::persist)) {
                    return;
                }

                this->upsert_relation_property_with_type_one_to_one_inverted_or_one_to_many(reference_column, value);
            }));
        });

        if(!has_transactional) {
            commit();
        }
    }

    template<typename Container>
    void insert(Container& value)
    {
        insert(value.begin(), value.end());
    }

    void update(ClassType& value)
    {
        query_craft::sql_table sql_table(_dto.table_info());

        const auto has_transactional = _open_transaction != nullptr;

        if(!has_transactional) {
            transaction();
        }

        _dto.for_each(visitor::make_reference_column_visitor([this, &value](auto& reference_column) {
            if(reference_column.has_cascade(cascade_type::all) || reference_column.has_cascade(cascade_type::merge_orphan)) {
                this->sync_deleted_reference(value, reference_column);
            } else {
                this->update_deleted_reference(value, reference_column);
            }
        }));

        query_craft::condition_group condition_for_update;
        std::vector<query_craft::column_info> columns_for_update;
        query_craft::sql_table::row row;

        if(_dto.has_reques_callback()) {
            _dto.reques_callback()->pre_request_callback(value, request_callback_type::update, _open_transaction);
        }

        prepare_to_update(value, condition_for_update, columns_for_update, row);

        sql_table.add_row(row);

        const auto sql = sql_table.update_sql(condition_for_update, columns_for_update);

        exec(sql);

        if(_dto.has_reques_callback()) {
            _dto.reques_callback()->post_request_callback(value, request_callback_type::update, _open_transaction);
        }

        // Вставка зависимых объектов должна происходить после вставки объекта на который происходит ссылка
        _dto.for_each(visitor::make_reference_column_visitor([this, &value](auto& reference_column) {
            if(reference_column.type() != relation_type::one_to_one_inverted && reference_column.type() != relation_type::one_to_many) {
                return;
            }

            // Если нет прав на каскадную вставку ничего не делаем
            if(!reference_column.has_cascade(cascade_type::all) && !reference_column.has_cascade(cascade_type::merge) && !reference_column.has_cascade(cascade_type::merge_orphan)) {
                return;
            }

            this->upsert_relation_property_with_type_one_to_one_inverted_or_one_to_many(reference_column, value);
        }));

        if(!has_transactional) {
            commit();
        }
    }

    template<typename Begin, typename End>
    void update(const Begin& begin, const End& end)
    {
        std::for_each(begin, end, [this](auto& value) {
            this->update(value);
        });
    }

    template<typename Container>
    void update(Container& value)
    {
        update(value.begin(), value.end());
    }

    void upsert(ClassType& value)
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
        std::for_each(begin, end, [this](auto& value) {
            this->upsert(value);
        });
    }

    template<typename Container>
    void upsert(Container& value)
    {
        upsert(value.begin(), value.end());
    }

    void remove_by_condition(const query_craft::condition_info& condition)
    {
        clear_select_settings();
        _condition_group = condition;

        auto res = select();
        remove(res);
    }

    void remove_by_condition(const query_craft::condition_group& condition)
    {
        clear_select_settings();
        _condition_group = condition;

        auto res = select();
        remove(res);
    }

    void remove(ClassType& value)
    {
        std::vector<ClassType> data = { value };

        remove(data.begin(), data.end());
    }

    template<typename Begin, typename End>
    void remove(const Begin& begin, const End& end)
    {
        if(begin == end) {
            return;
        }

        query_craft::sql_table sql_table(_dto.table_info());

        query_craft::condition_group condition_for_remove;

        const auto has_transactional = _open_transaction != nullptr;

        if(!has_transactional) {
            transaction();
        }

        std::for_each(begin, end, [this, &condition_for_remove](auto& value) {
            if(_dto.has_reques_callback()) {
                _dto.reques_callback()->pre_request_callback(value, request_callback_type::remove, _open_transaction);
            }

            _dto.for_each(visitor::make_any_column_visitor(
                [&value, &condition_for_remove](auto& column) {
                    auto column_info = column.column_info();

                    if(!column_info.has_settings(query_craft::column_settings::primary_key))
                        return;

                    auto property = column.property();

                    const auto string_property_value = property.property_converter()
                                                           ->convert_to_string(property.value(value));

                    if(condition_for_remove.is_valid()) {
                        condition_for_remove = condition_for_remove || column_info == string_property_value;
                    } else {
                        condition_for_remove = column_info == string_property_value;
                    }
                },
                [this, &value](auto& reference_column) {
                    if(reference_column.has_cascade(cascade_type::all) || reference_column.has_cascade(cascade_type::remove)) {
                        auto reference_storage = make_storage(this->_database, reference_column.reference_table());
                        reference_storage.set_transaction(this->_open_transaction);

                        auto property_value = reference_column.property().value(value);
                        reference_storage.remove(property_value);
                    } else {
                        this->update_deleted_reference(value, reference_column);
                    }
                }));
        });

        const auto sql = sql_table.remove_sql(condition_for_remove);

        exec(sql);

        std::for_each(begin, end, [this](auto& value) {
            if(!_dto.has_reques_callback()) {
                return;
            }

            _dto.reques_callback()->post_request_callback(value, request_callback_type::remove, _open_transaction);
        });

        if(!has_transactional) {
            commit();
        }
    }

    template<typename Container>
    void remove(Container& value)
    {
        remove(value.begin(), value.end());
    }

    void remove()
    {
        std::vector<ClassType> data;
        remove(data.begin(), data.end());
    }

private:
    /**
     * Получить информацию о колонке с флагом primary_key
     * @param dto Таблица из которой нужно получить информаю
     * @return Информацию о primary_key
     */
    template<typename Dto>
    static query_craft::column_info primary_key_column(Dto& dto)
    {
        query_craft::column_info primary_key;

        dto.for_each([&primary_key](const auto& column) {
            auto column_info = column.column_info();
            if(column_info.has_settings(query_craft::column_settings::primary_key))
                primary_key = column_info;
        });

        return primary_key;
    }

    /**
     * Добавить информацию о join на основе связанных сущностей
     * @param columns Список куда нужно добавлять информацию о колонках
     * @param dto Таблица из которой нужно добавить информацию о связанных колонках
     * @note Используется для many_to_one, one_to_one_inverted, one_to_one
     * @note Рекурсивно дополняет связанные сущности
     */
    template<typename Dto>
    static void append_join_columns(std::vector<query_craft::column_info>& columns, Dto& dto)
    {
        dto.for_each(visitor::make_reference_column_visitor([&columns](auto& reference_column) {
            if(reference_column.type() != relation_type::many_to_one
                && reference_column.type() != relation_type::one_to_one_inverted
                && reference_column.type() != relation_type::one_to_one) {
                return;
            }

            auto reference_table = reference_column.reference_table();
            reference_table.for_each([&columns](const auto& column) {
                auto column_info = column.column_info();
                columns.emplace_back(column_info);
            });

            append_join_columns(columns, reference_table);
        }));
    }

    /**
     * Получить информацию о необходимых join для выборки со связанными сущностями
     * @param dto Таблица для которой необходимо получить join параметры
     * @return Информацию о необходимых join для выборки со связанными сущностями
     */
    template<typename Dto>
    static std::vector<query_craft::join_column> join_columns(Dto& dto)
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

    /**
     * Функция для заполнения поля класса на основе данных из sql запроса
     * @param column Колонка по которой нужно заполнить связанное поле
     * @param query_result Результат запроса
     * @param entity Сущность в которой находится связанное поле
     */
    template<typename Column_, typename Entity_>
    static void parse_property_from_sql(Column_& column, const database_adapter::models::query_result::result_row& query_result, Entity_& entity)
    {
        const auto column_info = column.column_info();

        auto property = column.property();

        const auto it = query_result.find(column_info.alias());
        if(it == query_result.end() || it->second == query_craft::column_info::null_value())
            return;

        auto property_value = property.empty_property();
        property.property_converter()->fill_from_string(property_value, it->second);
        property.set_value(entity, property_value);
    }

    /**
     * Функция для определения какие ссылки были удалены при обновлении объекта для отношений one to many
     * @tparam Value Контейнер который хранит ссылочные объекты
     * @tparam Dto ДТО для получение информации из объекта
     * @param old_value Старое значение
     * @param new_value Новое значение
     * @param dto ДТО для получение информации из объекта
     * @return Множетсво объектов ссылки на которые были удалены
     */
    template<typename Value, typename Dto,
        std::enable_if_t<sfinae::is_iterable_v<Value>, bool> = true>
    static std::vector<typename Value::value_type> check_need_remove(const Value& old_value, const Value& new_value, Dto& dto)
    {
        std::unordered_map<std::string, typename Value::value_type> old_map;
        std::set<std::string> new_ids;

        dto.for_each([&old_map, &new_ids, &old_value, &new_value](const auto& column) {
            auto column_info = column.column_info();
            if(!column_info.has_settings(query_craft::column_settings::primary_key)) {
                return;
            }

            auto converter = column.converter();
            for(const auto& value : old_value) {
                auto column_value = column.property().value(value);
                if(!column.null_cheker()->is_null(column_value)) {
                    old_map.insert(std::make_pair(converter->convert_to_string(column_value), value));
                }
            }

            for(const auto& value : new_value) {
                new_ids.insert(converter->convert_to_string(column.property().value(value)));
            }
        });

        std::vector<typename Value::value_type> removed;

        for(const auto& id : old_map) {
            if(new_ids.find(id.first) == new_ids.end())
                removed.emplace_back(id.second);
        }

        return removed;
    }

    /**
     * Функция для определения какие ссылки были удалены при обновлении объекта для отношений one to one inverted
     * @tparam Dto ДТО для получение информации из объекта
     * @param old_value Старое значение
     * @param new_value Новое значение
     * @param dto ДТО для получение информации из объекта
     * @return Множетсво объектов ссылки на которые были удалены
     */
    template<typename Value,
        typename Dto,
        std::enable_if_t<!sfinae::is_iterable_v<Value>, bool> = true>
    static std::vector<Value> check_need_remove(const Value& old_value, const Value& new_value, Dto& dto)
    {
        std::vector<Value> old_c = { old_value };
        std::vector<Value> new_c = { new_value };

        return check_need_remove(old_c, new_c, dto);
    }

private:
    /**
     * Заполнить сущность по данным из sql запроса
     * @param dto DTO для заполнения
     * @param query_result Результат запроса
     * @param without_relation_entity Флаг на догрузку полей с типом связи one to many
     * @return
     */
    template<typename JoinClassType, typename... JoinClassColumn>
    JoinClassType parse_entity_from_sql(table<JoinClassType, JoinClassColumn...>& dto,
        const database_adapter::models::query_result::result_row& query_result,
        bool without_relation_entity)
    {
        auto entity = dto.empty_entity();

        dto.for_each(visitor::make_any_column_visitor(
            [&entity, &query_result](auto& column) {
                parse_property_from_sql(column, query_result, entity);
            },
            [this, &entity, &query_result, &dto, &without_relation_entity](auto& reference_column) {
                auto reference_propery = reference_column.property();
                auto reference_table = reference_column.reference_table();

                switch(reference_column.type()) {
                    case relation_type::many_to_one:
                    case relation_type::one_to_one_inverted:
                    case relation_type::one_to_one: {
                        auto reference_entity = this->parse_entity_from_sql(reference_table, query_result, without_relation_entity);

                        if(reference_table.has_reques_callback()) {
                            reference_table.reques_callback()->post_request_callback(reference_entity, request_callback_type::select, _open_transaction);
                        }

                        reference_propery.set_value(entity, reference_entity);
                        break;
                    }
                    case relation_type::one_to_many: {
                        if(without_relation_entity)
                            break;

                        auto reference_storage = make_storage(_database, reference_table);
                        reference_storage.set_transaction(_open_transaction);

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
                        reference_column.inserter().convert_to_target(property_value, result);

                        reference_propery.set_value(entity, property_value);

                        break;
                    }
                }
            }));

        return entity;
    }

    /**
     * Функция для подготовки данных для генерации запросов на обновление
     * @param value
     * @param condition_for_update
     * @param columns_for_update
     * @param row
     */
    void prepare_to_update(const ClassType& value, query_craft::condition_group& condition_for_update, std::vector<query_craft::column_info>& columns_for_update, query_craft::sql_table::row& row)
    {
        _dto.for_each(visitor::make_any_column_visitor(
            [&row, &condition_for_update, &columns_for_update, &value](auto& column) {
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
            },
            [this, &value, &row, &columns_for_update](auto& reference_column) {
                // Обработка других типов отношений находится дальше,
                // так как для корректности ссылок нужно сначала вставить объекты на которые будет создаваться ссылка
                if(reference_column.type() != relation_type::many_to_one && reference_column.type() != relation_type::one_to_one) {
                    return;
                }

                // Добавляем эту колонку так как при таком типе связи ссылочная информация хранится в текущей таблице а не в ссылочной
                columns_for_update.emplace_back(reference_column.column_info());

                bool cascade = reference_column.has_cascade(cascade_type::all) || reference_column.has_cascade(cascade_type::merge) || reference_column.has_cascade(cascade_type::merge_orphan);
                this->upsert_relation_property_with_type_one_to_one_or_many_to_one(reference_column, row, value, cascade);
            }));
    }

    /**
     * Функция для подготовки данных для генерации запросов на вставку
     * Так же функция занимается добавлением объектов с типом связи many_to_one или one_to_one
     * @param columns_for_insert Список колонок которые будут использоваться для генерации INSERT запроса
     * @param sql_table Представление таблицы в которые будут добавлены данные для вставки
     * @param value Значение типа из которого будут браться значение связанных полей
     */
    void prepare_to_insert(std::vector<query_craft::column_info>& columns_for_insert, query_craft::sql_table& sql_table, const ClassType& value)
    {
        // Переменная чтобы только один раз записать названия колонок для вставки
        bool need_update_column_info = columns_for_insert.empty();

        query_craft::sql_table::row row;

        _dto.for_each(visitor::make_any_column_visitor(
            [this, &row, &value, &need_update_column_info, &columns_for_insert](auto& column) {
                if(need_update_column_info) {
                    // Добавляем имена колонок для блока INSERT INTO (column1, column2, ...);
                    columns_for_insert.emplace_back(column.column_info());
                }

                auto property = column.property();

                const auto property_value = property.value(value);
                const auto column_info = column.column_info();

                if(!column_info.has_settings(query_craft::column_settings::primary_key)
                    && !column_info.has_settings(query_craft::column_settings::not_null)
                    && column.null_cheker()->is_null(property_value)) {
                    // Если колонка имеет флаг что она может быть null
                    // и при проверки null_cheker вернул true то записываем null вместо значения поля
                    row.emplace_back(query_craft::column_info::null_value());
                } else {
                    // Иначе добавляем сконвертированное значения поля
                    row.emplace_back(property.property_converter()->convert_to_string(property_value));
                }
            },
            [this, &value, &row, &columns_for_insert](auto& reference_column) {
                // Обработка других типов отношений находится дальше,
                // так как для корректности ссылок нужно сначала вставить объекты на которые будет создаваться ссылка
                if(reference_column.type() != relation_type::many_to_one && reference_column.type() != relation_type::one_to_one) {
                    return;
                }

                // Добавляем эту колонку так как при таком типе связи ссылочная информация хранится в текущей таблице а не в ссылочной
                columns_for_insert.emplace_back(reference_column.column_info());

                bool cascade = reference_column.has_cascade(cascade_type::all) || reference_column.has_cascade(cascade_type::persist);
                this->upsert_relation_property_with_type_one_to_one_or_many_to_one(reference_column, row, value, cascade);
            }));

        sql_table.add_row(row);
    }

    /**
     * Функция для добавления ссылочных объктов с типом связи one_to_one or many_to_one
     * @param reference_column Колонка из которой будет браться информация о связанной таблице
     * @param row Строка куда будут вставлена информация о идентификаторе ссылки
     * @param value Значение из которого будет браться информация для вствки
     */
    template<typename ReferenceCoulmn_>
    void upsert_relation_property_with_type_one_to_one_or_many_to_one(
        ReferenceCoulmn_& reference_column,
        query_craft::sql_table::row& row,
        const ClassType& value,
        bool cascad)
    {
        auto property = reference_column.property();
        auto reference_property_value = property.value(value);

        auto reference_table = reference_column.reference_table();
        reference_table.for_each(visitor::make_column_visitor([&row, &reference_property_value, &reference_column](auto& column) {
            auto column_info = column.column_info();
            // Добавляем информации для ссылке по primary_key в исходную таблицу
            if(!column_info.has_settings(query_craft::column_settings::primary_key)) {
                return;
            }
            auto reference_property = column.property();

            const auto property_value = reference_property.value(reference_property_value);
            // Если исходная колонка которая содержит ссылку имеет флаг что она может быть null
            // и при проверки null_cheker вернул true то записываем null вместо значения поля
            if(!reference_column.column_info().has_settings(query_craft::column_settings::not_null) && column.null_cheker()->is_null(property_value)) {
                row.emplace_back(query_craft::column_info::null_value());
            } else {
                // Иначе добавляем сконвертированное значения поля
                row.emplace_back(reference_property.property_converter()->convert_to_string(property_value));
            }
        }));

        // Если установлены права на каскадную вставку добавляем объект если у него не пустой primary_key
        if(cascad && row.back() != query_craft::column_info::null_value()) {
            auto reference_storage = make_storage(_database, reference_table);
            reference_storage.set_transaction(_open_transaction);
            reference_storage.upsert(reference_property_value);
        }
    }

    /**
     * Функция для добавления ссылочных объктов с типом связи one_to_one_inverted or one_to_many
     * @param reference_column Колонка из которой будет браться информация о связанной таблице
     * @param value Значение из которого будет браться информация для вставки
     */
    template<typename ReferenceCoulmn_>
    void upsert_relation_property_with_type_one_to_one_inverted_or_one_to_many(ReferenceCoulmn_& reference_column, const ClassType& value)
    {
        auto reference_table = reference_column.reference_table();
        auto reference_storage = make_storage(_database, reference_table);
        reference_storage.set_transaction(_open_transaction);

        bool is_empty_property = false;
        auto property_value = reference_column.property().value(value);

        // Если тип связи one_to_one_inverted необходимо проверить
        // что объект не пустой для избежания ситуции когда будет создана строка с пустым объектом
        if(reference_column.type() == relation_type::one_to_one_inverted) {
            reference_table.for_each([&property_value, &is_empty_property](const auto& column) {
                auto column_info = column.column_info();
                if(!column_info.has_settings(query_craft::column_settings::primary_key)) {
                    return;
                }

                is_empty_property = column.null_cheker()->is_null(column.property().value(property_value));
            });
        }

        if(!is_empty_property) {
            reference_storage.upsert(property_value);
        }
    }

    /**
     * Функция для отслеживания и очистки удалённых связей
     * @param value Значение из которого будет браться информация для отслеживания
     * @param reference_column Колонка из которой будет браться информация о связанной таблице
     */
    template<typename ReferenceCoulmn_>
    void sync_deleted_reference(const ClassType& value, ReferenceCoulmn_& reference_column)
    {
        auto reference_table = reference_column.reference_table();
        auto reference_storage = make_storage(this->_database, reference_table);
        reference_storage.set_transaction(this->_open_transaction);

        const auto property_value = reference_column.property().value(value);
        const auto old_state = get_old_state(value);

        auto removed_items = check_need_remove(reference_column.property().value(old_state), property_value, reference_table);

        if(removed_items.empty()) {
            return;
        }

        reference_storage.remove(removed_items);
    }

    /**
     * Функция для отслеживания и очистки удалённых связей
     * @param value Значение из которого будет браться информация для отслеживания
     * @param reference_column Колонка из которой будет браться информация о связанной таблице
     */
    template<typename Value_, typename ReferenceCoulmn_>
    void update_deleted_reference(const Value_& value, ReferenceCoulmn_& reference_column)
    {
        auto reference_table = reference_column.reference_table();
        auto reference_storage = make_storage(_database, reference_table);
        reference_storage.set_transaction(_open_transaction);

        const auto property_value = reference_column.property().value(value);
        const auto old_state = get_old_state(value);

        auto removed_items = check_need_remove(reference_column.property().value(old_state), property_value, reference_table);

        if(removed_items.empty()) {
            return;
        }

        for(auto& removed_item : removed_items) {
            reference_column.reference_table().for_each(visitor::make_column_visitor([&removed_item, &reference_column](auto& column) {
                auto column_info = column.column_info();
                if(column_info.name() != reference_column.column_info().name())
                    return;

                auto property = column.property();

                property.set_value(removed_item, decltype(property.empty_property())());
            }));
        }

        reference_storage.update(removed_items);
    }

    /**
     * Получить текущее состояние в базе данных на основе обновленного значения
     * @param value обновлённое сосотяние
     * @return Текущее состояние в базе данных
     */
    ClassType get_old_state(const ClassType& value)
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

        return *get();
    }

    /**
     * Очистить настройки для выборки
     */
    void clear_select_settings()
    {
        _condition_group = {};
        _limit = 0;
        _offset = 0;
        _sortColumns = {};
        _without_relation_entity = false;
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
