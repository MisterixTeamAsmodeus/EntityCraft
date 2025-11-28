project(EnityCraft_Test LANGUAGES CXX)

enable_testing()

set(HEADERS_FILES ${HEADERS_FILES}
        DatabaseAdapter/mock/databaseadapter.hpp
        DatabaseAdapter/mock/mock_connection.hpp
        DatabaseAdapter/mock/mock_connection_pool.hpp
)

set(SOURCE_FILES ${SOURCE_FILES}
        ReflectionApi/entity_test.cpp
        ReflectionApi/reference_property_test.cpp
        ReflectionApi/property_test.cpp

        TypeConverterApi/containerconverter_test.cpp
        TypeConverterApi/typeconverter_containers_test.cpp
        TypeConverterApi/typeconverter_numeric_test.cpp
        TypeConverterApi/typeconverter_pointers_test.cpp
        TypeConverterApi/typeconverter_qt_test.cpp
        TypeConverterApi/typeconverter_strings_test.cpp
        TypeConverterApi/utilities_test.cpp

        DatabaseAdapter/query_result_test.cpp
        DatabaseAdapter/exception_test.cpp
        DatabaseAdapter/validation_test.cpp
        DatabaseAdapter/connection_test.cpp
        DatabaseAdapter/transaction_guard_test.cpp
        DatabaseAdapter/connectionpool_test.cpp

)

set(ENTITY_CRAFT_GOOGLETEST_MODULES_LOCATION "EXTERNAL" CACHE STRING "Location where to find googletest modules. can be [INSTALLED|EXTERNAL]")

###################################################################################################
## get googletest module in specified location

set(ENTITY_CRAFT_GOOGLETEST_MODULES_LOCATION_INSTALLED INSTALLED)
set(ENTITY_CRAFT_GOOGLETEST_MODULES_LOCATION_EXTERNAL EXTERNAL)

if(ENTITY_CRAFT_GOOGLETEST_MODULES_LOCATION STREQUAL ENTITY_CRAFT_GOOGLETEST_MODULES_LOCATION_INSTALLED)
    message("Finding googletest in location=INSTALLED")
elseif (ENTITY_CRAFT_GOOGLETEST_MODULES_LOCATION STREQUAL ENTITY_CRAFT_GOOGLETEST_MODULES_LOCATION_EXTERNAL)
    message("Finding googletest in location=EXTERNAL")
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    include(FetchContent)

    set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY "https://github.com/google/googletest.git"
            GIT_TAG "b796f7d44681514f58a683a3a71ff17c94edb0c1" #1.13.0 version
            GIT_PROGRESS ON
    )

    FetchContent_MakeAvailable(googletest)
    
    # Disable -Wcharacter-conversion warning for GoogleTest to fix compilation error
    # This is a known issue with GoogleTest and certain compilers
    # Function to apply compiler-specific flags for suppressing character conversion warnings
    function(suppress_character_conversion_warning target_name)
        # Check for Clang first (including clang-cl which sets both Clang and MSVC)
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            # Clang (including clang-cl on Windows): suppress -Wcharacter-conversion
            target_compile_options(${target_name} PRIVATE -Wno-character-conversion)
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            # GCC: suppress -Wcharacter-conversion (available in GCC 7+)
            if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 7.0)
                target_compile_options(${target_name} PRIVATE -Wno-character-conversion)
            endif()
        elseif(MSVC)
            # MSVC (native, not clang-cl): suppress warning C4838 (narrowing conversion)
            # Note: MSVC doesn't have exact equivalent, but we can suppress related warnings
            target_compile_options(${target_name} PRIVATE /wd4838)
        endif()
    endfunction()
    
    # Apply to all GoogleTest targets
    if(TARGET gtest)
        suppress_character_conversion_warning(gtest)
    endif()
    if(TARGET gtest_main)
        suppress_character_conversion_warning(gtest_main)
    endif()
    if(TARGET gmock)
        suppress_character_conversion_warning(gmock)
    endif()
    if(TARGET gmock_main)
        suppress_character_conversion_warning(gmock_main)
    endif()
else()
    message(FATAL_ERROR "Unknown location to find googletest '${ENTITY_CRAFT_GOOGLETEST_MODULES_LOCATION}'")
endif()

add_executable(
    ${PROJECT_NAME}
    ${HEADERS_FILES}
    ${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        gtest_main           # Google Test + main()
        gmock                # Google Mock
        ReflectionApi
        TypeConverterApi
        DatabaseAdapter
)

cmake_policy(SET CMP0084 NEW)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core)
if (Qt${QT_VERSION_MAJOR}_FOUND)
    target_compile_definitions(${PROJECT_NAME} INTERFACE USE_TYPE_QT)

    target_link_libraries(${PROJECT_NAME} INTERFACE
            Qt${QT_VERSION_MAJOR}::Core
    )
endif ()

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})  # Автоматическая регистрация тестов

add_test(NAME ${PROJECT_NAME} COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})