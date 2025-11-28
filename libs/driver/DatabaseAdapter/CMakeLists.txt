project(DatabaseAdapter LANGUAGES CXX)

# Заголовочные файлы
set(HEADER_FILES
    include/DatabaseAdapter/connectionpool.hpp
    include/DatabaseAdapter/databaseadapter.hpp
    include/DatabaseAdapter/iconnection.hpp
    include/DatabaseAdapter/ilogger.hpp
    include/DatabaseAdapter/transaction_guard.hpp
    include/DatabaseAdapter/transaction_isolation.hpp
    include/DatabaseAdapter/exception/opendatabaseexception.hpp
    include/DatabaseAdapter/exception/sqlexception.hpp
    include/DatabaseAdapter/model/connection_entry.hpp
    include/DatabaseAdapter/model/databasesettings.hpp
    include/DatabaseAdapter/model/queryresult.hpp
)

# Исходные файлы
set(SOURCE_FILES
    src/connectionpool.cpp
    src/iconnection.cpp
    src/transaction_guard.cpp
    src/exception/opendatabaseexception.cpp
    src/exception/sqlexception.cpp
    src/model/connection_entry.cpp
    src/model/queryresult.cpp
)

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        TypeConverterApi
)

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)

install(DIRECTORY include
        DESTINATION include
)