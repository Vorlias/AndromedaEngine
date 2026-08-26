include(FetchContent)


add_executable(VorliasEditor)

target_sources(
    VorliasEditor
    PRIVATE
    src/main.cpp
    src/EditorPanels/SceneHeirarchy.h
    src/EditorPanels/SceneHeirarchy.cpp
    src/Widgets/IconsLucide.h
    src/Widgets/IconsCodicons.h
    src/Widgets/IconsMDI.h
)

# Include Luau pls
target_link_libraries(VorliasEditor PUBLIC AndromedaEngine)
