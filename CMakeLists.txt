cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

# Have to use this so all the code is using the "-fPIC" thing
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(ENGINE_DIR VorliasEngine)

# Add Luau as a dependency here
add_subdirectory(Luau)
add_subdirectory(External/spdlog)
add_subdirectory(External/entt)
# add_subdirectory(External/cpptrace)

project(AndromedaEngine VERSION 0.1 LANGUAGES CXX)

add_subdirectory(VorliasEngine)

add_subdirectory(VorliasGame)
target_include_directories(VorliasGame PUBLIC VorliasEngine/include)