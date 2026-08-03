#pragma once
#include "Engine/Graphics/Shader.h"
#include <shaderc/shaderc.hpp>
#include <vulkan/vulkan.h>

namespace andromeda::graphics {
    class VulkanShader : public Shader {
    public:
        VulkanShader(VkDevice device, ShaderType type);
        bool LoadFromFile(const std::string& fileName);
    private:
        VkShaderModule CreateShaderModule(const std::string& source, shaderc_shader_kind kind);
        std::string fileName;

        shaderc_shader_kind shader_kind;

        VkDevice device = VK_NULL_HANDLE;
        VkShaderModule shaderModule = VK_NULL_HANDLE;
    };
}