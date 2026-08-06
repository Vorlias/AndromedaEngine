#pragma once
#include "Engine/Data/Color.h"
#include "Engine/Graphics/GraphicsContext.h"
#include <SDL3/SDL.h>

namespace andromeda::graphics {
	enum class API {
		None = 0,
		Vulkan = 1,
#ifdef ANDROMEDA_OPENGL
		OpenGL = 2,
#endif
#ifdef ANDROMEDA_MAC
		Metal = 3,
#endif
#ifdef ANDROMEDA_WIN
		DirectX = 4,
#endif
	};

	// A collection of supported APIs in Andromeda
	constexpr API supportedAPIs[] = {
		API::None,
		API::Vulkan,
// #if ANDROMEDA_OPENGL
// 		API::OpenGL,
// #endif
	};

	class Renderer {
	public:
		using API = API;

		virtual bool Initialize() = 0;
		virtual void Clear() = 0;
		virtual void SetClearColor(Color color) = 0;
		virtual void Shutdown() = 0;

		virtual API GetAPI() = 0;
		virtual const std::string GetAPIString() const = 0;

		virtual GraphicsContext* CreateWindowGraphicsContext(SDL_Window* window) = 0;
		virtual ~Renderer() {}
	};
} // namespace andromeda::graphics