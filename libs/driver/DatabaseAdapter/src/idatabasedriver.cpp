#include "DatabaseAdapter/idatabasedriver.h"

#include "DatabaseAdapter/iconnection.h"
#include "DatabaseAdapter/model/databasesettings.h"

namespace database_adapter {

IDataBaseDriver::IDataBaseDriver(models::database_settings /*settings*/)
{
}

IDataBaseDriver::IDataBaseDriver(std::shared_ptr<IConnection> connection)
    : _connection(std::move(connection))
{
}

bool IDataBaseDriver::is_open() const
{
    return _connection != nullptr && _connection->is_valid();
}

models::query_result IDataBaseDriver::exec(const std::string& query)
{
    return _connection->exec(query);
}

std::shared_ptr<ITransaction> IDataBaseDriver::open_base_transaction() const
{
    return open_transaction(-1);
}

void IDataBaseDriver::prepare(const std::string& query, const std::string& name)
{
    _connection->prepare(query, name);
}

models::query_result IDataBaseDriver::exec_prepared(const std::vector<std::string>& params, const std::string& name)
{
    return _connection->exec_prepared(params, name);
}

} // namespace database_adapter
