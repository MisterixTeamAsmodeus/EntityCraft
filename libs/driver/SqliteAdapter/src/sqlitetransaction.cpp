#include "SqliteAdapter/sqlitetransaction.h"

#include "DatabaseAdapter/exception/sqlexception.h"
#include "SqliteAdapter/sqliteconnection.h"
#include "SqliteAdapter/sqlitetransactiontype.h"

namespace database_adapter {
namespace sqlite {
transaction::transaction(std::shared_ptr<IConnection> connection)
    : ITransaction(std::move(connection))
{
}

void transaction::open_transaction(int type)
{
    const auto sql = [&type]() {
        switch(static_cast<transaction_type>(type)) {
            case transaction_type::DEFERRED:
                return "BEGIN DEFERRED;";
            case transaction_type::IMMEDIATE:
                return "BEGIN IMMEDIATE;";
            case transaction_type::EXCLUSIVE:
                return "BEGIN EXCLUSIVE;";
            default:
                return "BEGIN;";
        }
    }();

    _connection->exec(sql);
}

} // namespace sqlite
} // namespace database_adapter