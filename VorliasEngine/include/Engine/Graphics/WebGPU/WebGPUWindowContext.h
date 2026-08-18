#pragma once
#if ANDROMEDA_WGPU
// #	define SDL_MAIN_HANDLED
#	include "sdl3webgpu.h"
#	include <webgpu/webgpu.h>
#	include "../RendererAPI.h"
#	include "Engine/Log.h"


namespace andromeda::graphics {
	class WGPUWindowContext : public GraphicsContext {
	public:
		WGPUWindowContext(WGPUInstance instance, SDL_Window* window) {
			m_surface = SDL_GetWGPUSurface(instance, window);
			andromeda::trace("Set up WGPU surface");
		}

		void Initialize() override {}
		void Shutdown() override {}
		void Resized(int width, int height) override {}

		constexpr WGPUSurface GetSurface() const { return m_surface; }
	private:
		WGPUSurface m_surface;
	};
} // namespace andromeda::graphics
#endif