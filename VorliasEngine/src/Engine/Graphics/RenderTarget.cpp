#include "Engine/Graphics/RenderTarget.h"
#include "Engine/Engine.h"

#include "Engine/Graphics/Vulkan/VulkanRenderTexture.h"

std::shared_ptr<andromeda::graphics::RenderTexture> andromeda::graphics::RenderTexture::Create(RenderTextureType type) {
	auto& engine = andromeda::Engine::GetInstance();
	auto& renderer = engine.GetRenderer();
	auto main_window = engine.GetMainWindow()->GetGraphicsContext();

	switch (renderer->GetAPI()) {
		case andromeda::graphics::API::Vulkan: {
			VulkanRenderer* vkRenderer = static_cast<VulkanRenderer*>(renderer.get());
			VulkanWindowContext* vkWindow = static_cast<VulkanWindowContext*>(main_window);

            VulkanRenderTexture::RenderTextureFlags flags = VulkanRenderTexture::RENDER_TEXTURE_NONE;
            if (type == RenderTextureType::IMGUITexture) {
                flags = VulkanRenderTexture::RENDER_TEXTURE_IMGUI;
            }

            VulkanRenderTexture* rt = new VulkanRenderTexture(flags);
			if (!rt->Create(vkRenderer->GetContext(), vkWindow, 128, 128)) return nullptr;

            return std::shared_ptr<RenderTexture>(rt);
		}
	}

	return nullptr;
}