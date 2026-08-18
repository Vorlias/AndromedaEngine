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

	WGPUSurfaceConfiguration config = {};
	config.nextInChain = nullptr;
	config.device = renderer->GetDevice();
	config.format = WGPUTextureFormat_Undefined;
	config.usage = WGPUTextureUsage_RenderAttachment;

	// We initialize an empty capability struct:
	WGPUSurfaceCapabilities capabilities = {};

	// We get the capabilities for a pair of (surface, adapter).
	// If it works, this populates the `capabilities` structure
	WGPUStatus status = wgpuSurfaceGetCapabilities(m_surface, renderer->GetAdapter(), &capabilities);
	if (status != WGPUStatus_Success) {
		andromeda::error("Failed to get surface caps");
		return;
	}
	// From the capabilities, we get the preferred format: it is always the first one !
	// (NB: There is always at least 1 format if the GetCapabilities was successful)
	config.format = capabilities.formats[0];

	// We no longer need to access the capabilities, so we release their memory.
	wgpuSurfaceCapabilitiesFreeMembers(capabilities);

	int width, height;
	SDL_GetWindowSize(window, &width, &height);

	config.width = width;
	config.height = height;

	// // And we do not need any particular view format:
	config.viewFormatCount = 0;
	config.viewFormats = nullptr;
	config.alphaMode = WGPUCompositeAlphaMode_Auto;
	config.presentMode = WGPUPresentMode_Fifo;

	// WGPUSurfaceCapabilities surfaceCapabilities;
	// wgpuSurfaceGetCapabilities(m_surface, renderer->GetAdapter(), &surfaceCapabilities);

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

void andromeda::graphics::WGPUWindowContext::Prepare() {
	m_textureView = GetNextSurfaceView();
	if (!m_textureView) {
		andromeda::error("no texture view");
		return;
	}
}

WGPUColor toWGPUColor(andromeda::Color color) {
	return WGPUColor{static_cast<double>(color.r), static_cast<double>(color.g), static_cast<double>(color.b), static_cast<double>(color.a)};
}

static struct {
	WGPURenderPassColorAttachment color_attachment;
	WGPURenderPassDescriptor render_pass_descriptor;
	WGPUBool initialized;
} s_renderState = {
	.color_attachment =
		{
			.loadOp = WGPULoadOp_Clear,
			.storeOp = WGPUStoreOp_Store,
			.clearValue = {1.0, 1.0, 1.0, 1.0},
			.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
		},
	.render_pass_descriptor = {
		.colorAttachmentCount = 1,
		.colorAttachments = &s_renderState.color_attachment,
	},
};

void andromeda::graphics::WGPUWindowContext::Render() {
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

void andromeda::graphics::WGPUWindowContext::Present() {
	// At the end of the frame
	wgpuTextureViewRelease(m_textureView);
	wgpuSurfacePresent(m_surface);
}