# ===========================
# AndromedaEngine CMakeLists
# ===========================

include(FetchContent)

find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS EGL GLX)
find_package(SDL3 REQUIRED)

# include vk-bootstrap
FetchContent_Declare(
        fetch_vk_bootstrap
        GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
        GIT_TAG        v1.4.356 #suggest using a tag so the library doesn't update whenever new commits are pushed to a branch
)
FetchContent_MakeAvailable(fetch_vk_bootstrap)

add_compile_definitions(ENGINE_VERSION_MAJOR=0)
add_compile_definitions(ENGINE_VERSION_MINOR=0)
add_compile_definitions(ENGINE_VERSION_REVIS=1)
add_compile_definitions(LUAU_VERSION=730)
add_compile_definitions(ENGINE_VERSION_STRING="Andromeda Engine v0.0.1 [No Renderer]")

add_compile_definitions(ANDROMEDA_BUILD_TYPE="${CMAKE_BUILD_TYPE}")
if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_definitions(ANDROMEDA_DEBUG=1)
else()
    add_compile_definitions(ANDROMEDA_RELEASE=1)
endif()

add_library(AndromedaEngine SHARED)

target_sources(
    AndromedaEngine # library name
    PRIVATE
    src/PCH.h
    src/Engine/Luau/LuauScript.cpp
    src/Engine/Luau/LuauState.cpp
    src/Engine/Luau/LuauState.h
    src/Engine/Engine.h
    src/Engine/Engine.cpp
    src/Engine/Log.cpp
    PUBLIC # exposed
    include/Engine/Application.h
    include/Engine/Luau/LuauScript.h
    include/Engine/Log.h
)
target_precompile_headers(AndromedaEngine PUBLIC src/PCH.h)
target_include_directories(AndromedaEngine PUBLIC src $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

# Include Luau pls
target_link_libraries(AndromedaEngine PRIVATE 
    Luau.Common Luau.Config Luau.Compiler Luau.VM # Luau 
    EnTT::EnTT # EnTT
    spdlog # Logging
    Vulkan::Vulkan vk-bootstrap::vk-bootstrap # Vulkan Libs
    OpenGL::OpenGL # Open GL Libs
    SDL3::SDL3 # SDL
)
