cmake_minimum_required(VERSION 3.10)

project(PostgreAdapter_Example LANGUAGES CXX)

file(GLOB_RECURSE EXAMPLE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

foreach(EXAMPLE_SOURCE_FILE ${EXAMPLE_SOURCE_FILES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE_FILE} NAME_WE)

    set(EXAMPLE_TARGET "PostgreAdapter_${EXAMPLE_NAME}")

    add_executable(${EXAMPLE_TARGET} ${EXAMPLE_SOURCE_FILE})

    target_link_libraries(${EXAMPLE_TARGET} PRIVATE PostgreAdapter)

    # Копируем PostgreAdapter библиотеку (DLL на Windows, .so на Linux)
    add_custom_command(TARGET ${EXAMPLE_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:PostgreAdapter> $<TARGET_FILE_DIR:${EXAMPLE_TARGET}>
            COMMENT "Copying PostgreAdapter to example output directory"
    )

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