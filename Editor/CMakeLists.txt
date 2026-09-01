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
    src/EditorPanels/Console.h
    src/EditorPanels/Console.cpp
    src/EditorPanels/Inspector.h
    src/EditorPanels/Inspector.cpp
    src/Widgets/Widgets.h
    src/Widgets/Widgets.cpp
    src/Assets/Assets.h

    src/crossguid/guid.hpp
    src/crossguid/guid.cpp
    src/Assets/FileWatcher.h
    src/Assets/Importers.h
    src/Assets/LuauScriptImporter.h
    src/Settings/UserSettings.h
    src/Settings/toml.hpp
    src/Settings/UserSettings.cpp
    src/ArgParse.h
    src/EditorApplication.h
    src/EditorApplication.cpp
    src/EditorPanels/SceneView.h
    src/EditorPanels/SceneView.cpp
)

if (WIN32)
    target_compile_definitions(VorliasEditor PRIVATE GUID_WINDOWS)
elseif(APPLE)
    target_compile_definitions(VorliasEditor PRIVATE GUID_CFUUID)
else()
    # find_package(libuuid REQUIRED)
    # if (NOT LIBUUID_FOUND)
    #     message(FATAL_ERROR
    #         "You might need to run 'sudo apt-get install uuid-dev' or similar")
    # endif()
    # target_include_directories(crossguid PRIVATE ${LIBUUID_INCLUDE_DIR})
    # target_link_libraries(crossguid ${LIBUUID_LIBRARY})
    # target_compile_definitions(crossguid PRIVATE GUID_LIBUUID)
endif()

# Include Luau pls
target_link_libraries(VorliasEditor PUBLIC AndromedaEngine)