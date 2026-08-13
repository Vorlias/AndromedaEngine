#pragma once
#include "Engine/Graphics/Shader.h"
#if ANDROMEDA_SHADER_COMPILATION || ANDROMEDA_EDITOR
#	warning "including compiler"
#	include <shaderc/shaderc.hpp>
#endif
#include <vulkan/vulkan.h>

namespace andromeda::graphics {
	class VulkanShader : public Shader {
		struct ShaderModule {
			ShaderType shaderType;
			VkShaderModule shaderModule = VK_NULL_HANDLE;
			const char* entryPoint;

			VkPipelineShaderStageCreateInfo GetShaderStage() {
				VkShaderStageFlagBits stage;
				switch (shaderType) {
					case ShaderType::Fragment:
						stage = VK_SHADER_STAGE_FRAGMENT_BIT;
						break;
					case ShaderType::Vertex:
						stage = VK_SHADER_STAGE_VERTEX_BIT;
						break;
				}

				ANDROMEDA_ASSERT(shaderModule != nullptr);
				ANDROMEDA_ASSERT(entryPoint != nullptr);
				return VkPipelineShaderStageCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage = stage,
					.module = shaderModule,
					.pName = entryPoint,
				};
			}

			ShaderModule(ShaderType shaderType, VkShaderModule module) : entryPoint("main"), shaderType(shaderType), shaderModule(module) {}
			ShaderModule(ShaderType shaderType, VkShaderModule module, const char* entryPoint)
				: entryPoint(entryPoint), shaderType(shaderType), shaderModule(module) {}

			ShaderModule(const ShaderModule& other) {
				entryPoint = other.entryPoint;
				shaderType = other.shaderType;
				shaderModule = other.shaderModule;
			}

			ShaderModule(const ShaderModule&& other) {
				entryPoint = other.entryPoint;
				shaderType = other.shaderType;
				shaderModule = other.shaderModule;
			}
		};

	public:
		VulkanShader(VkDevice device);
		bool LoadFromFile(const std::string& fileName, ShaderType shaderType, const char* entryPoint = "main");
		bool LoadSPIRV(const std::vector<char>& data, ShaderType shaderType, const char* entryPoint = "main") override;

		void Unload() override;
		~VulkanShader();

		[[nodiscard]] const std::vector<ShaderModule>& GetShaderModules() const {
			return m_modules;
		}

		// std::vector<VkPipelineShaderStageCreateInfo> GetShaderStages() {
		// 	std::vector<VkPipelineShaderStageCreateInfo> stages;
		// 	stages.reserve(m_modules.size());

		// 	for (ShaderModule& module : m_modules) {
		// 		stages.push_back(module.GetShaderStage());
		// 	}
		// 	return stages;
		// }

	private:
#if ANDROMEDA_SHADER_COMPILATION || ANDROMEDA_EDITOR
		VkShaderModule CompileShaderModuleFromSource(const std::string& source, shaderc_shader_kind kind);
#endif
		VkShaderModule LoadShaderModule(const std::vector<char>& shader);

		std::string fileName;

#if ANDROMEDA_SHADER_COMPILATION || ANDROMEDA_EDITOR
		shaderc_shader_kind shader_kind;
#endif

		VkDevice m_device = VK_NULL_HANDLE;
		std::vector<ShaderModule> m_modules{};
	};
} // namespace andromeda::graphics