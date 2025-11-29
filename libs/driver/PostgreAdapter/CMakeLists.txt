project(PostgreAdapter LANGUAGES CXX)

set(ENTITY_CRAFT_POSTGRE_MODULES_LOCATION "INSTALLED" CACHE STRING "Location where to find libpq modules. can be [INSTALLED|CUSTOM]")

###################################################################################################
## get libpq module in specified location

set(ENTITY_CRAFT_POSTGRE_MODULES_LOCATION_CUSTOM CUSTOM)
set(ENTITY_CRAFT_POSTGRE_MODULES_LOCATION_INSTALLED INSTALLED)

if(ENTITY_CRAFT_POSTGRE_MODULES_LOCATION STREQUAL ENTITY_CRAFT_POSTGRE_MODULES_LOCATION_CUSTOM)
    message("Finding libpq in location=CUSTOM")
    message("PostgreSQL_INCLUDE_DIRS = ${PostgreSQL_INCLUDE_DIRS}")
    set(PostgreSQL_LIBRARIES PostgreSQL)
    message("PostgreSQL_LIBRARIES = ${PostgreSQL_LIBRARIES}")
elseif(ENTITY_CRAFT_POSTGRE_MODULES_LOCATION STREQUAL ENTITY_CRAFT_POSTGRE_MODULES_LOCATION_INSTALLED)
    message("Finding libpq in location=INSTALLED")
    find_package(PostgreSQL REQUIRED)
    message("PostgreSQL_INCLUDE_DIRS = ${PostgreSQL_INCLUDE_DIRS}")
    message("PostgreSQL_LIBRARIES = ${PostgreSQL_LIBRARIES}")
else()
    message(FATAL_ERROR "Unknown location to find libpq '${ENTITYCRAFT_POSTGRE_MODULES_LOCATION}'")
endif()

set(HEADER_FILES
        include/PostgreAdapter/postgreadapter.hpp
        include/PostgreAdapter/postgreadapter_global.hpp
        include/PostgreAdapter/postgreconnection.hpp
        include/PostgreAdapter/postgreconnectionpool.hpp
        include/PostgreAdapter/postgreerrorcode.hpp
)

set(SOURCE_FILES
        src/postgreconnection.cpp
        src/postgreconnectionpool.cpp
        src/postgreerrorcode.cpp
)

add_definitions(-DPOSTGRE_LIBRARY)

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${PostgreSQL_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        DatabaseAdapter
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${PostgreSQL_LIBRARIES}
)

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)

install(DIRECTORY include/
        DESTINATION include
)
