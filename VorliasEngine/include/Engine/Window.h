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

	enum class WindowFlags : uint64_t {
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

	struct WindowOptions {
		Vector2u size = Vector2u(1024, 768);
		Vector2i position = Vector2i(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

		const char* title = "Andromeda";
		WindowFlags window_flags = WindowFlags::Default;
		WindowOptions();
		WindowOptions(const char* title, Vector2u size): title(title), size(size) {}
		WindowOptions(const char* title, Vector2u size, WindowFlags flags): title(title), size(size), window_flags(flags) {}
	};

#if ANDROMEDA_EXPERIMENTAL
	enum class WindowChangeEvent {
		Resized = SDL_EVENT_WINDOW_RESIZED,
		Moved = SDL_EVENT_WINDOW_MOVED,
		Minimized = SDL_EVENT_WINDOW_MINIMIZED,
		Maximized = SDL_EVENT_WINDOW_MAXIMIZED,
		EnteredFullscreen = SDL_EVENT_WINDOW_ENTER_FULLSCREEN,
		ExitedFullscreen = SDL_EVENT_WINDOW_LEAVE_FULLSCREEN,
	};

	struct WindowEvent {
		struct Unknown {};

		struct Closed {};

		struct Resized {
			int width, height;
		};

		struct FocusStateChange {
			bool focused;
		};

		struct WindowStateChange {
			WindowChangeEvent event;
			bool maximized;
			bool minimized;
			bool fullscreen;

			int width, height;
			int x, y;
		};

		struct MouseFocusStateChange {
			bool focused;
		};

		template<typename T>
		[[nodiscard]] inline bool is() const {
			return std::holds_alternative<T>(m_event);
		}

		template<typename T>
		[[nodiscard]] inline T* getIf() const {
			return std::get_if<T>(&m_event);
		}

		template<typename T>
		[[nodiscard]] inline T get() const {
			return std::get<T>(m_event);
		}

	private:
		friend class Window;
		std::variant<Unknown, Closed, Resized, FocusStateChange, WindowStateChange, MouseFocusStateChange> m_event;
	};
#endif

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
		graphics::GraphicsContext* m_graphics_context;
		WindowOptions m_window_options;
		SDL_WindowID m_window_id;
		SDL_Window* m_window = nullptr;
		bool m_requestedExit = false;

		// static SDL_WindowID s_primary_window_id;
		// static std::map<SDL_WindowID, Window&> s_windows;
	};
} // namespace andromeda