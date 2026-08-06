#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/Vulkan/VulkanShader.h"
#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"
#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"
#include "Engine/Engine.h"
#include "Engine/Log.h"
#include "Engine/File.h"

namespace andromeda::graphics {
	SharedRef<Shader> Shader::LoadShaderFromFile(const std::string& fileName, ShaderType type) {
		auto api = Engine::GetInstance().GetCurrentAPI();
		switch (api) {
			case graphics::API::Vulkan: {
                // TODO: Preferably not use the window...
                auto main_window = Engine::GetInstance().GetMainWindow();
                auto context = static_cast<VulkanWindowContext*>(main_window->m_graphics_context); 

				SharedRef<VulkanShader> shader = CreateSharedRef<VulkanShader>(context->GetDevice(), type);
                if (shader->LoadFromFile(fileName)) {
                    return shader;
                }

                andromeda::error("Failed to load shader from file");
				return nullptr;
			}
			default:
				andromeda::error("Failed to create shader");
				return nullptr;
		}
	}
} // namespace andromeda::graphics