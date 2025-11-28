cmake_minimum_required(VERSION 3.8)

project(EnityCraft_Test LANGUAGES CXX)

enable_testing()

file(GLOB_RECURSE SOURCE_FILES *.cpp)

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
    ${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        GTest::gtest_main    # Google Test + main()
        ReflectionApi
        TypeConverterApi
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})  # Автоматическая регистрация тестов

add_test(NAME ${PROJECT_NAME} COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})