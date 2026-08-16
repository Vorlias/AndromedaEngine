#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/Vulkan/VulkanShader.h"
#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"
#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"

#include "Engine/Graphics/OpenGL/OpenGLWindowContext.h"
#include "Engine/Graphics/OpenGL/OpenGLShader.h"

#include "Engine/Engine.h"
#include "Engine/Log.h"
#include "Engine/File.h"

namespace andromeda::graphics {
	Ref<Shader> ShaderLibrary::LoadShaderFromFiles(const std::string& name, const std::string& vertexFileName, const std::string& fragmentFileName) {
		auto api = Engine::GetInstance().GetCurrentAPI();
		switch (api) {
			case graphics::API::Vulkan: { // TODO: Preferably not use the window...
				auto main_window = Engine::GetInstance().GetMainWindow();

				ANDROMEDA_ASSERTM(main_window != nullptr, "Cannot create a shader without a window graphics context");
				auto context = static_cast<VulkanWindowContext*>(main_window->GetGraphicsContext());


				VulkanShader* shader = new VulkanShader(context->GetDevice());
				shader->LoadFromFile(vertexFileName, ShaderType::Vertex, "main");
				shader->LoadFromFile(fragmentFileName, ShaderType::Fragment, "main");

				Ref<Shader> shaderRef = shader;
				m_shaders.insert({name, shaderRef});

				ANDROMEDA_VRB("Loaded vulkan shader '{}'", name);
				return shaderRef;
			}
#if ANDROMEDA_OPENGL
			case graphics::API::OpenGL: {
				OpenGLShader* shader = new OpenGLShader();
				ANDROMEDA_ASSERT(shader->LoadFromFile(vertexFileName, ShaderType::Vertex));
				ANDROMEDA_ASSERT(shader->LoadFromFile(fragmentFileName, ShaderType::Fragment));
				Ref<Shader> shaderRef = shader;
				m_shaders.insert({name, shaderRef});

				ANDROMEDA_VRB("Loaded OpenGL shader '{}'", name);
				return shaderRef;
			}
#endif
			default:
				andromeda::error("Failed to create shader");
				return nullptr;
		}

		return nullptr;
	}

	void ShaderLibrary::UnloadAllShaders() {
		for (auto& [id, shader] : m_shaders) {
			ANDROMEDA_VRB("Unloaded shader '{}'", id);

			shader->Unload();
			shader.Reset();
		}

		m_shaders.clear();
	}

	SharedRef<Shader> Shader::LoadShaderFromFile(const std::string& fileName, ShaderType type) {
		auto api = Engine::GetInstance().GetCurrentAPI();
		switch (api) {
			case graphics::API::Vulkan: {
				// TODO: Preferably not use the window...
				auto main_window = Engine::GetInstance().GetMainWindow();
				auto context = static_cast<VulkanWindowContext*>(main_window->m_graphics_context);

				SharedRef<VulkanShader> shader = CreateSharedRef<VulkanShader>(context->GetDevice());
				if (shader->LoadFromFile(fileName, type)) {
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