#pragma once
#include <map>
#include <optional>
#include <variant>
#include <SDL3/SDL.h>
#include "Engine/Graphics/RendererAPI.h"
#include "Engine/Graphics/GraphicsContext.h"

namespace andromeda {
	struct WindowOptions {
		int width = 1024;
		int height = 768;
		const char* title = "Andromeda";
		unsigned long window_flags = SDL_WINDOW_RESIZABLE;
		WindowOptions();
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
	public:
		Window(const WindowOptions& options);
		~Window();

		bool Initialize(graphics::Renderer::API api);

#if ANDROMEDA_EXPERIMENTAL
		const std::optional<WindowEvent> PollEvent();
#endif

#if ANDROMEDA_INTERNAL
        // Uses SDL window polling - not guaranteed to always be a thing 
		bool PollSDLEvent(SDL_Event* e);
#endif

		void Shutdown();

		void Close();

		SDL_Window* GetHandle() const;
		SDL_WindowID GetWindowId() const;
		inline bool HasRequestedExit() const {
			return m_requestedExit;
		}

	private:
		friend class Engine;
		graphics::GraphicsContext* m_graphics_context;

		WindowOptions m_window_options;
		SDL_WindowID m_window_id;
		SDL_Window* m_window = nullptr;
		bool m_requestedExit = false;

		static SDL_WindowID s_primary_window_id;
		static std::map<SDL_WindowID, Window&> s_windows;
	};
} // namespace andromeda