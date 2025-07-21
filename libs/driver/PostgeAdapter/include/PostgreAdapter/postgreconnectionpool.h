#pragma once

#include "postgreadapter_global.h"
#include "postgreconnection.h"

#include <DatabaseAdapter/connectionpool.h>

namespace database_adapter {
namespace postgre {

class POSTGRE_EXPORT connection_pool : public database_adapter::connection_pool<connection>
{
public:
    static settings connection_settings;

    static size_t start_pool_size;
    static size_t max_pool_size;
    static std::chrono::seconds wait_time;

    static std::shared_ptr<connection_pool> instance();

public:
    connection_pool(settings settings, size_t start_pool_size, size_t max_pool_size, std::chrono::seconds wait_time);

    connection_pool(settings settings, size_t start_pool_size, std::chrono::seconds wait_time);
};

} // namespace postgre
} // namespace database_adapter