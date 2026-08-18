#pragma once
#if ANDROMEDA_WGPU
// #	define SDL_MAIN_HANDLED
#	include "sdl3webgpu.h"
#	include <webgpu/webgpu.h>
#	include "../RendererAPI.h"
#	include "Engine/Log.h"
#	include "Engine/Data/Color.h"


namespace andromeda::graphics {
	class WGPURenderer;
	class WGPUWindowContext final : public GraphicsContext {
	public:
		WGPUWindowContext(const WGPURenderer* renderer, SDL_Window* window);

		void Initialize() override {}
		void Shutdown() override {
			wgpuSurfaceUnconfigure(m_surface);
		}
		void Resized(int width, int height) override {}

		constexpr WGPUSurface GetSurface() const {
			return m_surface;
		}

		void Prepare() override;
		void Render() override;
		void Present() override;

		void SetClearColor(Color color) override {
			m_clearColor =
				WGPUColor{static_cast<double>(color.r), static_cast<double>(color.g), static_cast<double>(color.b), static_cast<double>(color.a)};
		}

	private:
		const WGPURenderer* m_renderer;

		WGPUColor m_clearColor{1.0, 0.8, 0.55, 1.0};

		WGPUTextureView GetNextSurfaceView();

		WGPUCommandEncoder m_encoder{};

		WGPUTextureView m_textureView{};
		WGPUSurface m_surface{};
	};
} // namespace andromeda::graphics
#endif