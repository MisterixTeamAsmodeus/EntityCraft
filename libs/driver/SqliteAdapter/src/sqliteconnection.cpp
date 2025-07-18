#include "SqliteAdapter/sqliteconnection.h"

#include <DatabaseAdapter/exception/opendatabaseexception.h>
#include <DatabaseAdapter/exception/sqlexception.h>
#include <DatabaseAdapter/idatabasedriver.h>
#include <DatabaseAdapter/model/databasesettings.h>

#include <iostream>
#include <stdexcept>

namespace database_adapter {
namespace sqlite {

connection::connection(const settings& settings)
    : IConnection(settings)
{
    connect(settings);
}

connection::~connection()
{
    sqlite3_finalize(prepared);

    disconnect();
}

bool connection::is_valid()
{
    if(_connection == nullptr)
        return false;

    try {
        exec("select 1");
        return true;
    } catch(const sql_exception&) {
        return false;
    }
}

models::query_result connection::exec(const std::string& query)
{
    using namespace database_adapter;
    sqlite3_stmt* stmt;

    if(sqlite3_prepare_v2(_connection, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_finalize(stmt);

        std::string last_error = "Failed to prepare statement: ";
        last_error.append(sqlite3_errmsg(_connection));
        throw sql_exception(std::move(last_error), query);
    }

    int rc = sqlite3_step(stmt);

    models::query_result result;
    while(rc == SQLITE_ROW) {
        models::query_result::result_row row;
        for(int i = 0; i < sqlite3_column_count(stmt); i++) {
            auto* column_name = sqlite3_column_name(stmt, i);
            auto* column_value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));

            row.emplace(column_name, column_value == nullptr ? "" : column_value);
        }
        result.add_row(row);

        rc = sqlite3_step(stmt);
    }

    if(rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);

        std::string last_error = "Failed to execute statement: ";
        last_error.append(sqlite3_errmsg(_connection));
        throw sql_exception(std::move(last_error), query);
    }

    sqlite3_finalize(stmt);

    return result;
}

void connection::prepare(const std::string& query, const std::string& name)
{
    // TODO добавить поддержку кэширования подготовленных запросов
    if(prepared != nullptr) {
        sqlite3_finalize(prepared);
    }

    if(sqlite3_prepare_v2(_connection, query.c_str(), -1, &prepared, nullptr) != SQLITE_OK) {
        std::string _last_error = "Failed to prepare statement: ";
        _last_error.append(sqlite3_errmsg(_connection));
        throw sql_exception(std::move(_last_error), query);
    }
}

models::query_result connection::exec_prepared(const std::vector<std::string>& params, const std::string& name)
{
    if(prepared == nullptr) {
        throw sql_exception("Doesn't have prepared statment");
    }

    const auto size = sqlite3_bind_parameter_count(prepared);

    if(params.size() > size) {
        sqlite3_finalize(prepared);
        prepared = nullptr;
        throw std::invalid_argument("binding values more that binding parameters");
    }

    sqlite3_reset(prepared);

    int a;

    auto* null_value = NULL_VALUE;
    for(int i = 0; i < size; i++) {
        if(params[i] == null_value) {
            sqlite3_bind_null(prepared, i);
            continue;
        }

        // i + 1, так как в sqlite индекс параметров начинается с 1, а не с 0
        sqlite3_bind_text(prepared, i + 1, params[i].c_str(), params[i].size(), SQLITE_STATIC);
    }

    int rc = sqlite3_step(prepared);

    models::query_result result;
    while(rc == SQLITE_ROW) {
        models::query_result::result_row row;
        for(int i = 0; i < sqlite3_column_count(prepared); i++) {
            auto* column_name = sqlite3_column_name(prepared, i);
            auto* column_value = reinterpret_cast<const char*>(sqlite3_column_text(prepared, i));

            row.emplace(column_name, column_value == nullptr ? "" : column_value);
        }
        result.add_row(row);

        rc = sqlite3_step(prepared);
    }

    if(rc != SQLITE_DONE) {
        std::string _last_error = "Failed to execute statement: ";
        _last_error.append(sqlite3_errmsg(_connection));
        throw sql_exception(std::move(_last_error));
    }

    sqlite3_finalize(prepared);
    prepared = nullptr;

    return result;
}

void connection::connect(const models::database_settings& settings)
{
    if(sqlite3_open_v2(settings.url.c_str(), &_connection, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, nullptr) != SQLITE_OK) {
        sqlite3_close(_connection);
        _connection = nullptr;
        std::string last_error = "Can't open database: ";
        last_error.append(sqlite3_errmsg(_connection));
        throw open_database_exception(std::move(last_error));
    }
}

void connection::disconnect()
{
    if(!is_valid())
        return;

    sqlite3_close(_connection);
    _connection = nullptr;
}
} // namespace sqlite
} // namespace database_adapter