#include "Engine/Graphics/Vulkan/VulkanShader.h"
#include "Engine/Log.h"
#include "Engine/File.h"
#include <volk.h>

namespace andromeda::graphics {
	VulkanShader::VulkanShader(VkDevice device) : m_device(device) {
		m_modules.reserve(2); // for fragment + vertex
	}

#if ANDROMEDA_SHADER_COMPILATION || ANDROMEDA_EDITOR
	VkShaderModule VulkanShader::CompileShaderModuleFromSource(const std::string& source, shaderc_shader_kind kind) {
		if (source.empty()) {
			andromeda::error("Failed to create shader, shader is empty");
			return nullptr;
		}

		std::string targetSource = source;

		if (!targetSource.starts_with("#version")) {
			targetSource = "#version 460\n" + targetSource;
		}

		shaderc::Compiler compiler;
		shaderc::CompileOptions opts;

		opts.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
		opts.SetTargetSpirv(shaderc_spirv_version_1_6);
		opts.SetOptimizationLevel(shaderc_optimization_level_performance);

		shaderc::CompilationResult result = compiler.CompileGlslToSpv(targetSource, kind, fileName.c_str(), opts);
		if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
			andromeda::error("Failed to compile shader: " + result.GetErrorMessage());
			return nullptr;
		}

		std::vector<uint32_t> spv = {result.cbegin(), result.cend()};
		VkShaderModuleCreateInfo moduleCreateInfo{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = spv.size() * sizeof(uint32_t),
			.pCode = spv.data(),
		};

		VkShaderModule shaderModule = nullptr;
		if (vkCreateShaderModule(m_device, &moduleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			andromeda::error("Failed creating shader module");
			return nullptr;
		}

		return shaderModule;
	}
#endif

	VkShaderModule VulkanShader::LoadShaderModule(const std::vector<char>& shader) {
		VkShaderModuleCreateInfo moduleCreateInfo{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = shader.size(),
			.pCode = reinterpret_cast<const uint32_t*>(shader.data()),
		};

		VkShaderModule shaderModule = nullptr;
		if (vkCreateShaderModule(m_device, &moduleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			andromeda::error("Failed creating shader module");
			return nullptr;
		}

		return shaderModule;
	}

	bool VulkanShader::LoadSPIRV(const std::vector<char>& data, ShaderType shaderType, const char* entryPoint) {
		VkShaderModule shaderModule = LoadShaderModule(data);
		if (shaderModule == nullptr)
			return false;

		m_modules.emplace_back(shaderType, shaderModule, entryPoint);
		return true;
	}

	bool VulkanShader::LoadFromFile(const std::string& fileName, ShaderType shaderType, const char* entryPoint) {
		auto src = andromeda::ReadFile(fileName);
		if (src.empty()) {
			andromeda::error("Failed to create shader at path " + fileName + ", empty or non-existent.");
			return false;
		}

		if (fileName.ends_with(".spv")) {
			auto data = std::vector<char>(src.begin(), src.end());
			return LoadSPIRV(data, shaderType, entryPoint);
		} else {
#if ANDROMEDA_SHADER_COMPILATION || ANDROMEDA_EDITOR
			shaderc_shader_kind kind;
			switch (shaderType) {
				case ShaderType::Fragment:
					kind = shaderc_fragment_shader;
					break;
				case ShaderType::Vertex:
					kind = shaderc_vertex_shader;
					break;
			}

			auto shaderModule = CompileShaderModuleFromSource(src, kind);
			if (shaderModule != nullptr) {
				m_modules.emplace_back(shaderType, shaderModule, entryPoint);
				return true;
			}

			return false;
#else
			andromeda::error("Shader must be an .spv file");
			return false;
#endif
		}
	}

	void VulkanShader::Unload() {
		ANDROMEDA_ASSERTM(vkDestroyShaderModule, "Vulkan shader module cannot be cleaned up when Vulkan has been destroyed");

		for (auto& module : m_modules) {
			if (module.shaderModule != nullptr)
				vkDestroyShaderModule(m_device, module.shaderModule, nullptr);
		}

		m_modules.clear();
	}

	VulkanShader::~VulkanShader() {}
} // namespace andromeda::graphics