include(FetchContent)


add_compile_definitions(ANDROMEDA_INTERNAL=1)
add_compile_definitions(ANDROMEDA_DEBUG=1)
add_executable(VorliasGame)

target_sources(
    VorliasGame
    PRIVATE
    src/main.cpp
)

# Include Luau pls
target_link_libraries(VorliasGame PUBLIC AndromedaEngine)
target_link_libraries(VorliasGame PUBLIC IMGUI)