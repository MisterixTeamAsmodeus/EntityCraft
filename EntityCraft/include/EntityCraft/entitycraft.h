#pragma once

#include "column.h"
#include "referencecolumn.h"
#include "storage.h"
#include "table.h"

#include <DatabaseAdapter/databaseadapter.h>
#include <ReflectionApi/reflectionapi.h>
#include <QueryCraft/querycraft.h>

#ifdef ENABLE_SQLITE
#    include <SqliteAdapter/sqliteadapter.hpp>
#endif

#ifdef ENABLE_POSTGRE
#    include <PostgreAdapter/postgreadapter.hpp>
#endif