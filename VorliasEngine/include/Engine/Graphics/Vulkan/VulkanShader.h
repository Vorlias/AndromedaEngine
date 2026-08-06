#pragma once
#include "Engine/Graphics/Shader.h"
#if ANDROMEDA_SHADER_COMPILATION || ANDROMEDA_EDITOR
#warning "including compiler"
#include <shaderc/shaderc.hpp>
#endif
#include <vulkan/vulkan.h>

namespace andromeda::graphics {
    class VulkanShader : public Shader {
    public:
        VulkanShader(VkDevice device, ShaderType type);
        bool LoadFromFile(const std::string& fileName);
    private:
#if ANDROMEDA_SHADER_COMPILATION || ANDROMEDA_EDITOR
        VkShaderModule CompileShaderModuleFromSource(const std::string& source, shaderc_shader_kind kind);
#endif

        std::string fileName;

#if ANDROMEDA_SHADER_COMPILATION || ANDROMEDA_EDITOR
        shaderc_shader_kind shader_kind;
#endif

        VkDevice device = VK_NULL_HANDLE;
        VkShaderModule shaderModule = VK_NULL_HANDLE;
    };
}