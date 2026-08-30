#if ANDROMEDA_WGPU
#include "Engine/Graphics/WebGPU/WebGPURenderer.h"
#include "Engine/Graphics/WebGPU/WebGPUWindowContext.h"
#include <webgpu/webgpu.hpp>

andromeda::graphics::WGPUWindowContext::WGPUWindowContext(const WGPURenderer* renderer, SDL_Window* window) {
	m_renderer = renderer;
	m_surface = SDL_GetWGPUSurface(renderer->GetInstance(), window);
	if (m_surface == nullptr) {
		andromeda::error("Could not create surface");
		return;
	}

	int width, height;
	SDL_GetWindowSize(window, &width, &height);
    Resized(width, height);
}

void andromeda::graphics::WGPUWindowContext::Resized(int width, int height) {
	WGPUSurfaceConfiguration config = {};
	config.nextInChain = nullptr;
	config.device = m_renderer->GetDevice();
	config.format = WGPUTextureFormat_Undefined;
	config.usage = WGPUTextureUsage_RenderAttachment;

	// We get the capabilities for a pair of (surface, adapter).
	// If it works, this populates the `capabilities` structure
	WGPUSurfaceCapabilities capabilities = {};
	WGPUStatus status = wgpuSurfaceGetCapabilities(m_surface, m_renderer->GetAdapter(), &capabilities);
	if (status != WGPUStatus_Success) {
		andromeda::error("Failed to get surface caps");
		return;
	}

	// From the capabilities, we get the preferred format: it is always the first one !
	// (NB: There is always at least 1 format if the GetCapabilities was successful)
	config.format = capabilities.formats[0];

	// We no longer need to access the capabilities, so we release their memory.
	wgpuSurfaceCapabilitiesFreeMembers(capabilities);

	config.width = width;
	config.height = height;
    std::cout << "resized " << width << " x " << height << std::endl;

	config.viewFormatCount = 0;
	config.viewFormats = nullptr;
	config.alphaMode = WGPUCompositeAlphaMode_Auto;
	config.presentMode = WGPUPresentMode_Fifo;

	wgpuSurfaceConfigure(m_surface, &config);
}

WGPUTextureView andromeda::graphics::WGPUWindowContext::GetNextSurfaceView() {
	WGPUSurfaceTexture surfaceTexture{
		.nextInChain = nullptr,
		.texture = nullptr,
		.status = (WGPUSurfaceGetCurrentTextureStatus)0,
	};

	wgpuSurfaceGetCurrentTexture(m_surface, &surfaceTexture);

	if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal &&
	    surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal) {
		return nullptr;
	}

	WGPUTextureViewDescriptor viewDescriptor{};
	viewDescriptor.label = {"VIEW", 5};
	viewDescriptor.mipLevelCount = WGPU_MIP_LEVEL_COUNT_UNDEFINED;
	viewDescriptor.arrayLayerCount = WGPU_ARRAY_LAYER_COUNT_UNDEFINED;
	viewDescriptor.baseArrayLayer = 0;
	viewDescriptor.baseMipLevel = 0;
	viewDescriptor.dimension = WGPUTextureViewDimension_2D; // not to confuse with 2DArray

	WGPUTextureView targetView = wgpuTextureCreateView(surfaceTexture.texture, &viewDescriptor);

	// We no longer need the texture, only its view,
	// so we release it at the end of GetNextSurfaceViewData
	// wgpuTextureRelease(surfaceTexture.texture);

	return targetView;
}

void andromeda::graphics::WGPUWindowContext::RenderPrepare() {
	m_textureView = GetNextSurfaceView();
	if (!m_textureView) {
		andromeda::error("no texture view");
		return;
	}
}

void andromeda::graphics::WGPUWindowContext::RenderDraw() {
	WGPUCommandEncoderDescriptor cmdEncoderDesc{};
	WGPUCommandEncoder cmdEncoder = wgpuDeviceCreateCommandEncoder(m_renderer->GetDevice(), &cmdEncoderDesc);
	WGPURenderPassDescriptor renderPassDescriptor{
		.nextInChain = nullptr,
		.label = WGPU_STRING_VIEW_INIT,
		.colorAttachmentCount = 0,
		.colorAttachments = nullptr,
		.depthStencilAttachment = nullptr,
		.occlusionQuerySet = nullptr,
		.timestampWrites = nullptr,
	};

	WGPURenderPassColorAttachment clearColorAttachment{
		.view = m_textureView,
		.loadOp = WGPULoadOp_Clear,
		.storeOp = WGPUStoreOp_Store,
		.clearValue = m_clearColor,
	};

	renderPassDescriptor.colorAttachmentCount = 1;
	renderPassDescriptor.colorAttachments = &clearColorAttachment;

	WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(cmdEncoder, &renderPassDescriptor);
	wgpuRenderPassEncoderEnd(renderPass);
	wgpuRenderPassEncoderRelease(renderPass);

	WGPUCommandBufferDescriptor cmdBufferDescriptor{};
	WGPUCommandBuffer command = wgpuCommandEncoderFinish(cmdEncoder, &cmdBufferDescriptor);
	wgpuCommandEncoderRelease(cmdEncoder);

	wgpuQueueSubmit(m_renderer->GetQueue(), 1, &command);
	wgpuCommandBufferRelease(command);
}

void andromeda::graphics::WGPUWindowContext::RenderPresent() {
	// At the end of the frame
	wgpuTextureViewRelease(m_textureView);
	wgpuSurfacePresent(m_surface);
}
#endif