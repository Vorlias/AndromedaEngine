add_library(IMGUI)
target_sources(IMGUI PUBLIC 
    imgui/imconfig.h
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_internal.h
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/imgui.h
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
    imgui/imgui_impl_vulkan.h
    imgui/imgui_impl_vulkan.cpp
    imgui/imgui_impl_sdl3.h
    imgui/imgui_impl_sdl3.cpp
)
target_include_directories(IMGUI PUBLIC .)