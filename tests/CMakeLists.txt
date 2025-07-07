cmake_minimum_required(VERSION 3.10)

project(EnityCraft_Test LANGUAGES CXX)

enable_testing()

file(GLOB_RECURSE SOURCE_FILES *.cpp)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

if (NOT TARGET GTest::gtest_main)
    add_subdirectory(googletest)
endif ()

add_executable(
    ${PROJECT_NAME}
    ${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    TypeConverterApi
    GTest::gtest_main    # Google Test + main()
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})  # Автоматическая регистрация тестов

add_test(NAME ${PROJECT_NAME} COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME})