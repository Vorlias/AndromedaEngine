#include "Engine/Window.h"
#include "SDL3/SDL.h"
#include <optional>
#include "Engine/Log.h"

#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"
#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"

andromeda::WindowOptions::WindowOptions() {}

andromeda::Window::Window(const WindowOptions& options) : m_window_options(options) {}
andromeda::Window::~Window() {}

SDL_WindowID andromeda::Window::s_primary_window_id{};
std::map<SDL_WindowID, andromeda::Window&> andromeda::Window::s_windows{};

bool andromeda::Window::Initialize(graphics::Renderer* renderer) {
	using namespace graphics;

	int window_flags = m_window_options.window_flags;
	Renderer::API api = renderer != nullptr ? renderer->GetAPI() : graphics::Renderer::API::None;

	if (api != graphics::Renderer::API::None) {
		if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
			return false;
		}
	}

	switch (api) {
		case Renderer::API::Vulkan:
			window_flags |= SDL_WINDOW_VULKAN;
			break;
		case Renderer::API::OpenGL:
			break;
		default:
			break;
	}

	m_window = SDL_CreateWindow(m_window_options.title, m_window_options.width, m_window_options.height, window_flags);

	if (!m_window) {
		andromeda::error("Failed to initialize window!");
		return false;
	}

	m_window_id = SDL_GetWindowID(m_window);
	SDL_Window* main_window = SDL_GetWindowFromID(s_primary_window_id);
	if (main_window == nullptr) {
		s_primary_window_id = m_window_id;
	}

	s_windows.insert({m_window_id, *this});

	if (renderer != nullptr) {
		renderer->Initialize();
		m_graphics_context = renderer->CreateWindowGraphicsContext(m_window);
		m_graphics_context->Initialize();
	}

	andromeda::trace("Created window " + std::to_string(m_window_id));
	return true;
}

#if ANDROMEDA_EXPERIMENTAL
const std::optional<andromeda::WindowEvent> andromeda::Window::PollEvent() {
	WindowEvent windowEvent;
	SDL_Event e;

	if (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				windowEvent.m_event = WindowEvent::Closed();
				break;
			case SDL_EVENT_WINDOW_FOCUS_GAINED:
			case SDL_EVENT_WINDOW_FOCUS_LOST: {
				WindowEvent::FocusStateChange focusStateChange;
				focusStateChange.focused = e.type == SDL_EVENT_WINDOW_FOCUS_GAINED;
				windowEvent.m_event = focusStateChange;
				break;
			}
			case SDL_EVENT_WINDOW_MOUSE_LEAVE:
			case SDL_EVENT_WINDOW_MOUSE_ENTER: {
				WindowEvent::MouseFocusStateChange mouseState;
				mouseState.focused = e.type == SDL_EVENT_WINDOW_MOUSE_ENTER;
				windowEvent.m_event = mouseState;
				break;
			}
			case SDL_EVENT_WINDOW_MOVED:
			case SDL_EVENT_WINDOW_RESIZED:
			case SDL_EVENT_WINDOW_RESTORED:
			case SDL_EVENT_WINDOW_MINIMIZED:
			case SDL_EVENT_WINDOW_MAXIMIZED: {
				int flags = SDL_GetWindowFlags(m_window);

				int x, y;
				SDL_GetWindowPosition(m_window, &x, &y);

				int width, height;
				SDL_GetWindowSize(m_window, &width, &height);

				WindowEvent::WindowStateChange event;
				event.event = static_cast<WindowChangeEvent>(e.type);
				event.maximized = flags & SDL_WINDOW_MAXIMIZED;
				event.minimized = flags & SDL_WINDOW_MINIMIZED;
				event.fullscreen = flags & SDL_WINDOW_FULLSCREEN;
				event.width = width;
				event.height = height;
				event.x = x;
				event.y = y;
				windowEvent.m_event = event;
				break;
			}
		}

		return windowEvent;
	}

	return std::optional<WindowEvent>();
}
#endif

#if ANDROMEDA_INTERNAL
bool andromeda::Window::PollSDLEvent(SDL_Event* e) {
	return SDL_PollEvent(e);
}
#endif

void andromeda::Window::Close() {
	m_requestedExit = true;
}

void andromeda::Window::Shutdown() {
	if (m_window != nullptr) {
		s_windows.erase(m_window_id);


		if (s_primary_window_id == m_window_id) {
			for (auto& window : s_windows) {
				window.second.Shutdown();
			}

			s_primary_window_id = 0;
		}

		m_graphics_context->Shutdown();
		andromeda::trace("Cleaned up window " + std::to_string(m_window_id));

		SDL_DestroyWindow(m_window);
	}
}

SDL_Window* andromeda::Window::GetHandle() const {
	return m_window;
}

SDL_WindowID andromeda::Window::GetWindowId() const {
	return SDL_GetWindowID(m_window);
}