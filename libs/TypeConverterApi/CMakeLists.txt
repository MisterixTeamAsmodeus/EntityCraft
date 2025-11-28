project(TypeConverterApi LANGUAGES CXX)

set(HEADER_FILES
    include/TypeConverterApi/helper/void_t.hpp
    include/TypeConverterApi/helper/sfinae.hpp
    include/TypeConverterApi/converter/typeconverter_base.hpp
    include/TypeConverterApi/converter/typeconverter_strings.hpp
    include/TypeConverterApi/converter/typeconverter_numeric.hpp
    include/TypeConverterApi/converter/typeconverter_pointers.hpp
    include/TypeConverterApi/converter/typeconverter_containers.hpp
    include/TypeConverterApi/converter/typeconverter_qt.hpp
    include/TypeConverterApi/typeconverter.hpp
    include/TypeConverterApi/containerconverter.hpp
    include/TypeConverterApi/utilities.hpp
    include/TypeConverterApi/typeconverterapi.hpp
)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

cmake_policy(SET CMP0084 NEW)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core)
if (Qt${QT_VERSION_MAJOR}_FOUND)
    target_compile_definitions(${PROJECT_NAME} INTERFACE USE_TYPE_QT)

    target_link_libraries(${PROJECT_NAME} INTERFACE
            Qt${QT_VERSION_MAJOR}::Core
    )
endif ()