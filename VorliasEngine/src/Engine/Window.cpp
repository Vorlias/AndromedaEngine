#include "Engine/Window.h"
#include "SDL3/SDL.h"
#include "Engine/Log.h"

andromeda::WindowOptions::WindowOptions() {}

andromeda::Window::Window() {}
andromeda::Window::~Window() {}

SDL_WindowID andromeda::Window::s_primary_window_id{};
std::map<SDL_WindowID, andromeda::Window&> andromeda::Window::s_windows{};

bool andromeda::Window::Initialize(const WindowOptions& options) {
	m_window = SDL_CreateWindow(options.title, options.width, options.height, options.window_flags);

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
	return true;
}

void andromeda::Window::PushEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_EVENT_QUIT:
				m_requestedExit = true;
				break;
			default:
				break;
		}
	}
}

void andromeda::Window::Close() {
	m_requestedExit = true;
}

void andromeda::Window::Shutdown() {
	if (m_window != nullptr) {
		s_windows.erase(m_window_id);
		SDL_DestroyWindow(m_window);

		if (s_primary_window_id == m_window_id) {
			for (auto& window : s_windows) {
				window.second.Shutdown();
			}
            
            s_primary_window_id = 0;
		}
	}
}

SDL_Window* andromeda::Window::GetHandle() const {
	return m_window;
}

SDL_WindowID andromeda::Window::GetWindowId() const {
	return SDL_GetWindowID(m_window);
}