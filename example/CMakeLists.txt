cmake_minimum_required(VERSION 3.10)

project("${entity_craft_project_name}Example" LANGUAGES CXX)

file(GLOB_RECURSE EXAMPLE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

foreach(EXAMPLE_SOURCE_FILE ${EXAMPLE_SOURCE_FILES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE_FILE} NAME_WE)

    set(EXAMPLE_TARGET "${entity_craft_project_name}_${EXAMPLE_NAME}")

    add_executable(${EXAMPLE_TARGET} ${EXAMPLE_SOURCE_FILE})

    target_compile_features(${EXAMPLE_TARGET} PUBLIC cxx_std_14)
    target_link_libraries(${EXAMPLE_TARGET} PRIVATE ${entity_craft_project_name} SqliteAdapter)
endforeach()