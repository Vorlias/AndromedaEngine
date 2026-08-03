#include "Engine/Graphics/Vulkan/VulkanShader.h"
#include "Engine/Log.h"
#include "Engine/File.h"
#include <volk.h>

namespace andromeda::graphics {
    VulkanShader::VulkanShader(VkDevice device, ShaderType type): device(device) {
        shaderc_shader_kind kind;
        switch (type) {
            case ShaderType::Fragment:
                kind = shaderc_fragment_shader;
                break;
            case ShaderType::Vertex:
                kind = shaderc_vertex_shader;
                break;
        }

        shader_kind = kind;
    }

    VkShaderModule VulkanShader::CreateShaderModule(const std::string& source, shaderc_shader_kind kind) {
        if (source.empty()) {
            andromeda::error("Failed to create shader, shader is empty");
            return nullptr;
        }

        shaderc::Compiler compiler;
        shaderc::CompileOptions opts;

        opts.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
        opts.SetTargetSpirv(shaderc_spirv_version_1_6);
        opts.SetOptimizationLevel(shaderc_optimization_level_performance);

        shaderc::CompilationResult result = compiler.CompileGlslToSpv(source, kind, fileName.c_str(), opts);
        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            andromeda::error("Failed to compile shader: " + result.GetErrorMessage());
            return nullptr;
        }

        std::vector<uint32_t> spv = { result.cbegin(), result.cend() };
        VkShaderModuleCreateInfo moduleCreateInfo {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = spv.size() * sizeof(uint32_t),
            .pCode = spv.data(),
        };

        VkShaderModule shaderModule = nullptr;
        if (vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            andromeda::error("Failed creating shader module");
            return nullptr;
        }

        return shaderModule;
    }

    bool VulkanShader::LoadFromFile(const std::string& fileName) {
        auto src = andromeda::ReadFile(fileName);
        if (src.empty()) {
            andromeda::error("Failed to create shader at path " + fileName + ", empty or non-existent.");
            return false;
        }

        andromeda::trace("Compiling shader at path " + fileName);
        shaderModule = CreateShaderModule(fileName, shader_kind);
        return shaderModule != nullptr;
    }
}