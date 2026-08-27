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
    src/IMGUIExt.h

    src/Widgets/TextDiff.h
    src/Widgets/TextDiff.cpp

    src/Widgets/TextEditor.h
    src/Widgets/TextEditor.cpp

    src/Widgets/dtl.h
)

# Include Luau pls
target_link_libraries(VorliasEditor PUBLIC AndromedaEngine)
