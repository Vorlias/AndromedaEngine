# ===========================
# AndromedaEngine CMakeLists
# ===========================

include(FetchContent)
message(STATUS "Include OPENGL " ${OPENGL_RENDERER})

find_package(Vulkan REQUIRED COMPONENTS volk shaderc_combined)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux") # need this on linux for whatever reason
    message(STATUS "Include SPIRV-Tools for Linux " ${BUILTIN_SHADER_COMPILER})

    find_package(glslang REQUIRED)
    find_package(SPIRV-Tools REQUIRED)
endif()

if(OPENGL_RENDERER)
    find_package(OpenGL REQUIRED COMPONENTS EGL GLX)
    add_compile_definitions(ANDROMEDA_OPENGL=1)
endif()

if (WEBGPU_RENDERER)
    add_compile_definitions(ANDROMEDA_WGPU=1)
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
    src/Engine/Data/Vector.cpp
    src/Engine/Time.cpp
    src/Engine/Application.cpp
    src/Engine/IMGUI.cpp
    src/Engine/Graphics/Vulkan/VulkanUtils.h
    src/Engine/Graphics/Vulkan/VulkanUtils.cpp
    src/Engine/Graphics/OpenGL/OpenGLShader.cpp
    src/Engine/Graphics/OpenGL/OpenGLRenderer.cpp
    src/Engine/Graphics/OpenGL/OpenGLWindowContext.cpp
    src/Engine/Luau/Task.cpp
    src/Engine/Luau/Lib.cpp
    src/Engine/Luau/ThreadData.cpp
    src/Engine/Graphics/Vulkan/VulkanBase.cpp
    src/Engine/Luau/PrettyPrint.cpp
    src/Engine/ObjectPool.cpp
    src/Engine/Luau/LuauRuntime.cpp
    src/Engine/Objects/Object.cpp
    src/Engine/Scene/Scene.cpp
    src/Engine/Luau/LuauTimeoutHandler.cpp
    src/Engine/Data/Vector3.luau.cpp
    src/Engine/Luau/AtomsDef.cpp
    src/Engine/Data/Vector2.luau.cpp
    src/Engine/Objects/Object.luau.cpp
    src/Engine/Graphics/WebGPU/WebGPURenderer.cpp
    src/Engine/Graphics/WebGPU/WebGPUWindowContext.cpp
    src/Engine/Asset.cpp
    src/Engine/Luau/Userdata.cpp
    src/Engine/Graphics/Vulkan/VulkanGraphicsPipeline.cpp
    PUBLIC # exposed
    include/Engine/Graphics/Vulkan/VulkanWindowContext.h
    include/Engine/Graphics/Vulkan/VulkanRendererAPI.h
    include/Engine/Graphics/Vulkan/VulkanInstance.h
    include/Engine/Graphics/Vulkan/VulkanShader.h
    include/Engine/Engine.h
    include/Engine/Platform.h
    include/Engine/Window.h
    include/Engine/FileDialogs.h
    include/Engine/Application.h
    include/Engine/Luau/LuauScript.h
    include/Engine/Luau/LuauState.h
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
    include/Engine/Graphics/OpenGL/OpenGLShader.h
    include/Engine/Graphics/OpenGL/OpenGLRenderer.h
    include/Engine/Graphics/OpenGL/OpenGLWindowContext.h
    include/Engine/Graphics/OpenGL/OpenGLHelpers.h
    include/Engine/Graphics/Vulkan/VulkanBase.h
    include/Engine/Luau/Task.h
    include/Engine/Luau/Lib.h
    include/Engine/Luau/ThreadData.h
    include/Engine/Luau/PrettyPrint.h
    include/Engine/ObjectPool.h
    include/Engine/Luau/LuauRuntime.h
    include/Engine/Luau/LuauTimeoutHandler.h
    include/Engine/Data/Rect.h
    include/Engine/Objects/Object.h
    include/Engine/Scene/Scene.h
    include/Engine/Math.h
    include/Engine/Luau/AtomsDef.h
    include/Engine/Objects/Component.h
    include/Engine/Graphics/WebGPU/WebGPURenderer.h
    include/Engine/Graphics/WebGPU/WebGPUWindowContext.h
    include/Engine/Graphics/RenderCommand.h
    include/Engine/Asset.h
    include/Engine/STL.h
    include/Engine/Graphics/Vulkan/VulkanIMGUI.h
    src/Engine/Graphics/Vulkan/VulkanIMGUI.cpp
    include/Engine/Luau/Reg.h
    include/Engine/Luau/Userdata.h
    include/Engine/Perf.h
    include/Engine/Luau/Stack.h
    include/Engine/Luau/ComponentUserdata.h
    src/Engine/Objects/Component.cpp
    include/Andromeda.h
    include/Andromeda/System.h
    include/Andromeda/Luau.h
    include/Andromeda/Scene.h
    include/Engine/Graphics/Vulkan/VulkanGraphicsPipeline.h

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
        glad
    )
endif()

if (WEBGPU_RENDERER)
    target_link_libraries(AndromedaEngine PRIVATE webgpu sdl3webgpu)
    target_copy_webgpu_binaries(AndromedaEngine)
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
#if (CMAKE_BUILD_TYPE STREQUAL "Release")
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
#endif()