#pragma once
#include "Window.h"
#include "Time.h"

namespace andromeda {
	class RenderTarget {};

	class Application {
	public:
		virtual const WindowOptions GetWindowOptions() const {
			return WindowOptions("AndromedaEngine", Vector2u(1024, 768), WindowFlags::Default);
		}

		// Set up the application - returns a boolean indicating if initialization was successful
		virtual bool Initialize() {
			return CreateWindow(GetWindowOptions()) != nullptr;
		}

#if ANDROMEDA_INTERNAL
		virtual void WindowEvent(SDL_Event& e) {}
		virtual void RawRender(graphics::Renderer& renderer) {}
#endif

		// Called when the application hits an update frame
		virtual void Update(float deltaTime) {}

		DEPRECATED // TBD if using
		virtual void FixedUpdate(float fixedDeltaTime) {}

		// // Called when the application hits an update frame
		// virtual void Render(Window& window) {}

		// Called when the application is shutting down
		virtual void Shutdown() {}

	protected:
		std::shared_ptr<Window> CreateWindow(const WindowOptions& windowOptions);
		std::shared_ptr<Window> GetMainWindow() const;
		std::shared_ptr<Window> GetWindowById(WindowID id) const;
	public:
		// Gets the amount of time the application has been open
        [[nodiscard]] constexpr float GetElapsedTime() const;

		// Gets the current deltaTime of the application
		[[nodiscard]] constexpr float GetDeltaTime() const;

		// Set the framerate of this application to the given limit
		void SetFramerateLimit(uint32_t limit);
		[[nodiscard]] uint32_t GetFramerateLimit() const;

		void Quit();
	private:
		void UpdateWindows();
		void CloseAllWindows() {
			for (auto wnd : m_windows) {
				wnd->Shutdown();
			}

			m_windows.clear();
			m_main_window = nullptr;
		}

        friend class Engine;

		std::shared_ptr<Window> m_main_window;
		std::vector<std::shared_ptr<Window>> m_windows{};

        Time m_frameTime;
		float m_deltaTime;
		
		Time m_fixedFrameTime = milliseconds(33);
		float m_fixedDeltaTime;

		float m_elapsedTime;

		bool m_quitRequested;
	};

	constexpr float Application::GetElapsedTime() const {
		return m_elapsedTime;
	}

	constexpr float Application::GetDeltaTime() const {
        return m_deltaTime;
    }
} // namespace andromeda
