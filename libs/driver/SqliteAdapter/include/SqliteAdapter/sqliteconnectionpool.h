#pragma once

#include "sqliteadapter_global.h"
#include "sqliteconnection.h"

#include <DatabaseAdapter/connectionpool.h>

namespace database_adapter {
namespace sqlite {

class SQLITE_EXPORT connection_pool : public database_adapter::connection_pool<connection>
{
public:
    static settings connection_settings;

    static size_t start_pool_size;
    static size_t max_pool_size;
    static std::chrono::seconds wait_time;

    static std::shared_ptr<connection_pool> instance();

public:
    connection_pool(settings settings, size_t start_pool_size, size_t max_pool_size, std::chrono::seconds wait_time = std::chrono::seconds(10));

    explicit connection_pool(settings settings, size_t start_pool_size = 10, std::chrono::seconds wait_time = std::chrono::seconds(10));
};

} // namespace sqlite
} // namespace database_adapter