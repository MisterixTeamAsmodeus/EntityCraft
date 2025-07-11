cmake_minimum_required(VERSION 3.8)

project(SqliteAdapter LANGUAGES CXX)

set(ENTITYCRAFT_SQLITE3_MODULES_LOCATION "INSTALLED" CACHE STRING "Location where to find SQLite3 modules. can be [INSTALLED|CUSTOM|EXTERNAL]")

###################################################################################################
## get SQLite3 module in specified location

set(ENTITYCRAFT_SQLITE3_MODULES_LOCATION_CUSTOM CUSTOM)
set(ENTITYCRAFT_SQLITE3_MODULES_LOCATION_INSTALLED INSTALLED)
set(ENTITYCRAFT_SQLITE3_MODULES_LOCATION_EXTERNAL EXTERNAL)

if(ENTITYCRAFT_SQLITE3_MODULES_LOCATION STREQUAL ENTITYCRAFT_SQLITE3_MODULES_LOCATION_CUSTOM)
    message("Finding SQLite3 in location=CUSTOM")
    message("SQLite3_INCLUDE_DIRS = ${SQLite3_INCLUDE_DIRS}")
elseif(ENTITYCRAFT_SQLITE3_MODULES_LOCATION STREQUAL ENTITYCRAFT_SQLITE3_MODULES_LOCATION_INSTALLED)
    message("Finding SQLite3 in location=INSTALLED")
    find_package(SQLite3 REQUIRED)
    message("SQLite3_INCLUDE_DIRS = ${SQLite3_INCLUDE_DIRS}")

elseif (ENTITYCRAFT_SQLITE3_MODULES_LOCATION STREQUAL ENTITYCRAFT_SQLITE3_MODULES_LOCATION_EXTERNAL)
    message("Finding SQLite3 in location=EXTERNAL")
    include(FetchContent)

    set(FETCHCONTENT_QUIET OFF)

    FetchContent_Declare(
            SQLite
            GIT_REPOSITORY "https://github.com/azadkuh/sqlite-amalgamation.git"
            GIT_TAG "15d0ff10ebc7e7225eced1de84bb52137000899b" # origin/master at Apr 1, 2022
            GIT_PROGRESS ON
    )
    FetchContent_Populate(SQLite)

    set(SQLite3_INCLUDE_DIRS ${sqlite_SOURCE_DIR})
    message("SQLite3_INCLUDE_DIRS = ${SQLite3_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "Unknown location to find SQLite3 '${ENTITYCRAFT_SQLITE3_MODULES_LOCATION}'")
endif()

file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${SQLite3_INCLUDE_DIRS}
)


target_link_libraries(${PROJECT_NAME} PUBLIC
        DatabaseAdapter
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        SQLite3
)

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)

install(DIRECTORY include/
        DESTINATION include
)
