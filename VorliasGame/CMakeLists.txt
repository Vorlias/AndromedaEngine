include(FetchContent)


add_compile_definitions(ANDROMEDA_INTERNAL=1)
add_compile_definitions(ANDROMEDA_DEBUG=1)
add_executable(VorliasGame)

if (WEBGPU_RENDERER)
    add_compile_definitions(ANDROMEDA_WGPU=1)
endif()

target_sources(
    VorliasGame
    PRIVATE
    src/main.cpp
    src/VulkanPlayground.h
    src/VulkanPlayground.cpp
    src/LuauTesting.h
)

# Include Luau pls
target_link_libraries(VorliasGame PUBLIC AndromedaEngine)
target_link_libraries(VorliasGame PUBLIC IMGUI)