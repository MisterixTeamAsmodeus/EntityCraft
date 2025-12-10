#pragma once

#include "DatabaseAdapter/iconnection.hpp"
#include "mapper.h"
#include "storage_utils.h"
#include "table.h"

#include <QueryCraft/builder/delete_builder.h>
#include <QueryCraft/builder/dsl.h>
#include <QueryCraft/builder/insert_builder.h>
#include <QueryCraft/builder/select_builder.h>
#include <QueryCraft/builder/update_builder.h>

#include <memory>
#include <set>
#include <stdexcept>
#include <vector>

namespace entity_craft {

#define storage_type(dto) decltype(make_storage(nullptr, dto));

template<typename ClassType, typename... Columns>
class storage
{
public:
    using class_type = ClassType;

    storage(const std::shared_ptr<database_adapter::IConnection>& database, table<ClassType, Columns...> dto, const bool auto_commit = true);

    storage(const storage& other) = default;
    storage(storage&& other) noexcept = default;

    ~storage();

    storage& operator=(const storage& other) = default;
    storage& operator=(storage&& other) noexcept = default;

    /**
     * @brief Вставляет одну сущность в базу данных
     * @param entity Сущность для вставки
     * @return Вставленная сущность с обновленным ID (если auto_increment)
     */
    ClassType insert(const ClassType& entity);

    /**
     * @brief Вставляет несколько сущностей в базу данных (batch insert) из итераторов.
     * @param begin Начало итератора.
     * @param end Конец итератора.
     * @return Вектор вставленных сущностей с обновленными ID (если auto_increment)
     */
    template<typename Begin, typename End>
    std::vector<ClassType> insert_batch(Begin begin, End end);

    /**
     * @brief Находит сущность по ID
     * @tparam IdType Тип ID
     * @param id Значение ID
     * @return Уникальный указатель на сущность, если найдена, иначе nullptr
     */
    template<typename IdType>
    std::unique_ptr<ClassType> find_by_id(const IdType& id);

    /**
     * @brief Получает все сущности из таблицы
     * @return Вектор всех сущностей
     */
    std::vector<ClassType> find_all();

    /**
     * @brief Находит одну сущность по условию
     * @param condition Условие WHERE
     * @return Уникальный указатель на сущность, если найдена, иначе nullptr
     */
    std::unique_ptr<ClassType> find_one(const query_craft::ast::expression& condition);

    /**
     * @brief Находит сущности по условию
     * @param condition Условие WHERE
     * @return Вектор найденных сущностей
     */
    std::vector<ClassType> find_where(const query_craft::ast::expression& condition);

    /**
     * @brief Обновляет несколько сущностей в базе данных (batch update) из итераторов.
     * @param begin Начало итератора.
     * @param end Конец итератора.
     * @return Вектор обновленных сущностей
     */
    template<typename Begin, typename End>
    std::vector<ClassType> update_batch(Begin begin, End end);

    /**
     * @brief Обновляет сущность в базе данных (по ID)
     * @param entity Сущность для обновления
     * @return Обновленная сущность
     */
    ClassType update(const ClassType& entity);

    /**
     * @brief Обновляет сущности по условию
     * @param entity Сущность с новыми значениями
     * @param condition Условие WHERE
     * @return Обновленная сущность (первая из обновленных)
     */
    ClassType update_where(const ClassType& entity, const query_craft::ast::expression& condition);

    /**
     * @brief Удаляет несколько сущностей в базе данных (batch delete) из итераторов.
     * @param begin Начало итератора.
     * @param end Конец итератора.
     * @return true, если все сущности были удалены
     */
    template<typename Begin, typename End>
    bool remove_batch(Begin begin, End end);

    /**
     * @brief Удаляет сущность из базы данных (по ID)
     * @param entity Сущность для удаления
     * @return true, если сущность была удалена
     */
    bool remove(const ClassType& entity);

    /**
     * @brief Удаляет сущность по ID значению
     * @tparam IdType Тип ID
     * @param id Значение ID
     * @return true, если сущность была удалена
     */
    template<typename IdType>
    bool remove_by_id(const IdType& id);

    /**
     * @brief Удаляет сущности по условию
     * @param condition Условие WHERE
     * @return true, если хотя бы одна сущность была удалена
     */
    bool remove_where(const query_craft::ast::expression& condition);

private:
    /**
     * @brief Начинает транзакцию, если она еще не начата
     * @return true, если транзакция была начата, false если уже была активна
     */
    bool begin_transaction_if_needed() const;

    /**
     * @brief Фиксирует транзакцию, если она была начата автоматически и включен auto_commit
     * @param transaction_started Флаг, указывающий была ли начата транзакция автоматически
     */
    void commit_transaction_if_needed(bool transaction_started) const;

    /**
     * @brief Откатывает транзакцию при ошибке, если она была начата автоматически
     * @param transaction_started Флаг, указывающий была ли начата транзакция автоматически
     */
    void rollback_transaction_on_error(bool transaction_started) const;

private:
    std::shared_ptr<database_adapter::IConnection> _database;
    table<ClassType, Columns...> _dto;
    bool _auto_commit;
};

// Реализация методов storage

template<typename ClassType, typename... Columns>
storage<ClassType, Columns...>::storage(const std::shared_ptr<database_adapter::IConnection>& database, table<ClassType, Columns...> dto, const bool auto_commit)
    : _database(database)
    , _dto(std::move(dto))
    , _auto_commit(auto_commit)
{
    if(_database == nullptr) {
        throw std::invalid_argument("Connection is not valid");
    }
}

template<typename ClassType, typename... Columns> storage<ClassType, Columns...>::~storage()
{
    if(_database != nullptr && _database->is_transaction() && _auto_commit) {
        try {
            _database->commit();
        } catch(...) {
            // Игнорируем ошибки при коммите в деструкторе, чтобы не скрывать другие исключения
        }
    }
}

template<typename ClassType, typename... Columns>
ClassType storage<ClassType, Columns...>::insert(const ClassType& entity)
{
    bool transaction_started = begin_transaction_if_needed();
    try {
        auto dialect = _database->dialect();
        if(dialect == nullptr) {
            throw std::runtime_error("Unable to determine SQL dialect");
        }

        auto column_names = _dto.column_names_without_auto_increment();
        if(column_names.empty()) {
            throw std::runtime_error("No columns to insert");
        }

        query_craft::dsl::insert_builder builder;
        builder.into(_dto.table_name(), _dto.scheme())
            .columns(column_names.begin(), column_names.end());

        std::vector<query_craft::ast::expression> values;
        for(const auto& param : map_entity_to_params(_dto, entity, false)) {
            if(param == NULL_VALUE) {
                values.push_back(query_craft::dsl::value(NULL_VALUE));
            } else {
                values.push_back(query_craft::dsl::param(param));
            }
        }
        builder.values(values);

        auto auto_increment_column_names = _dto.auto_increment_column_names();
        builder.returning(auto_increment_column_names.begin(), auto_increment_column_names.end());

        auto compiled = builder.compile(dialect);
        database_adapter::query_result result;

        // Всегда используем prepared statements для защиты от SQL инъекций
        // QueryCraft генерирует SQL с плейсхолдерами и список параметров
        // Генерируем уникальное имя на основе SQL и параметров
        std::string base_name = "insert_" + _dto.table_name();
        std::string statement_name = generate_unique_statement_name(base_name, compiled.sql);
        _database->prepare(compiled.sql, statement_name);
        result = _database->exec_prepared(compiled.parameters, statement_name);

        ClassType result_entity = entity;
        if(!result.empty()) {
            auto row = result.at(0);

            _dto.for_each([&result_entity, &row](const auto& column) {
                auto it = row.find(column.name());
                if(it != row.end() && it->second != NULL_VALUE) {
                    column.from_string(result_entity, it->second);
                }
            });
        }

        commit_transaction_if_needed(transaction_started);
        return result_entity;
    } catch(...) {
        rollback_transaction_on_error(transaction_started);
        throw;
    }
}

template<typename ClassType, typename... Columns>
template<typename Begin, typename End>
std::vector<ClassType> storage<ClassType, Columns...>::insert_batch(Begin begin, End end)
{
    if(begin == end) {
        return {};
    }

    const bool transaction_started = begin_transaction_if_needed();
    try {
        std::vector<ClassType> result_entities;
        result_entities.reserve(std::distance(begin, end));

        for(auto it = begin; it != end; ++it) {
            // insert() видит, что транзакция уже начата, и не будет коммитить
            result_entities.emplace_back(insert(*it));
        }

        commit_transaction_if_needed(transaction_started);
        return result_entities;
    } catch(...) {
        rollback_transaction_on_error(transaction_started);
        throw;
    }
}

template<typename ClassType, typename... Columns>
template<typename IdType>
std::unique_ptr<ClassType> storage<ClassType, Columns...>::find_by_id(const IdType& id)
{
    auto primary_key_name = _dto.primary_key_column_name();
    if(primary_key_name.empty()) {
        throw std::runtime_error("Table has no primary key");
    }

    auto condition = create_where_condition(primary_key_name, id);
    return find_one(condition);
}

template<typename ClassType, typename... Columns>
std::vector<ClassType> storage<ClassType, Columns...>::find_all()
{
    return find_where({});
}

template<typename ClassType, typename... Columns>
std::unique_ptr<ClassType> storage<ClassType, Columns...>::find_one(const query_craft::ast::expression& condition)
{
    auto results = find_where(condition);
    if(results.empty()) {
        return nullptr;
    }
    return std::make_unique<ClassType>(std::move(results[0]));
}

template<typename ClassType, typename... Columns>
std::vector<ClassType> storage<ClassType, Columns...>::find_where(const query_craft::ast::expression& condition)
{
    auto dialect = _database->dialect();
    if(dialect == nullptr) {
        throw std::runtime_error("Unable to determine SQL dialect");
    }

    query_craft::dsl::select_builder builder;
    std::vector<query_craft::ast::expression> columns;
    for(const auto& name : _dto.columns_name()) {
        columns.push_back(query_craft::dsl::col(name, _dto.column_alias(name)));
    }

    builder.from(_dto.table_name(), _dto.scheme())
        .columns(columns)
        .where(condition);

    auto compiled = builder.compile(dialect);
    database_adapter::query_result result;

    // Используем prepared statements для защиты от SQL инъекций, если есть параметры
    if(!compiled.parameters.empty()) {
        // Генерируем уникальное имя на основе SQL и параметров
        std::string base_name = "find_where_" + _dto.table_name();
        std::string statement_name = generate_unique_statement_name(base_name, compiled.sql);
        _database->prepare(compiled.sql, statement_name);
        result = _database->exec_prepared(compiled.parameters, statement_name);
    } else {
        result = _database->exec(compiled.sql);
    }

    return map_result_to_entities(_dto, result);
}

template<typename ClassType, typename... Columns> template<typename Begin, typename End> std::vector<ClassType> storage<ClassType, Columns...>::update_batch(Begin begin, End end)
{
    if(begin == end) {
        return {};
    }

    const bool transaction_started = begin_transaction_if_needed();
    try {
        std::vector<ClassType> result_entities;
        result_entities.reserve(std::distance(begin, end));

        for(auto it = begin; it != end; ++it) {
            result_entities.emplace_back(update(*it));
        }

        commit_transaction_if_needed(transaction_started);
        return result_entities;
    } catch(...) {
        rollback_transaction_on_error(transaction_started);
        throw;
    }
    return {};
}

template<typename ClassType, typename... Columns>
ClassType storage<ClassType, Columns...>::update(const ClassType& entity)
{
    auto primary_key_name = _dto.primary_key_column_name();
    if(primary_key_name.empty()) {
        throw std::runtime_error("Table has no primary key for update");
    }

    auto condition = create_where_condition(primary_key_name, _dto.primary_key_column_value(entity));
    return update_where(entity, condition);
}

template<typename ClassType, typename... Columns>
ClassType storage<ClassType, Columns...>::update_where(const ClassType& entity, const query_craft::ast::expression& condition)
{
    bool transaction_started = begin_transaction_if_needed();
    try {
        auto dialect = _database->dialect();
        if(dialect == nullptr) {
            throw std::runtime_error("Unable to determine SQL dialect");
        }

        query_craft::dsl::update_builder builder;
        builder.table(_dto.table_name(), _dto.scheme())
            .where(condition);

        auto column_names = _dto.column_names_without_auto_increment();
        auto params = map_entity_to_params(_dto, entity, false);
        auto default_params = map_entity_to_params(_dto, _dto.empty_entity(), false);

        for(size_t i = 0; i < column_names.size() && i < params.size(); ++i) {
            if(params[i] == default_params[i]) {
                continue;
            }

            builder.set(column_names[i], query_craft::dsl::param(params[i]));
        }

        auto return_columns = _dto.columns_name();
        builder.returning(return_columns.begin(), return_columns.end());

        auto compiled = builder.compile(dialect);
        database_adapter::query_result result;

        // Всегда используем prepared statements для защиты от SQL инъекций
        // Генерируем уникальное имя на основе SQL и параметров
        const auto base_name = "update_" + _dto.table_name();
        auto statement_name = generate_unique_statement_name(base_name, compiled.sql);
        _database->prepare(compiled.sql, statement_name);
        result = _database->exec_prepared(compiled.parameters, statement_name);

        auto result_entity = _dto.empty_entity();
        if(!result.empty()) {
            result_entity = map_row_to_entity(_dto, result.at(0));
        }

        commit_transaction_if_needed(transaction_started);
        return result_entity;
    } catch(...) {
        rollback_transaction_on_error(transaction_started);
        throw;
    }
}

template<typename ClassType, typename... Columns> template<typename Begin, typename End> bool storage<ClassType, Columns...>::remove_batch(Begin begin, End end)
{
    if(begin == end) {
        return false;
    }

    const bool transaction_started = begin_transaction_if_needed();
    try {
        for(auto it = begin; it != end; ++it) {
            if(!remove(*it)) {
                rollback_transaction_on_error(transaction_started);
                return false;
            }
        }
        commit_transaction_if_needed(transaction_started);
        return true;
    } catch(...) {
        rollback_transaction_on_error(transaction_started);
        throw;
    }
    return false;
}

template<typename ClassType, typename... Columns>
bool storage<ClassType, Columns...>::remove(const ClassType& entity)
{
    auto primary_key_name = _dto.primary_key_column_name();
    if(primary_key_name.empty()) {
        throw std::runtime_error("Table has no primary key for delete");
    }

    return remove_by_id(_dto.primary_key_column_value(entity));
}

template<typename ClassType, typename... Columns>
template<typename IdType>
bool storage<ClassType, Columns...>::remove_by_id(const IdType& id)
{
    auto primary_key_name = _dto.primary_key_column_name();
    if(primary_key_name.empty()) {
        throw std::runtime_error("Table has no primary key for delete");
    }

    return remove_where(create_where_condition(primary_key_name, id));
}

template<typename ClassType, typename... Columns>
bool storage<ClassType, Columns...>::remove_where(const query_craft::ast::expression& condition)
{
    auto objects = find_where(condition);
    if(objects.empty()) {
        return false;
    }

    bool transaction_started = begin_transaction_if_needed();
    try {
        auto dialect = _database->dialect();
        if(dialect == nullptr) {
            throw std::runtime_error("Unable to determine SQL dialect");
        }

        query_craft::dsl::delete_builder builder;
        builder.from(_dto.table_name(), _dto.scheme())
            .where(condition);

        auto compiled = builder.compile(dialect);

        // Используем prepared statements для защиты от SQL инъекций, если есть параметры
        if(!compiled.parameters.empty()) {
            // Генерируем уникальное имя на основе SQL и параметров
            std::string base_name = "remove_where_" + _dto.table_name();
            std::string statement_name = generate_unique_statement_name(base_name, compiled.sql);
            _database->prepare(compiled.sql, statement_name);
            _database->exec_prepared(compiled.parameters, statement_name);
        } else {
            _database->exec(compiled.sql);
        }

        commit_transaction_if_needed(transaction_started);
        return true;
    } catch(...) {
        rollback_transaction_on_error(transaction_started);
        throw;
    }

    return false;
}

template<typename ClassType, typename... Columns>
bool storage<ClassType, Columns...>::begin_transaction_if_needed() const
{
    if(!_database->is_transaction()) {
        _database->begin_transaction();
        return true;
    }
    return false;
}

template<typename ClassType, typename... Columns>
void storage<ClassType, Columns...>::commit_transaction_if_needed(bool transaction_started) const
{
    if(transaction_started && _auto_commit) {
        _database->commit();
    }
}

template<typename ClassType, typename... Columns>
void storage<ClassType, Columns...>::rollback_transaction_on_error(bool transaction_started) const
{
    if(transaction_started) {
        try {
            _database->rollback();
        } catch(...) {
            // Игнорируем ошибки при откате, чтобы не скрывать исходное исключение
        }
    }
}

template<typename ClassType, typename... Columns>
auto make_storage(const std::shared_ptr<database_adapter::IConnection>& database, table<ClassType, Columns...> dto, const bool auto_commit = true)
{
    return storage<ClassType, Columns...>(database, std::move(dto), auto_commit);
}

} // namespace entity_craft