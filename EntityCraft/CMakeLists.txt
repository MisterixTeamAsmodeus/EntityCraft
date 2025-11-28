project(EntityCraft LANGUAGES CXX)

file(GLOB_RECURSE HEADER_FILES include/*.h)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/libs
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        QueryCraft
        ReflectionApi
        DatabaseAdapter
)

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
    target_link_libraries(${PROJECT_NAME} PUBLIC
            SqliteAdapter
    )
endif ()

if (${ENTITY_CRAFT_ENABLE_POSTGRE})
    target_link_libraries(${PROJECT_NAME} PUBLIC
            PostgreAdapter
    )
endif ()