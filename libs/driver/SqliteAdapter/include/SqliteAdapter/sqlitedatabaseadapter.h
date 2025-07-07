#pragma once

#include <DatabaseAdapter/databaseadapter.h>

namespace database_adapter {
class sqlite_connection;
} // namespace database_adapter

namespace database_adapter {

class sqlite_database_adapter final : public IDataBaseDriver
{
public:
    explicit sqlite_database_adapter(const models::database_settings& settings);

    explicit sqlite_database_adapter(const std::shared_ptr<sqlite_connection>& connection);

    ~sqlite_database_adapter() override = default;
    std::shared_ptr<ITransaction> open_transaction(int type) const override;
};

}; // namespace database_adapter
