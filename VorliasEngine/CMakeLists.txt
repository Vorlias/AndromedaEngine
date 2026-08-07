# ===========================
# AndromedaEngine CMakeLists
# ===========================

include(FetchContent)
message(STATUS "Include OPENGL " ${OPENGL_RENDERER})

find_package(Vulkan REQUIRED COMPONENTS volk shaderc_combined)
find_package(glslang REQUIRED)
find_package(SPIRV-Tools REQUIRED)

if(OPENGL_RENDERER)
    find_package(OpenGL REQUIRED COMPONENTS EGL GLX)
    add_compile_definitions(ANDROMEDA_OPENGL=1)
endif()



# include vk-bootstrap
FetchContent_Declare(
        fetch_vk_bootstrap
        GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
        GIT_TAG        v1.4.356 #suggest using a tag so the library doesn't update whenever new commits are pushed to a branch
)
FetchContent_MakeAvailable(fetch_vk_bootstrap)

add_compile_definitions(ANDROMEDA_INTERNAL=1)
add_compile_definitions(ANDROMEDA_EXPERIMENTAL=0)
add_compile_definitions(ANDROMEDA_EDITOR=0)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_program(LSB_RELEASE_EXEC lsb_release)
    execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
        OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    message(STATUS "Compiling on Linux distro: " ${LSB_RELEASE_ID_SHORT})
endif()

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_REVIS 5)
add_library(AndromedaEngine STATIC)
# add_dependencies(AndromedaEngine shaders)

target_sources(
    AndromedaEngine # library name
    PRIVATE
    src/PCH.h
    src/Engine/Luau/LuauScript.cpp
    src/Engine/Luau/LuauState.cpp
    src/Engine/Luau/LuauState.h
    src/Engine/Engine.cpp
    src/Engine/Log.cpp
    src/Engine/Platform.cpp
    src/Engine/Window.cpp
    src/Engine/Graphics/Vulkan/VulkanRendererAPI.cpp
    src/Engine/Graphics/Vulkan/VulkanWindowContext.cpp
    src/Engine/Graphics/Vulkan/VulkanInstance.cpp
    src/Engine/Graphics/Vulkan/VulkanShader.cpp
    src/Engine/Graphics/Shader.cpp
    src/Engine/File.cpp
    include/Engine/Graphics/Vulkan/VulkanWindowContext.h
    include/Engine/Graphics/Vulkan/VulkanRendererAPI.h
    include/Engine/Graphics/Vulkan/VulkanInstance.h
    include/Engine/Graphics/Vulkan/VulkanShader.h
    src/Engine/Data/Vector.cpp
    src/Engine/Time.cpp
    src/Engine/Application.cpp
    src/Engine/IMGUI.cpp
    PUBLIC # exposed
    include/Engine/Engine.h
    include/Engine/Platform.h
    include/Engine/Window.h
    include/Engine/FileDialogs.h
    include/Engine/Application.h
    include/Engine/Luau/LuauScript.h
    include/Engine/Log.h
    include/Engine/Graphics/GraphicsContext.h
    include/Engine/Common.h
    include/Engine/Graphics/RendererAPI.h
    include/Engine/Data/Color.h
    include/Engine/Graphics/Shader.h
    include/Engine/File.h
    include/Engine/Memory.h
    include/Engine/Data/Vector.h
    include/Engine/Time.h
    include/Engine/DateTime.h
    include/Engine/IMGUI.h
)
target_precompile_headers(AndromedaEngine PUBLIC src/PCH.h)
target_include_directories(AndromedaEngine PUBLIC src $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

# Include Luau pls
target_link_libraries(AndromedaEngine PUBLIC 
    Luau.Common Luau.Config Luau.Compiler Luau.VM # Luau 
    EnTT::EnTT # EnTT
    spdlog # Logging
    Vulkan::volk vk-bootstrap::vk-bootstrap Vulkan::shaderc_combined # Vulkan Libs
    SDL3::SDL3 # SDL
    IMGUI
)

if(OPENGL_RENDERER)
    target_link_libraries(AndromedaEngine PRIVATE 
        OpenGL::OpenGL # Open GL Libs
    )
endif()

# Include the SPIRV tools
if (BUILTIN_SHADER_COMPILER AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_definitions(ANDROMEDA_SHADER_COMPILATION=1)
    target_link_libraries(
        AndromedaEngine INTERFACE
        glslang::glslang
        SPIRV-Tools-opt
        SPIRV-Tools
    )
    message(STATUS "Including Built-in shader compiler")
endif()

# Precompile shaders
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/Shaders)
    file(GLOB SHADERS ${SHADER_DIR}/*.vert ${SHADER_DIR}/*.frag)
    foreach(SHADER IN LISTS SHADERS)
        get_filename_component(FILENAME ${SHADER} NAME)
        add_custom_command(OUTPUT ${SHADER_DIR}/${FILENAME}.spv
            COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} -o ${SHADER_DIR}/${FILENAME}.spv
            DEPENDS ${SHADER}
            COMMENT "Compiling ${FILENAME}")
        list(APPEND SPV_SHADERS ${SHADER_DIR}/${FILENAME}.spv)
    endforeach()

    add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})
    #message(STATUS "SPV SHADERS: " ${SPV_SHADERS})
endif()