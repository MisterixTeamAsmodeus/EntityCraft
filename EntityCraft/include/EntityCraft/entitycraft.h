#pragma once

#include "reflection/column.h"
#include "reflection/referencecolumn.h"
#include "reflection/table.h"
#include "storage.h"

#include <QueryCraft/querycraft.h>

#ifdef ENABLE_SQLITE
#    include <SqliteAdapter/sqliteadapter.hpp>
#endif

#ifdef ENABLE_POSTGRE
#    include <PostgreAdapter/postgreadapter.hpp>
#endif