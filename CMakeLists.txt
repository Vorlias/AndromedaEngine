cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 23)

# The Andromeda Engine Project
project(AndromedaEngine VERSION 0.0.5 LANGUAGES CXX C)

# Have to use this so all the code is using the "-fPIC" thing
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS OFF)

message(STATUS "SYSTEM_NAME" ${CMAKE_SYSTEM_NAME})
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(SDL_STATIC ON)
    set(SDL_SHARED OFF)
else()
    set(SDL_STATIC OFF)
    set(SDL_SHARED ON)
endif()

option(OPENGL_RENDERER "Include OpenGL" ON)
option(BUILTIN_SHADER_COMPILER "Build with shader compilation" ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Add Luau as a dependency here
add_subdirectory(Luau)
add_subdirectory(External/imgui)
add_subdirectory(External/spdlog)
add_subdirectory(External/entt)
add_subdirectory(External/SDL)
add_subdirectory(External/glad)
# add_subdirectory(External/cpptrace)

add_compile_definitions(ANDROMEDA_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_compile_definitions(LUAU_VERSION=730)
add_compile_definitions(ANDROMEDA_VERSION_MINOR=${PROJECT_VERSION_MINOR})
add_compile_definitions(ANDROMEDA_VERSION_PATCH=${PROJECT_VERSION_PATCH})
add_compile_definitions(ANDROMEDA_VERSION_STRING="Andromeda v${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
if (ANDROMEDA_INTERNAL)
    add_compile_definitions(ANDROMEDA_INTERNAL=1)
endif()


add_compile_definitions(ANDROMEDA_BUILD_TYPE="${CMAKE_BUILD_TYPE}")
if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_definitions(ANDROMEDA_DEBUG=1)
else()
    add_compile_definitions(ANDROMEDA_RELEASE=1)
endif()

add_subdirectory(VorliasEngine)
add_subdirectory(VorliasGame)

add_compile_definitions(ANDROMEDA_EDITOR=1)
add_subdirectory(Editor)

