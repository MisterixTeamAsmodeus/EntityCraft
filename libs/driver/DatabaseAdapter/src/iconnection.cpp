#include "DatabaseAdapter/iconnection.hpp"

#include "DatabaseAdapter/exception/opendatabaseexception.hpp"
#include "DatabaseAdapter/exception/sqlexception.hpp"
#include "DatabaseAdapter/model/databasesettings.hpp"
#include "DatabaseAdapter/transaction_isolation.hpp"

#include <algorithm>
#include <cctype>

namespace {
void validate_connection_settings(const database_adapter::database_connection_settings& settings)
{
    if(settings.url.empty()) {
        throw database_adapter::open_database_exception("Неверное значение url");
    }
}

bool is_valid_savepoint_name(const std::string& save_point)
{
    if(save_point.empty()) {
        return false;
    }

    return std::all_of(save_point.begin(), save_point.end(), [](char c) {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
    });
}
} // namespace

namespace database_adapter {

IConnection::IConnection(const database_connection_settings& settings)
{
    validate_connection_settings(settings);
}

void IConnection::validate_query(const std::string& query) const
{
    if(query.empty()) {
        throw sql_exception("SQL-запрос не может быть пустым", "");
    }

    const bool is_only_whitespace = std::all_of(query.begin(), query.end(), [](char c) {
        return std::isspace(static_cast<unsigned char>(c));
    });

    if(is_only_whitespace) {
        throw sql_exception("SQL-запрос не может содержать только пробелы", query);
    }
}

bool IConnection::is_valid()
{
    try {
        exec("select 1");
        return true;
    } catch(const sql_exception&) {
        return false;
    }
}

bool IConnection::is_transaction() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _has_transaction;
}

bool IConnection::begin_transaction()
{
    _has_transaction = open_transaction(transaction_isolation_level::DEFAULT);
    return _has_transaction;
}

void IConnection::commit()
{
    std::lock_guard<std::mutex> lock(_mutex);

    if(!_has_transaction) {
        throw sql_exception("Попытка выполнить commit без активной транзакции", "COMMIT");
    }

    exec("COMMIT;");
    _has_transaction = false;
}

void IConnection::add_save_point(const std::string& save_point)
{
    if(save_point.empty()) {
        return;
    }

    std::lock_guard<std::mutex> lock(_mutex);

    if(!_has_transaction) {
        throw sql_exception("Попытка добавить savepoint без активной транзакции", "SAVEPOINT");
    }

    if(!is_valid_savepoint_name(save_point)) {
        throw sql_exception("Невалидное имя savepoint. Имя должно содержать только буквы, цифры и подчеркивания", "SAVEPOINT");
    }

    exec("SAVEPOINT " + save_point);
}

void IConnection::rollback_to_save_point(const std::string& save_point)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if(!_has_transaction) {
        throw sql_exception("Попытка выполнить rollback to savepoint без активной транзакции", "ROLLBACK");
    }

    if(save_point.empty()) {
        exec("ROLLBACK;");
        _has_transaction = false;
        return;
    }

    if(!is_valid_savepoint_name(save_point)) {
        throw sql_exception("Невалидное имя savepoint. Имя должно содержать только буквы, цифры и подчеркивания", "ROLLBACK TO");
    }

    exec("ROLLBACK TO " + save_point);
}

void IConnection::rollback()
{
    rollback_to_save_point("");
}

} // namespace database_adapter