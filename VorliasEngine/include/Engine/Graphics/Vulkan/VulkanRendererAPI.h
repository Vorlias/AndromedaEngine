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

		~VulkanRenderer() override;
	private:
		VulkanContext context;
	};
} // namespace andromeda::graphics