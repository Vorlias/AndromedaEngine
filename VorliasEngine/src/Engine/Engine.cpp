#include "Engine.h"
#include "Engine/Application.h"
#include "spdlog/spdlog.h"
#include "Engine/Log.h"
#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"

USING_ENGINE;

Engine* Engine::s_instance = nullptr;

void andromeda::Engine::SetGraphicsAPI(graphics::Renderer::API api) {
	using namespace graphics;

	if (m_isRunning && m_renderer != nullptr && m_renderer->GetAPI() != api) {
		// clean-up old API
		m_renderer->Shutdown();
		abort();
	}

	switch (api) {
		case Renderer::API::None:
			print("Using None");
			break;
		case Renderer::API::Vulkan:
			m_renderer = new VulkanRenderer();
			break;
		case Renderer::API::OpenGL:
			error("OpenGL is not currently supported");
			break;
	}

	m_currentAPI = api;
}

void Engine::Run(Application* app) {
	if (m_app != nullptr)
		return;
	m_app = app;


	print("Initializing Andromeda Application " ANDROMEDA_VERSION_STRING);
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
			m_main_window = new Window(m_app->GetWindowOptions());

            print("Using " + m_renderer->GetAPIString() + " graphics");

			if (!m_app->Initialize())
				return false;

			m_isInitialized = true;

			if (!m_main_window->Initialize(this->m_currentAPI)) {
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

	if (m_main_window != nullptr)
		delete m_main_window;
}