#pragma once
#include "Window.h"
#include "Time.h"
#include <filesystem>
#include "File.h"

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

		graphics::ShaderLibrary shaders;

		void SetDataPath(const std::filesystem::path& path) {
			m_dataPath = path;
		}

		void SetPersistentDataPath(const std::filesystem::path& path) {
			m_persistentDataPath = path;
		}

	public:
		// Gets the amount of time the application has been open
		[[nodiscard]] constexpr float GetElapsedTime() const;

		// Gets the current deltaTime of the application
		[[nodiscard]] constexpr float GetDeltaTime() const;

		// Gets the path to the data directory for the application
		[[nodiscard]] constexpr std::filesystem::path GetDataPath(bool absolute = false) const;

		// Gets the path to a data directory for persistent data for this application
		[[nodiscard]] constexpr std::filesystem::path GetPersistentDataPath() const;

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

		std::filesystem::path m_dataPath = GetDirectory(DirectoryType::ApplicationData);
		std::filesystem::path m_persistentDataPath = GetDirectory(DirectoryType::PersistentData);

		Time m_frameTime;
		float m_deltaTime;

		Time m_fixedFrameTime = milliseconds(33);
		float m_fixedDeltaTime;

		float m_elapsedTime;

		bool m_quitRequested;

		friend class Engine;
	};

	constexpr float Application::GetElapsedTime() const {
		return m_elapsedTime;
	}

	constexpr float Application::GetDeltaTime() const {
		return m_deltaTime;
	}

	constexpr std::filesystem::path Application::GetDataPath(bool absolute) const {
		if (absolute) {
			return std::filesystem::absolute(m_dataPath);
		} else {
			return std::filesystem::relative(m_dataPath, std::filesystem::current_path());
		}
	}

	constexpr std::filesystem::path Application::GetPersistentDataPath() const {
		return std::filesystem::absolute(m_persistentDataPath);
	}
} // namespace andromeda
