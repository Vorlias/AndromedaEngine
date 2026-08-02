cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

# Have to use this so all the code is using the "-fPIC" thing
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_REVIS 2)

# Add Luau as a dependency here
add_subdirectory(Luau)
add_subdirectory(External/spdlog)
add_subdirectory(External/entt)
# add_subdirectory(External/cpptrace)


# The Andromeda Engine Project
project(AndromedaEngine VERSION 0.0.5 LANGUAGES CXX)

add_compile_definitions(ANDROMEDA_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_compile_definitions(LUAU_VERSION=730)
add_compile_definitions(ANDROMEDA_VERSION_MINOR=${PROJECT_VERSION_MINOR})
add_compile_definitions(ANDROMEDA_VERSION_PATCH=${PROJECT_VERSION_PATCH})
add_compile_definitions(ANDROMEDA_VERSION_STRING="Andromeda v${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

add_compile_definitions(ANDROMEDA_BUILD_TYPE="${CMAKE_BUILD_TYPE}")
if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_definitions(ANDROMEDA_DEBUG=1)
else()
    add_compile_definitions(ANDROMEDA_RELEASE=1)
endif()

add_subdirectory(VorliasEngine)
add_subdirectory(VorliasGame)
target_include_directories(VorliasGame PUBLIC VorliasEngine/include)