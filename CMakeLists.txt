cmake_minimum_required(VERSION 3.26)
project(Radmir-Crasher)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library (${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_compile_definitions(PROJECT_NAME="Radmir Crasher")
add_compile_definitions(PROJECT_NAME_STRICT="Radmir-Crasher")
add_compile_definitions(PROJECT_NAME_STRICT_LOWER="radmir-crasher")
add_compile_definitions(PROJECT_NAME_SHORT="rchrasher")
add_compile_definitions(PROJECT_VERSION="${PROJECT_VERSION}")
add_compile_definitions(_USE_MATH_DEFINES)

set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}
        SUFFIX ".asi"
)

find_package(PluginSDK REQUIRED gtasa)

include(FindDirectX9)
include(FindRakHook)

add_subdirectory ("3rdparty/SAMP-API-multiver")
add_subdirectory ("3rdparty/kthook")
add_subdirectory ("3rdparty/renderer")

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        DirectX9
        PluginSDK::gtasa
        RakHook
        sampapi
        kthook
        renderer
)