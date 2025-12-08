project(EntityCraft LANGUAGES CXX)

# Header-only библиотека
add_library(${PROJECT_NAME} INTERFACE)

# Добавляем заголовочные файлы как источники интерфейса
file(GLOB_RECURSE HEADER_FILES include/*.h include/*.hpp)
target_sources(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${HEADER_FILES}>
    $<INSTALL_INTERFACE:include>
)

# Настройка путей включения
target_include_directories(${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs>
        $<INSTALL_INTERFACE:libs>
)

target_include_directories(${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Связывание с зависимостями
target_link_libraries(${PROJECT_NAME} INTERFACE
        QueryCraft
        ReflectionApi
        DatabaseAdapter
)

# Установка интерфейсной библиотеки
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

# Установка заголовочных файлов
install(DIRECTORY include
        DESTINATION include
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

if (${ENTITY_CRAFT_ENABLE_SQLITE})
    target_link_libraries(${PROJECT_NAME} INTERFACE
            SqliteAdapter
    )
endif ()

if (${ENTITY_CRAFT_ENABLE_POSTGRE})
    target_link_libraries(${PROJECT_NAME} INTERFACE
            PostgreAdapter
    )
endif ()