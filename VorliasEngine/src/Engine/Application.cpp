#include "Engine/Application.h"
#include "Engine/Log.h"
#include "Engine/Engine.h"
#include "Engine/Graphics/Vulkan/VulkanIMGUI.h"
#define DISALLOW_MULTI_WINDOWS 1

namespace andromeda {
	SharedRef<Window> Application::GetMainWindow() const {
		return m_main_window;
	}

	SharedRef<Window> Application::CreateWindow(const WindowOptions& windowOptions) {
		// temporary for now
#if DISALLOW_MULTI_WINDOWS
		if (m_main_window != nullptr) {
			andromeda::error("Cannot create more than one window currently.");
			return nullptr;
		}
#endif

		SharedRef<Window> window = std::make_shared<Window>(windowOptions);
		auto& engine = Engine::GetInstance();
		auto& renderer = engine.GetRenderer();

		if (!window->Initialize(renderer.get())) {
			window->Shutdown();
			return nullptr;
		}

		if (m_main_window == nullptr) {
			m_main_window = window;
		}
		m_windows.push_back(window);
		return window;
	}

	SharedRef<Window> Application::GetWindowById(WindowID id) const {
		for (auto window : m_windows) {
			if (window->GetWindowId() == id)
				return window;
		}

		return nullptr;
	}

	void Application::UpdateWindows() {
		if (m_main_window != nullptr) {
			SDL_Event e;
			while (m_main_window->PollSDLEvent(&e)) {
				if (imgui) imgui->ProcessEvent(e);

				switch (e.type) {
					case SDL_EVENT_QUIT:
					case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
						if (e.window.windowID == m_main_window->GetWindowId()) {
							shaders.UnloadAllShaders();
							CloseAllWindows();
							Quit();
						} else {
							auto window = GetWindowById(e.window.windowID);
							if (window != nullptr) {
								window->Close();
							}
						}

						break;
					case SDL_EVENT_WINDOW_RESIZED:
						m_main_window->Resized(e.window.data1, e.window.data2);
						if (imgui != nullptr) imgui->Resize(e.window.data1, e.window.data2);
						break;
				}
			}

			for (auto window : m_windows) {
				if (window->HasRequestedExit()) {
					window->Shutdown();
					m_windows.erase(std::remove(m_windows.begin(), m_windows.end(), window), m_windows.end());
				}
				
			}
		}
	}

	void Application::InitIMGUI() {
		auto& engine = Engine::GetInstance();
		auto& renderer = engine.GetRenderer();
		switch (renderer->GetAPI()) {
			case andromeda::graphics::API::Vulkan: {
				auto vulkan = static_cast<graphics::VulkanRenderer*>(renderer.get());
				auto windowContext = static_cast<graphics::VulkanWindowContext*>(m_main_window->GetGraphicsContext());

				std::unique_ptr<andromeda::IMGUI> imguiPtr(
					(andromeda::IMGUI*)new VulkanIMGUI(m_main_window->GetHandle(), vulkan->GetContext(), windowContext)
				);
				imgui = std::move(imguiPtr);

				break;
			}
		}

		if (imgui != nullptr) {
			imgui->Initialize();
		}
	}

	void Application::SetFramerateLimit(uint32_t limit) {
		if (limit <= 0) {
			m_frameTime = Time::Zero;
			return;
		}

		m_frameTime = (seconds(1.f / static_cast<float>(limit)));
	}

	uint32_t Application::GetFramerateLimit() const {
		return 1.f / m_frameTime.toSeconds();
	}

	void Application::Quit() {
		m_quitRequested = true;
	}
} // namespace andromeda