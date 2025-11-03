cmake_minimum_required(VERSION 4.0.1)
project(AbyssFramework)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "OFF")

set(SOURCES
    Source/Private/Log.cpp
)
set(HEADERS
    Source/Public/AbyssFramework/containers/BiMap.hpp
    Source/Public/AbyssFramework/Log.h
    Source/Public/AbyssFramework/Macros.h
    Source/Public/AbyssFramework/Types.h
)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(
    ${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Source
    ${CMAKE_CURRENT_LIST_DIR}/Source/Public
    ${CMAKE_CURRENT_LIST_DIR}/Source/Public/AbyssFramework
)

source_group("AbyssFramework/Public"  FILES ${HEADERS})
source_group("AbyssFramework/Private" FILES ${SOURCES})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        # MSVC debug flags
        target_compile_options(${PROJECT_NAME} PRIVATE /Zc:preprocessor /Zi /Od /MDd)
        target_link_options(${PROJECT_NAME} PRIVATE /DEBUG)
    else()
        # Clang/GCC debug flags
        target_compile_options(${PROJECT_NAME} PRIVATE -g -O0 -Wall -Wextra -fno-omit-frame-pointer)
    endif()
else()
    if(MSVC)
        # MSVC release flags
        target_compile_options(${PROJECT_NAME} PRIVATE /Zc:preprocessor)
        target_link_options(${PROJECT_NAME} PRIVATE /DEBUG)
    endif()
endif()

add_executable(${PROJECT_NAME}Test ${CMAKE_CURRENT_LIST_DIR}/main.cpp)
target_link_libraries(${PROJECT_NAME}Test PUBLIC ${PROJECT_NAME})
target_compile_options(${PROJECT_NAME}Test PRIVATE /Zc:preprocessor)
target_include_directories(${PROJECT_NAME}Test PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Source/Public)