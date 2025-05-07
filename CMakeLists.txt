cmake_minimum_required(VERSION 3.28)
project(Ink CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(vendor/glfw)

set(CMAKE_BUILD_TYPE Debug)

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentations")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

add_executable(Ink)

target_compile_features(Ink PRIVATE cxx_std_23)

target_sources(Ink PUBLIC
    vendor/glad/src/glad.c

    source/renderer/buffers.h
    source/renderer/buffers.cc
    source/renderer/shader.h
    source/renderer/shader.cc


    source/core/application.h
    source/core/application.cc
    source/core/entry.cc
)

target_include_directories(Ink PUBLIC
    vendor/glad/include
    vendor/glfw/include
    vendor/GLM
    source
)

target_link_libraries(Ink glfw)
