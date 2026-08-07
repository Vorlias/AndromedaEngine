#pragma once
#include "../RendererAPI.h"
#include "VulkanInstance.h"
#include "Engine/Common.h"

namespace andromeda::graphics {
	class VulkanRenderer : public Renderer {
	public:
		bool Initialize() override;
		void Clear() override;
		void SetClearColor(Color color) override;
		void Shutdown() override;
		
		const std::string GetAPIString() const override;
		inline Renderer::API GetAPI() override {
			return Renderer::API::Vulkan;
		}

		GraphicsContext* CreateWindowGraphicsContext(SDL_Window* window) override;
		ScopeRef<GraphicsContext> CreateGraphicsContext(SDL_Window* window);

		VulkanContext* GetContext() {
			return m_context;
		}

		~VulkanRenderer() override;
	private:
		VulkanContext* m_context{nullptr};
		std::vector<GraphicsContext> m_contexts{};
	};
} // namespace andromeda::graphics