#include "PostgreAdapter/postgredatabaseadapter.h"

#include "PostgreAdapter/postgreconnection.h"
#include "PostgreAdapter/postgreconnectionpool.h"
#include "PostgreAdapter/postgretransaction.h"

namespace database_adapter {
namespace postgre {

database_adapter::database_adapter(const database_connection_settings& settings)
    : IDataBaseDriver(std::make_shared<connection>(settings))
{
}

database_adapter::database_adapter(const std::shared_ptr<IConnection>& connection)
    : IDataBaseDriver(connection)
{
}

std::shared_ptr<ITransaction> database_adapter::open_transaction(const int type) const
{
    auto sqlite_transaction = std::make_shared<transaction>(_connection);
    try {
        sqlite_transaction->open_transaction(type);
        return sqlite_transaction;
    } catch(...) {
        return nullptr;
    }
}

} // namespace postgre
} // namespace database_adapter