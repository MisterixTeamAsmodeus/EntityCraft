cmake_minimum_required(VERSION 3.10)

project(EntityCraft_Example LANGUAGES CXX)

file(GLOB_RECURSE EXAMPLE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

foreach(EXAMPLE_SOURCE_FILE ${EXAMPLE_SOURCE_FILES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE_FILE} NAME_WE)

    set(EXAMPLE_TARGET "EntityCraft_${EXAMPLE_NAME}")

    add_executable(${EXAMPLE_TARGET} ${EXAMPLE_SOURCE_FILE})

    target_link_libraries(${EXAMPLE_TARGET} PRIVATE EntityCraft SqliteAdapter)

    add_custom_command(TARGET ${EXAMPLE_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "POST_BUILD COPY LIB FROM $<TARGET_FILE:SqliteAdapter> TO $<TARGET_FILE_DIR:${EXAMPLE_TARGET}>"
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SqliteAdapter> $<TARGET_FILE_DIR:${EXAMPLE_TARGET}>
    )

    if(WIN32)
        add_custom_command(TARGET ${EXAMPLE_TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE_DIR:SqliteAdapter>/sqlite3.dll $<TARGET_FILE_DIR:${EXAMPLE_TARGET}>
                COMMENT "Copying sqlite3.dll to example output directory"
        )
    endif ()

    # Копируем libpq только на Windows (на Linux библиотеки доступны системно)
    if (WIN32)
        add_custom_command(TARGET ${EXAMPLE_TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE_DIR:PostgreAdapter>/libcrypto-3-x64.dll $<TARGET_FILE_DIR:${EXAMPLE_TARGET}>
                COMMENT "Copying libcrypto-3.dll to example output directory"
        )
        add_custom_command(TARGET ${EXAMPLE_TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE_DIR:PostgreAdapter>/LIBPQ.dll $<TARGET_FILE_DIR:${EXAMPLE_TARGET}>
                COMMENT "Copying libpq.dll to example output directory"
        )
        add_custom_command(TARGET ${EXAMPLE_TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE_DIR:PostgreAdapter>/libssl-3-x64.dll $<TARGET_FILE_DIR:${EXAMPLE_TARGET}>
                COMMENT "Copying libssl-3.dll to example output directory"
        )
    endif ()
endforeach()