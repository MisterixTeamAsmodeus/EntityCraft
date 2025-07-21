#pragma once

#include "postgreadapter_global.h"
#include "postgreconnectionpool.h"

#include <DatabaseAdapter/databaseadapter.h>

class POSTGRE_EXPORT database_adapter::IDataBaseDriver;

namespace database_adapter {
namespace postgre {

class POSTGRE_EXPORT database_adapter final : public IDataBaseDriver
{
public:
    explicit database_adapter(const models::database_settings& settings);

    explicit database_adapter(const std::shared_ptr<IConnection>& connection = connection_pool::instance()->open_connection());

    ~database_adapter() override = default;
    std::shared_ptr<ITransaction> open_transaction(int type) const override;
};

} // namespace postgre
} // namespace database_adapter
