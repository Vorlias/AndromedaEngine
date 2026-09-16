#pragma once
#include "Engine/Graphics/RendererAPI.h"
#include "Engine/Window.h"
#include <SDL3/SDL.h>

struct SDL_Window;
namespace andromeda {
	class IMGUI {
	public:
		virtual void Initialize() = 0;

		virtual void UpdateSwapchain() = 0;
		virtual void Resize(int width, int height) = 0;
		virtual void NewFrame() = 0;
		virtual void Render() = 0;
		virtual bool ProcessEvent(SDL_Event& e) = 0;
		virtual void Shutdown() = 0;
	protected:
		IMGUI(SDL_Window* window) : m_window(window) {}
		SDL_Window* m_window;
	};
} // namespace andromeda