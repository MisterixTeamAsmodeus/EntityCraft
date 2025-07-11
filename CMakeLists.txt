cmake_minimum_required(VERSION 3.8)

option(ENTITY_CRAFT_EXAMPLE "Add EntityCraft examples file as executable (on|off)" OFF )
option(ENTITY_CRAFT_TEST "Включить тесты для EntityCraft" OFF)
option(ENTITY_CRAFT_QT "Включить поддержку Qt типов в библиотеки" OFF)
option(ENTITY_CRAFT_ENABLE_SQLITE "Включить драйвер для Sqlite3" OFF)

project(EntityCraft LANGUAGES CXX)

file(GLOB_RECURSE HEADER_FILES include/*.h)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_subdirectory(libs/ReflectionApi)
add_subdirectory(libs/QueryCraft)
add_subdirectory(libs/driver/DatabaseAdapter)
add_subdirectory(libs/TypeConverterApi)

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/libs
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        QueryCraft
        ReflectionApi
        DatabaseAdapter
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)

install(DIRECTORY include
        DESTINATION include
)

if (${ENTITY_CRAFT_ENABLE_SQLITE})
    add_subdirectory(libs/driver/SqliteAdapter)
endif ()

if(${ENTITY_CRAFT_EXAMPLE})
    if (NOT TARGET SqliteAdapter)
        add_subdirectory(libs/driver/SqliteAdapter)
    endif ()
    add_subdirectory(example)
endif ()

if(${ENTITY_CRAFT_TEST})
    add_subdirectory(tests)
endif ()