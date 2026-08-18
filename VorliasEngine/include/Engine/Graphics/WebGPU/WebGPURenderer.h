#pragma once
#include <webgpu/webgpu.h>
#include "../RendererAPI.h"
#include "Engine/Common.h"
#include "Engine/Log.h"
#include "WebGPUWindowContext.h"

#if ANDROMEDA_WGPU
namespace andromeda::graphics {
	class WGPURenderer : public Renderer {
	public:
		bool Initialize() override;

		void Clear() override {}
		void SetClearColor(Color color) override {}

		void Shutdown() override;

		[[nodiscard]] const std::string GetAPIString() const override {
			WGPUAdapterInfo info = {};
			wgpuAdapterGetInfo(m_adapter, &info);

			std::string backendName = "Unknown";
			switch (info.backendType) {
				case WGPUBackendType_WebGPU:
					backendName = "WebGPU";
					break;
				case WGPUBackendType_D3D11:
					backendName = "DirectX11";
					break;
				case WGPUBackendType_D3D12:
					backendName = "DirectX12";
					break;
				case WGPUBackendType_Metal:
					backendName = "Metal";
					break;
				case WGPUBackendType_Vulkan:
					backendName = "Vulkan";
					break;
				case WGPUBackendType_OpenGL:
					backendName = "OpenGL";
					break;
				case WGPUBackendType_OpenGLES:
					backendName = "OpenGLES";
					break;
			}

			wgpuAdapterInfoFreeMembers(info);
			return std::format("{} (WebGPU)", backendName);
		}
		inline Renderer::API GetAPI() override {
			return Renderer::API::Vulkan;
		}

		GraphicsContext* CreateWindowGraphicsContext(SDL_Window* window) override {
			return new WGPUWindowContext(m_instance, window);
		}
		ScopeRef<GraphicsContext> CreateGraphicsContext(SDL_Window* window) {}

		~WGPURenderer() override {}
	private:
		WGPUInstance m_instance;
		WGPUAdapter m_adapter;
        WGPUDevice m_device;
        WGPUQueue m_queue;
	};
} // namespace andromeda::graphics
#endif