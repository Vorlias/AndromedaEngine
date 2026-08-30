#include "Engine/Graphics/RenderCommand.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/RendererAPI.h"

#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"
#include "Engine/Graphics/Vulkan/VulkanShader.h"

using namespace andromeda::graphics;

void DrawVkTriangleDemoCommand::Draw(GraphicsContext* context) {
	ANDROMEDA_ASSERT(context->GetAPI() == API::Vulkan);

	auto ctx = static_cast<VulkanWindowContext*>(context);
	ctx->DrawDemoTriangle();
}

void DrawVertexArrayCommand::Draw(GraphicsContext* context) {
	ANDROMEDA_ASSERT(context->GetAPI() == API::Vulkan);
	auto ctx = static_cast<VulkanWindowContext*>(context);

	VulkanShader* vkShader = static_cast<VulkanShader*>(m_shader.get()); 
}