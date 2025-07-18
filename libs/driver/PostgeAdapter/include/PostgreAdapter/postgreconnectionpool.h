#pragma once

#include "postgreadapter_global.h"
#include "postgreconnection.h"

#include <DatabaseAdapter/connectionpool.h>

namespace database_adapter {
namespace postgre {

class POSTGRE_EXPORT connection_pool : public database_adapter::connection_pool<connection>
{
};

} // namespace postgre
} // namespace database_adapter