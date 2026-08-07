#include "Engine/Engine.h"
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

static Time defaultFrameTime = milliseconds(5); // 200hz

void Engine::Run(Application* app) {
	if (m_app != nullptr)
		return;
	m_app = app;

#if defined(ANDROMEDA_DEBUG)
	spdlog::set_level(spdlog::level::trace);
#endif

	print("Initializing " ANDROMEDA_VERSION_STRING);
	if (Initialize()) {
		uint64_t lastFixedUpdate = SDL_GetTicks();

		while (m_isRunning) {
			uint64_t currentTicks = SDL_GetTicks();
			app->m_elapsedTime = currentTicks / 1000.0f;

			Update();

			// uint64_t nextFixedUpdate = SDL_GetTicks() - lastFixedUpdate;
			// if (nextFixedUpdate >= app->m_fixedFrameTime.toMilliseconds()) {
			// 	FixedUpdate();
			// 	app->m_fixedDeltaTime = (nextFixedUpdate / 1000.0f);
			// 	lastFixedUpdate = SDL_GetTicks();
			// }

			if (m_renderer != nullptr)
				Render();

			if (app->m_frameTime != Time::Zero) {
				sleep(app->m_frameTime);
			} else {
				sleep(defaultFrameTime);
			}

			uint64_t deltaTime = SDL_GetTicks() - currentTicks;
			app->m_deltaTime = (deltaTime / 1000.0f);

			if (app->m_quitRequested)
				Quit();
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

Engine::Engine() : m_app(nullptr) {}

std::shared_ptr<Window> Engine::GetMainWindow() const {
	return m_app->GetMainWindow();
}

bool Engine::Initialize() {
	if (!m_isInitialized) {
		if (m_currentAPI != graphics::Renderer::API::None) {
			switch (m_currentAPI) {
				case graphics::API::None:
					break;
				case graphics::Renderer::API::Vulkan:
					m_renderer = CreateScopeRef<graphics::VulkanRenderer>(); // new graphics::VulkanRenderer();
					print("Using renderer " + m_renderer->GetAPIString());
					break;
#if ANDROMEDA_OPENGL
				case graphics::API::OpenGL:
					break;
#endif
			}

			if (!m_app->Initialize())
				return false;

			// If no window, we can't really do anything lol
			auto main_window = m_app->GetMainWindow();
			if (main_window == nullptr) {
				andromeda::warn("No main window set, shutting down...");
				m_app->Shutdown();
				return false;
			}


			m_isInitialized = true;
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
	m_app->UpdateWindows();

	// 	if (m_main_window != nullptr) {
	// 		SDL_Event e;
	// 		while (m_main_window->PollSDLEvent(&e)) {
	// 			switch (e.type) {
	// 				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
	// 					Quit();
	// 					break;
	// 				case SDL_EVENT_WINDOW_RESIZED:
	// 					m_main_window->Resized(e.window.data1, e.window.data2);
	// 					andromeda::trace("Resized to " + std::to_string(e.window.data1) + "x" + std::to_string(e.window.data2));
	// 					break;
	// 			}

	// #if ANDROMEDA_INTERNAL
	// 			m_app->WindowEvent(e);
	// #endif
	// 		}
	// 	}
	m_app->Update(m_app->m_deltaTime);
}

void Engine::FixedUpdate() {
	// m_app->FixedUpdate(m_app->m_fixedDeltaTime);
}

void Engine::Render() {
#if ANDROMEDA_INTERNAL
	m_app->RawRender(*m_renderer);
#endif
	m_app->Render();
}

void Engine::Shutdown() {
	m_app->Shutdown();
	m_app->CloseAllWindows();

	if (m_renderer != nullptr)
		m_renderer->Shutdown();
}