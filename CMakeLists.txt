cmake_minimum_required(VERSION 3.28)
project(Ink)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_C_COMPILE_OBJECT)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(vendor/glfw EXCLUDE_FROM_ALL)

# set(CMAKE_BUILD_TYPE Debug)

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentations")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

add_definitions(-DSHADER_DIR="${CMAKE_SOURCE_DIR}/assets/shaders/")

add_executable(Ink)

target_sources(Ink PUBLIC
    vendor/glad/src/glad.c

    source/stl/ink_memory.h

    source/renderer/buffers.h
    source/renderer/buffers.cc
    source/renderer/shader.h
    source/renderer/shader.cc
    source/renderer/textureManager.h
    source/renderer/textureManager.cc
    source/renderer/stbi.cc

    source/entities/gameObject.h
    source/entities/character.h
    source/entities/character.cc
    source/entities/platform.h
    source/entities/platform.cc
    source/entities/hitbox.h
    source/entities/hitbox.cc

    source/core/application.h
    source/core/application.cc
    source/core/entityManager.h
    source/core/entityManager.cc
    source/core/levelLoader.h
    source/core/levelLoader.cc
    source/core/entry.cc
)

target_include_directories(Ink PUBLIC
    vendor/glad/include
    vendor/glfw/include
    vendor/GLM
    vendor 
    source
)

target_link_libraries(Ink glfw)
