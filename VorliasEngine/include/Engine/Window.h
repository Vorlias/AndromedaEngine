#pragma once
#include <map>
#include <optional>
#include <variant>
#include <SDL3/SDL.h>
#include "Engine/Graphics/RendererAPI.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Shader.h"

namespace andromeda {
	using WindowID = SDL_WindowID;

	namespace WindowFlags {
		enum {
			Resizable = SDL_WINDOW_RESIZABLE,
			Fullscreen = SDL_WINDOW_FULLSCREEN,

			Borderless = SDL_WINDOW_BORDERLESS,
			Maximized = SDL_WINDOW_MAXIMIZED,
			AlwaysOnTop = SDL_WINDOW_ALWAYS_ON_TOP,

			// Modal = SDL_WINDOW_MODAL,
			// Utility = SDL_WINDOW_UTILITY,

			BorderlessFullscreen = Fullscreen | Borderless,
			Default = Resizable,
		};
	};


	struct WindowIcon {
		WindowIcon(void* data, size_t size);
		WindowIcon(const char* filePath);
		WindowIcon() : m_data(0) {}
		WindowIcon(const andromeda::WindowIcon& other) = delete;

		operator bool() const {
			return m_data != 0;
		}

		void Destroy();
		~WindowIcon();

	private:
		friend class Window;

		SDL_Surface* m_data{};
	};

	struct WindowOptions {
		Vector2u size = Vector2u(1024, 768);
		Vector2i position = Vector2i(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

		const char* title = "Andromeda";
		int window_flags = WindowFlags::Default;
		WindowIcon windowIcon;

		WindowOptions() = default;
		WindowOptions(const char* title, Vector2u size) : title(title), size(size), windowIcon(), window_flags(WindowFlags::Default) {}
		WindowOptions(const char* title, Vector2u size, int flags) : title(title), size(size), window_flags(flags), windowIcon() {}

		WindowOptions(const WindowOptions& src) {
			windowIcon = src.windowIcon;
			window_flags = src.window_flags;
			size = src.size;
			title = src.title;
			position = src.position;
		}
	};

	class Window {
		friend class graphics::Shader;

	public:
		Window(const WindowOptions& options);
		~Window();

		bool Initialize(graphics::Renderer* renderer);

#if ANDROMEDA_EXPERIMENTAL
		const std::optional<WindowEvent> PollEvent();
#endif

		// Uses SDL window polling - not guaranteed to always be a thing
		bool PollSDLEvent(SDL_Event* e);

		void Shutdown();

		void Close();

		SDL_Window* GetHandle() const;
		WindowID GetWindowId() const;
		inline bool HasRequestedExit() const {
			return m_requestedExit;
		}

		void Resized(int width, int height);

		void SetTitle(const char* title);

		constexpr Vector2i GetWindowSizeInPixels() const {
			int w, h;
			SDL_GetWindowSizeInPixels(m_window, &w, &h);
			return Vector2i(w, h);
		}

		constexpr graphics::GraphicsContext* GetGraphicsContext() const {
			return m_graphics_context;
		}

	private:
		friend class Engine;
		graphics::GraphicsContext* m_graphics_context = nullptr;
		const WindowOptions& m_window_options;
		SDL_WindowID m_window_id;
		SDL_Window* m_window = nullptr;
		bool m_requestedExit = false;

		SDL_Surface* m_windowIcon;
		// static SDL_WindowID s_primary_window_id;
		// static std::map<SDL_WindowID, Window&> s_windows;
	};
} // namespace andromeda

// static andromeda::WindowFlags operator|(andromeda::WindowFlags l, andromeda::WindowFlags r) {
// 	return (andromeda::WindowFlags)(static_cast<int>(l) | static_cast<int>(r)); // why I had to do this? idk
// }

// static andromeda::WindowFlags operator|=(andromeda::WindowFlags& l, andromeda::WindowFlags r) {
// 	return l | r;
// }