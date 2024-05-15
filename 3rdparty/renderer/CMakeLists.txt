cmake_minimum_required (VERSION 3.8)

project(renderer)

add_library (${PROJECT_NAME} "renderer.cpp")
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

#include(../FindDirectX9.cmake)
target_link_libraries(${PROJECT_NAME} PRIVATE DirectX9)

target_link_libraries(${PROJECT_NAME} INTERFACE renderer)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})