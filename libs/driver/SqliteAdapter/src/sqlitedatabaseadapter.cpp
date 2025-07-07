#include "SqliteAdapter/sqlitedatabaseadapter.h"

#include "SqliteAdapter/sqliteconnection.h"
#include "SqliteAdapter/sqlitetransaction.h"
#include "SqliteAdapter/transactiontype.h"

namespace database_adapter {

sqlite_database_adapter::sqlite_database_adapter(const models::database_settings& settings)
    : IDataBaseDriver(std::make_shared<sqlite_connection>(settings))
{
}

sqlite_database_adapter::sqlite_database_adapter(const std::shared_ptr<sqlite_connection>& connection)
    : IDataBaseDriver(connection)
{
}

std::shared_ptr<ITransaction> sqlite_database_adapter::open_transaction(const int type) const
{
    auto transaction = std::make_shared<sqlite_transaction>(_connection);
    try {
        transaction->open_transaction(type);
        return transaction;
    } catch(...) {
        return nullptr;
    }
}

} // namespace database_adapter