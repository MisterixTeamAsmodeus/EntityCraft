#pragma once

#include <DatabaseAdapter/itransaction.h>

namespace database_adapter {
class sqlite_connection;
} // namespace database_adapter

namespace database_adapter {

class sqlite_transaction final : public ITransaction
{
public:
    explicit sqlite_transaction(std::shared_ptr<IConnection> connection);
    ~sqlite_transaction() override = default;

    void open_transaction(int type) override;

    void commit() override;
    void add_save_point(const std::string& save_point) override;
    void rollback_to_save_point(const std::string& save_point) override;
};

} // namespace database_adapter