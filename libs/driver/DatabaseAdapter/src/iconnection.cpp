#include "DatabaseAdapter/iconnection.h"

#include "DatabaseAdapter/exception/sqlexception.h"

namespace database_adapter {

IConnection::IConnection(const database_connection_settings& /*settings*/)
{
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
    return _has_transaction;
}

bool IConnection::open_base_transaction()
{
    return open_transaction(-1);
}

void IConnection::commit()
{
    exec("COMMIT;");
    _has_transaction = false;
}

void IConnection::add_save_point(const std::string& save_point)
{
    if(save_point.empty())
        return;

    exec("SAVEPOINT " + save_point);
}

void IConnection::rollback_to_save_point(const std::string& save_point)
{
    exec(save_point.empty() ? "ROLLBACK;" : "ROLLBACK TO " + save_point);

    if(save_point.empty()) {
        _has_transaction = false;
    }
}

void IConnection::rollback()
{
    rollback_to_save_point("");
}

} // namespace database_adapter