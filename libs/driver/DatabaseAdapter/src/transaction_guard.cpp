#include "DatabaseAdapter/transaction_guard.hpp"

#include "DatabaseAdapter/exception/opendatabaseexception.hpp"
#include "DatabaseAdapter/exception/sqlexception.hpp"
#include "DatabaseAdapter/transaction_isolation.hpp"

namespace database_adapter {

transaction_guard::transaction_guard(std::shared_ptr<IConnection> connection)
    : _connection(std::move(connection))
{
    if(!_connection) {
        throw open_database_exception("Соединение с базой данных не может быть nullptr");
    }

    if(!_connection->open_base_transaction()) {
        throw sql_exception("Не удалось открыть транзакцию", "");
    }
}


transaction_guard::transaction_guard(std::shared_ptr<IConnection> connection, const transaction_isolation_level level)
    : _connection(std::move(connection))
{
    if(!_connection) {
        throw open_database_exception("Соединение с базой данных не может быть nullptr");
    }

    if(!_connection->open_transaction(level)) {
        throw sql_exception("Не удалось открыть транзакцию", "");
    }
}

transaction_guard::~transaction_guard()
{
    if(!_finished && _connection && _connection->is_transaction()) {
        try {
            _connection->rollback();
        } catch(const std::exception&) {
            // Игнорируем исключения в деструкторе
        }
    }
}

void transaction_guard::commit()
{
    if(_finished) {
        throw sql_exception("Транзакция уже завершена", "COMMIT");
    }

    if(!_connection) {
        throw sql_exception("Соединение с базой данных недействительно", "COMMIT");
    }

    _connection->commit();
    _finished = true;
}

void transaction_guard::rollback()
{
    if(_finished) {
        throw sql_exception("Транзакция уже завершена", "ROLLBACK");
    }

    if(!_connection) {
        throw sql_exception("Соединение с базой данных недействительно", "ROLLBACK");
    }

    _connection->rollback();
    _finished = true;
}

bool transaction_guard::is_finished() const
{
    return _finished;
}

std::shared_ptr<IConnection> transaction_guard::get_connection() const
{
    return _connection;
}

} // namespace database_adapter

