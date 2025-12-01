cmake_minimum_required(VERSION 3.10)

project(SqliteAdapter_Example LANGUAGES CXX)

file(GLOB_RECURSE EXAMPLE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

foreach(EXAMPLE_SOURCE_FILE ${EXAMPLE_SOURCE_FILES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE_FILE} NAME_WE)

    set(EXAMPLE_TARGET "SqliteAdapter_${EXAMPLE_NAME}")

    add_executable(${EXAMPLE_TARGET} ${EXAMPLE_SOURCE_FILE})

    target_link_libraries(${EXAMPLE_TARGET} PRIVATE SqliteAdapter)

    add_custom_command(TARGET ${EXAMPLE_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SqliteAdapter> $<TARGET_FILE_DIR:${EXAMPLE_TARGET}>
            COMMENT "Copying SqliteAdapter to example output directory"
    )

    if(WIN32)
        add_custom_command(TARGET ${EXAMPLE_TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE_DIR:SqliteAdapter>/sqlite3.dll $<TARGET_FILE_DIR:${EXAMPLE_TARGET}>
                COMMENT "Copying sqlite3.dll to example output directory"
        )
    endif ()
endforeach()