#include "DatabaseAdapter/itransaction.h"

#include "DatabaseAdapter/iconnection.h"

namespace database_adapter {

ITransaction::ITransaction(std::shared_ptr<IConnection> connection)
    : _connection(std::move(connection))
{
}

void ITransaction::commit()
{
    _connection->exec("COMMIT;");
}

void ITransaction::add_save_point(const std::string& save_point)
{
    if(save_point.empty())
        return;

    const auto sql = "SAVEPOINT " + save_point;
    _connection->exec(sql);
}

void ITransaction::rollback_to_save_point(const std::string& save_point)
{
    const auto sql = save_point.empty() ? "ROLLBACK;" : "ROLLBACK TO " + save_point;
    _connection->exec(sql);
}

void ITransaction::open_base_transaction()
{
    open_transaction(-1);
}

models::query_result ITransaction::exec(const std::string& query)
{
    return _connection->exec(query);
}

void ITransaction::rollback()
{
    rollback_to_save_point("");
}

} // namespace database_adapter
