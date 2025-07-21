#pragma once

#include "sqliteadapter_global.h"
#include "sqliteconnectionpool.h"

#include <DatabaseAdapter/itransaction.h>

class SQLITE_EXPORT database_adapter::ITransaction;

namespace database_adapter {
namespace sqlite {

class SQLITE_EXPORT transaction final : public ITransaction
{
public:
    explicit transaction(std::shared_ptr<IConnection> connection = connection_pool::instance()->open_connection());
    ~transaction() override = default;

    void open_transaction(int type) override;
};

} // namespace sqlite
} // namespace database_adapter