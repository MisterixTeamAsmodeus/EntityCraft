#pragma once

#include "postgreadapter_global.h"

#include <DatabaseAdapter/itransaction.h>

class POSTGRE_EXPORT database_adapter::ITransaction;

namespace database_adapter {
namespace postgre {

class POSTGRE_EXPORT transaction final : public ITransaction
{
public:
    explicit transaction(std::shared_ptr<IConnection> connection);
    ~transaction() override = default;

    void open_transaction(int type) override;
};

} // namespace postgre
} // namespace database_adapter