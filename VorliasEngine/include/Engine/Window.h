#pragma once
#include <map>
#include <SDL3/SDL.h>

namespace andromeda {
	struct WindowOptions {
		int width = 1024;
		int height = 768;
		const char* title = "Andromeda";
		unsigned long window_flags = SDL_WINDOW_RESIZABLE;
        WindowOptions();
	};

	class Window {
	public:
		Window();
		~Window();

		bool Initialize(const WindowOptions& options);
        void PushEvents();
		void Shutdown();

        void Close();

		SDL_Window* GetHandle() const;
		SDL_WindowID GetWindowId() const;
        inline bool HasRequestedExit() const { return m_requestedExit; }
	private:
        SDL_WindowID m_window_id;
		SDL_Window* m_window = nullptr;
        bool m_requestedExit = false;

		static SDL_WindowID s_primary_window_id;
		static std::map<SDL_WindowID, Window&> s_windows;
	};
} // namespace andromeda