cmake_minimum_required(VERSION 3.10)

set(entity_craft_project_name EntityCraft)
option(ENTITY_CRAFT_EXAMPLE "Add EntityCraft examples file as executable (on|off)" OFF )

project(${entity_craft_project_name} LANGUAGES CXX)

file(GLOB_RECURSE HEADER_FILES include/*.h)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_subdirectory(external/SqliteAdapter)

add_subdirectory(include/external/DatabaseAdapter)
add_subdirectory(include/external/ReflectionApi)
add_subdirectory(include/external/QueryCraft)

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
        include/external
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        QueryCraft
        ReflectionApi
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}
)

if(${WIN32})
    set_target_properties(${PROJECT_NAME}
            PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
endif ()

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

if(${ENTITY_CRAFT_EXAMPLE})
    add_subdirectory(example)
endif ()