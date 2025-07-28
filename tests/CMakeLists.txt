cmake_minimum_required(VERSION 3.8)

project(EnityCraft_Test LANGUAGES CXX)

enable_testing()

file(GLOB_RECURSE SOURCE_FILES *.cpp)

set(ENTITY_CRAFT_GOOGLETEST_MODULES_LOCATION "INSTALLED" CACHE STRING "Location where to find googletest modules. can be [INSTALLED|EXTERNAL]")

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
else()
    message(FATAL_ERROR "Unknown location to find googletest '${ENTITY_CRAFT_GOOGLETEST_MODULES_LOCATION}'")
endif()

add_executable(
    ${PROJECT_NAME}
    ${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    EntityCraft
    GTest::gtest_main    # Google Test + main()
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})  # Автоматическая регистрация тестов

add_test(NAME ${PROJECT_NAME} COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})