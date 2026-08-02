include(FetchContent)

add_executable(VorliasGame)

target_sources(
    VorliasGame
    PRIVATE
    src/main.cpp
)

# Include Luau pls
target_link_libraries(VorliasGame PRIVATE AndromedaEngine) 
