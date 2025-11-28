cmake_minimum_required(VERSION 3.8)

project(ReflectionApi LANGUAGES CXX)

# Header-only library - явное перечисление заголовочных файлов
set(HEADER_FILES
    include/ReflectionApi/entity.hpp
    include/ReflectionApi/property.hpp
    include/ReflectionApi/referenceproperty.hpp
    include/ReflectionApi/reflectionapi.hpp
    include/ReflectionApi/helper/templates.hpp
    include/ReflectionApi/helper/tuplealgoritm.hpp
    include/ReflectionApi/visitor/anypropertyvisitor.hpp
    include/ReflectionApi/visitor/propertyvisitor.hpp
    include/ReflectionApi/visitor/referencepropertyvisitor.hpp
)

# Создаем интерфейсную библиотеку (header-only)
add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME} INTERFACE
        TypeConverterApi
)

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}
)
