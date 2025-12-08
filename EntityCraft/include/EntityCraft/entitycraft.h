#pragma once

#include "column.h"
#include "mapper.h"
#include "referencecolumn.h"
#include "storage.h"
#include "storage_utils.h"
#include "table.h"

#include <QueryCraft/querycraft.h>

#ifdef ENABLE_SQLITE
#    include <SqliteAdapter/sqliteadapter.hpp>
#endif

#ifdef ENABLE_POSTGRE
#    include <PostgreAdapter/postgreadapter.hpp>
#endif