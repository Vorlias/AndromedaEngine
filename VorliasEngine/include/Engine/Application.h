#pragma once
#include "Window.h"
#include "Time.h"
#include <filesystem>
#include "File.h"
#include "IMGUI.h"
#include "Log.h"

#include "Engine/Events/Event.h"
#include "Engine/Graphics/RenderTarget.h"
// NOTE TO SELF: Don't use PREPROC defines for virtual methods

namespace andromeda {
	using EventDispatchFunction = std::function<void(Event&)>;

	class Application {
	public:
		// virtual const WindowOptions& GetWindowOptions() const {
		// 	return WindowOptions("AndromedaEngine", Vector2u(1024, 768), WindowFlags::Default);
		// }

		// Set up the application - returns a boolean indicating if initialization was successful
		virtual bool Initialize() = 0;

		// IMGUI step
		virtual void DrawIMGUI() {}

		// Called when the application hits an update frame
		virtual void Update(float deltaTime) {}

		// Called when the application is shutting down
		virtual void Shutdown() {}

		// virtual void WindowResized(int width, int height) {}
		// virtual void WindowMaximized() {}
		// virtual void WindowMinimized() {}
		// virtual void WindowRestored() {}

		virtual void Event(Event& e) {}

	protected:
		std::shared_ptr<Window> CreateWindow(const WindowOptions windowOptions);
		std::shared_ptr<Window> GetMainWindow() const;
		std::shared_ptr<Window> GetWindowById(WindowID id) const;

		void InitIMGUI();

		graphics::ShaderLibrary shaders;

		void SetDataPath(const std::filesystem::path& path) {
			m_dataPath = path;
		}

		void SetPersistentDataPath(const std::filesystem::path& path) {
			m_persistentDataPath = path;
		}

		std::unique_ptr<andromeda::IMGUI> imgui;

	public:
		// Gets the amount of time the application has been open
		[[nodiscard]] constexpr float GetElapsedTime() const;

		// Gets the current deltaTime of the application
		[[nodiscard]] constexpr float GetDeltaTime() const;

		// Gets the path to the data directory for the application
		[[nodiscard]] constexpr std::filesystem::path GetDataPath(bool absolute = false) const;

		// Gets the path to a data directory for persistent data for this application
		[[nodiscard]] constexpr std::filesystem::path GetPersistentDataPath() const;

		[[nodiscard]] constexpr std::filesystem::path GetFullPath(const std::filesystem::path& path) const;

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

		EventDispatchFunction m_dispatchFn;

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

	constexpr std::filesystem::path Application::GetFullPath(const std::filesystem::path& filePath) const {
		auto dataPathFull = GetDataPath(true);
		auto filePathFull = std::filesystem::absolute(filePath);

		const auto mismatch_pair = std::mismatch(filePathFull.begin(), filePathFull.end(), dataPathFull.begin(), dataPathFull.end());
		if (mismatch_pair.second == dataPathFull.end()) {
			return std::filesystem::relative(filePathFull, std::filesystem::current_path());
		}

		return filePath;
	}

	constexpr std::filesystem::path Application::GetPersistentDataPath() const {
		return std::filesystem::absolute(m_persistentDataPath);
	}
} // namespace andromeda
