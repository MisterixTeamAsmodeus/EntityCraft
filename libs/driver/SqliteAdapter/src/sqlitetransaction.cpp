#include "SqliteAdapter/sqlitetransaction.h"

#include "DatabaseAdapter/exception/sqlexception.h"
#include "SqliteAdapter/sqliteconnection.h"
#include "SqliteAdapter/transactiontype.h"

namespace database_adapter {

sqlite_transaction::sqlite_transaction(std::shared_ptr<IConnection> connection)
    : ITransaction(std::move(connection))
{
}

void sqlite_transaction::open_transaction(int type)
{
    const auto sql = [&type]() {
        switch(type) {
            case static_cast<int>(TransactionType::DEFERRED):
                return "BEGIN DEFERRED;";
            case static_cast<int>(TransactionType::IMMEDIATE):
                return "BEGIN IMMEDIATE;";
            case static_cast<int>(TransactionType::EXCLUSIVE):
                return "BEGIN EXCLUSIVE;";
            default:
                return "BEGIN;";
        }
    }();

    _connection->exec(sql);
}

void sqlite_transaction::commit()
{
    _connection->exec("COMMIT;");
}

void sqlite_transaction::rollback_to_save_point(const std::string& save_point)
{
    const auto sql = save_point.empty() ? "ROLLBACK;" : "ROLLBACK TO " + save_point;
    _connection->exec(sql);
}

void sqlite_transaction::add_save_point(const std::string& save_point)
{
    if(save_point.empty())
        return;

    const auto sql = "SAVEPOINT " + save_point;
    _connection->exec(sql);
}

} // namespace database_adapter