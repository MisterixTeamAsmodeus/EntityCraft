project(SqliteAdapter LANGUAGES CXX)

set(ENTITY_CRAFT_SQLITE3_MODULES_LOCATION "INSTALLED" CACHE STRING "Location where to find SQLite3 modules. can be [INSTALLED|CUSTOM|EXTERNAL]")

###################################################################################################
## get SQLite3 module in specified location

set(ENTITY_CRAFT_SQLITE3_MODULES_LOCATION_INSTALLED INSTALLED)
set(ENTITY_CRAFT_SQLITE3_MODULES_LOCATION_CUSTOM CUSTOM)
set(ENTITY_CRAFT_SQLITE3_MODULES_LOCATION_EXTERNAL EXTERNAL)

if(ENTITY_CRAFT_SQLITE3_MODULES_LOCATION STREQUAL ENTITY_CRAFT_SQLITE3_MODULES_LOCATION_CUSTOM)
    message("Finding SQLite3 in location=CUSTOM")
    message("SQLite3_INCLUDE_DIRS = ${SQLite3_INCLUDE_DIRS}")
    set(SQLite3_LIBRARIES SQLite3)
    message("SQLite3_LIBRARIES = ${SQLite3_LIBRARIES}")
elseif (ENTITY_CRAFT_SQLITE3_MODULES_LOCATION STREQUAL ENTITY_CRAFT_SQLITE3_MODULES_LOCATION_INSTALLED)
    message("Finding SQLite3 in location=INSTALLED")

    find_package(unofficial-sqlite3 REQUIRED)
    set(SQLite3_LIBRARIES unofficial::sqlite3::sqlite3)

elseif (ENTITY_CRAFT_SQLITE3_MODULES_LOCATION STREQUAL ENTITY_CRAFT_SQLITE3_MODULES_LOCATION_EXTERNAL)
    message("Finding SQLite3 in location=EXTERNAL")
    include(FetchContent)

    set(FETCHCONTENT_QUIET OFF)

    FetchContent_Declare(
            SQLite
            GIT_REPOSITORY "https://github.com/azadkuh/sqlite-amalgamation.git"
            GIT_TAG "15d0ff10ebc7e7225eced1de84bb52137000899b" # origin/master at Apr 1, 2022
            GIT_PROGRESS ON
    )
    FetchContent_MakeAvailable(SQLite)

    set(SQLite3_INCLUDE_DIRS ${sqlite_SOURCE_DIR})
    message("SQLite3_INCLUDE_DIRS = ${SQLite3_INCLUDE_DIRS}")
    set(SQLite3_LIBRARIES SQLite3)
    message("SQLite3_LIBRARIES = ${SQLite3_LIBRARIES}")
else()
    message(FATAL_ERROR "Unknown location to find SQLite3 '${ENTITYCRAFT_SQLITE3_MODULES_LOCATION}'")
endif()

file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_definitions(-DSQLITE_LIBRARY)

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${SQLite3_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        DatabaseAdapter
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        ${SQLite3_LIBRARIES}
)

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)

install(DIRECTORY include/
        DESTINATION include
)
