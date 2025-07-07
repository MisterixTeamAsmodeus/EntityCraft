#include "SqliteAdapter/sqliteconnection.h"

#include <DatabaseAdapter/exception/opendatabaseexception.h>
#include <DatabaseAdapter/exception/sqlexception.h>
#include <DatabaseAdapter/idatabasedriver.h>
#include <DatabaseAdapter/model/databasesettings.h>

#include <iostream>

namespace database_adapter {
sqlite_connection::sqlite_connection(const sqlite_settings& settings)
    : IConnection(settings)
{
    connect(settings);
}

sqlite_connection::~sqlite_connection()
{
    disconnect();
}

bool sqlite_connection::is_valid()
{
    return _connection != nullptr;
}

models::query_result sqlite_connection::exec(const std::string& query)
{
    using namespace database_adapter;
    sqlite3_stmt* stmt;

    if(sqlite3_prepare_v2(_connection, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::string _last_error = "Failed to prepare statement: ";
        _last_error.append(sqlite3_errmsg(_connection));
        throw sql_exception(std::move(_last_error), query);
    }

    int rc = sqlite3_step(stmt);

    models::query_result result;
    while(rc == SQLITE_ROW) {
        models::query_result::result_row row;
        for(int i = 0; i < sqlite3_column_count(stmt); i++) {
            auto* column_name = sqlite3_column_name(stmt, i);
            auto* column_value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            auto* null_value = NULL_VALUE;

            row.emplace(column_name, column_value == nullptr ? null_value : column_value);
        }
        result.add_row(row);

        rc = sqlite3_step(stmt);
    }

    if(rc != SQLITE_DONE) {
        std::string _last_error = "Failed to execute statement: ";
        _last_error.append(sqlite3_errmsg(_connection));
        throw sql_exception(std::move(_last_error), query);
    }

    sqlite3_finalize(stmt);

    return result;
}

void sqlite_connection::connect(const models::database_settings& settings)
{
    if(sqlite3_open_v2(settings.url.c_str(), &_connection, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, nullptr) != SQLITE_OK) {
        std::string _last_error = "Can't open database: ";
        _last_error.append(sqlite3_errmsg(_connection));
        throw open_database_exception(std::move(_last_error));
    }
}

void sqlite_connection::disconnect() const
{
    if(_connection == nullptr)
        return;

    sqlite3_close(_connection);
}
} // namespace database_adapter