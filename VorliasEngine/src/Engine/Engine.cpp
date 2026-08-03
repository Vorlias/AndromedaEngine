#include "Engine.h"
#include "Engine/Application.h"
#include "spdlog/spdlog.h"
#include "Engine/Log.h"
#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"

USING_ENGINE;

Engine* Engine::s_instance = nullptr;

void andromeda::Engine::SetGraphicsAPI(graphics::Renderer::API api) {
	using namespace graphics;

	if (m_renderer != nullptr && m_renderer->GetAPI() != api) {
		error("Cannot change graphics API at runtime (yet)");
		abort();
	}

	m_currentAPI = api;
}

void Engine::Run(Application* app) {
	if (m_app != nullptr)
		return;
	m_app = app;

#if defined(ANDROMEDA_DEBUG)
	spdlog::set_level(spdlog::level::trace);
#endif

	print("Initializing " ANDROMEDA_VERSION_STRING);
	if (Initialize()) {
		while (m_isRunning) {
			Update();
			if (m_renderer != nullptr)
				Render();
		}

		Shutdown();
	}
}

Engine& Engine::GetInstance() {
	if (s_instance == nullptr) {
		s_instance = new Engine();
	}

	return *s_instance;
}

Engine::Engine() : m_app(nullptr), m_main_window(nullptr) {}

bool Engine::Initialize() {
	if (!m_isInitialized) {
		if (m_currentAPI != graphics::Renderer::API::None) {
			m_main_window = CreateScopeRef<Window>(m_app->GetWindowOptions());

			if (!m_app->Initialize())
				return false;

			m_isInitialized = true;

			switch (m_currentAPI) {
				case graphics::API::None:
					break;
				case graphics::Renderer::API::Vulkan:
					m_renderer = CreateScopeRef<graphics::VulkanRenderer>(); // new graphics::VulkanRenderer();
					print("Using renderer " + m_renderer->GetAPIString());
					break;
				case graphics::API::OpenGL:
					break;
			}

			if (!m_main_window->Initialize(m_renderer.get())) {
				Shutdown();
				return false;
			}

			m_isRunning = true;
			return true;
		} else {
			print("Running Andromeda Application without a renderer (headless mode)");

			if (!m_app->Initialize())
				return false;
			m_isInitialized = true;
			m_isRunning = true;

			print("Running application in headless mode");
			return true;
		}
	}

	return false;
}

void Engine::Quit() {
	m_isRunning = false;
}

void Engine::Update() {
	if (m_main_window != nullptr) {
		SDL_Event e;
		while (m_main_window->PollSDLEvent(&e)) {
			switch (e.type) {
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
					Quit();
					break;
			}
		}
	}

	m_app->Update();
}

void Engine::Render() {
	m_app->Render();
}

void Engine::Shutdown() {
	m_app->Shutdown();
	m_main_window->Shutdown();

	if (m_renderer != nullptr)
		m_renderer->Shutdown();
}