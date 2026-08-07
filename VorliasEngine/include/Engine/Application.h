#pragma once
#include "Window.h"
#include "Time.h"

namespace andromeda {
	class Application {
	public:
		virtual const WindowOptions GetWindowOptions() const {
			return WindowOptions();
		}

		// Set up the application - returns a boolean indicating if initialization was successful
		virtual bool Initialize() {
			return true;
		}

#if ANDROMEDA_INTERNAL
		virtual void WindowEvent(SDL_Event& e) {}
		virtual void RawRender(graphics::Renderer& renderer) {}
#endif

		// Called when the application hits an update frame
		virtual void Update(float deltaTime) {}

		DEPRECATED // TBD if using
		virtual void FixedUpdate(float fixedDeltaTime) {}

		// Called when the application hits an update frame
		virtual void Render() {}

		// Called when the application is shutting down
		virtual void Shutdown() {}
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
        friend class Engine;

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
