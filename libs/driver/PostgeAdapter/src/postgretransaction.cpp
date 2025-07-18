#include "PostgreAdapter/postgretransaction.h"

#include "PostgreAdapter/postgretransactiontype.h"

#include <DatabaseAdapter/iconnection.h>

#include <utility>

namespace database_adapter {
namespace postgre {
transaction::transaction(std::shared_ptr<IConnection> connection)
    : ITransaction(std::move(connection))
{
}

void transaction::open_transaction(int type)
{
    const auto sql = [&type]() {
        switch(static_cast<transaction_type>(type)) {
            case transaction_type::READ_UNCOMMITTED:
                return "BEGIN TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;";
            case transaction_type::READ_COMMITTED:
                return "BEGIN TRANSACTION ISOLATION LEVEL READ COMMITTED;";
            case transaction_type::REPEATABLE_READ:
                return "BEGIN TRANSACTION ISOLATION LEVEL REPEATABLE READ;";
            case transaction_type::SERIALIZABLE:
                return "BEGIN TRANSACTION ISOLATION LEVEL SERIALIZABLE;";
            default:
                return "BEGIN;";
        }
    }();

    _connection->exec(sql);
}

} // namespace postgre
} // namespace database_adapter