include(FetchContent)


add_executable(VorliasEditor)

target_sources(
    VorliasEditor
    PRIVATE
    src/main.cpp
)

# Include Luau pls
target_link_libraries(VorliasEditor PUBLIC AndromedaEngine)
