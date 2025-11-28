cmake_minimum_required(VERSION 3.10)

project(EntityCraftOrm LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)

option(ENTITY_CRAFT_EXAMPLE "Add EntityCraft examples file as executable (on|off)" OFF )
option(ENTITY_CRAFT_TEST "Включить тесты для EntityCraft" OFF)
option(ENTITY_CRAFT_ENABLE_SQLITE "Включить драйвер для Sqlite3" OFF)
option(ENTITY_CRAFT_ENABLE_POSTGRE "Включить драйвер для PostgreSql" OFF)

if (${ENTITY_CRAFT_ENABLE_SQLITE})
    add_definitions(-DENABLE_SQLITE)
    add_subdirectory(libs/driver/SqliteAdapter)
endif ()

if (${ENTITY_CRAFT_ENABLE_POSTGRE})
    add_definitions(-DENABLE_POSTGRE)
    add_subdirectory(libs/driver/PostgreAdapter)
endif ()

if(${ENTITY_CRAFT_EXAMPLE})
    if (NOT TARGET SqliteAdapter)
        add_definitions(-DENABLE_SQLITE)
        add_subdirectory(libs/driver/SqliteAdapter)
    endif ()

    add_subdirectory(example)
endif ()

if(${ENTITY_CRAFT_TEST})
    add_subdirectory(tests)
endif ()

add_subdirectory(libs/ReflectionApi)
add_subdirectory(libs/QueryCraft)
add_subdirectory(libs/driver/DatabaseAdapter)
add_subdirectory(libs/TypeConverterApi)

add_subdirectory(EntityCraft)