#pragma once

#include "sqliteadapter_global.h"
#include "sqliteconnection.h"

#include <DatabaseAdapter/connectionpool.h>

namespace database_adapter {
namespace sqlite {

class SQLITE_EXPORT connection_pool : public database_adapter::connection_pool<connection>
{
};

} // namespace sqlite
} // namespace database_adapter