#include "Engine/Engine.h"
#include "Engine/Application.h"
#include "spdlog/spdlog.h"
#include "Engine/Log.h"
#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"
#include "Engine/Graphics/OpenGL/OpenGLRenderer.h"
#include "Engine/Graphics/WebGPU/WebGPURenderer.h"

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

	if (Initialize()) {
		uint64_t lastFixedUpdate = SDL_GetTicks();

		while (m_isRunning) {
			uint64_t currentTicks = SDL_GetTicks();
			app->m_elapsedTime = currentTicks / 1000.0f;

			Update();

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
#if ANDROMEDA_EDITOR
		andromeda::initializeLogger(m_app->GetPersistentDataPath() / "AndromedaEditor.log");
#else
		andromeda::initializeLogger(m_app->GetPersistentDataPath() / "AndromedaGame.log");
#endif

		print("Engine:\t" ANDROMEDA_VERSION_STRING);

		if (m_currentAPI != graphics::Renderer::API::None) {
			switch (m_currentAPI) {
				case graphics::API::None:
					break;
				case graphics::Renderer::API::Vulkan:
					m_renderer = CreateScopeRef<graphics::VulkanRenderer>();
					break;
#if ANDROMEDA_OPENGL
				case graphics::API::OpenGL:
					m_renderer = CreateScopeRef<graphics::OpenGLRenderer>();
					break;
#endif
#if ANDROMEDA_WGPU
				case graphics::API::WGPU:
					m_renderer = CreateScopeRef<graphics::WGPURenderer>();
					break;
#endif
			}

			if (!m_app->Initialize())
				return false;

			// If no window, we can't really do anything lol
			auto main_window = m_app->GetMainWindow();
			if (main_window == nullptr) {
				andromeda::warn("Initialization was successful but no window was created");
				m_app->Shutdown();
				return false;
			}

			if (m_renderer != nullptr) {
				trace("Renderer: {}", m_renderer->GetAPIString());
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
	m_app->Update(m_app->m_deltaTime);
}

void Engine::FixedUpdate() {
	// m_app->FixedUpdate(m_app->m_fixedDeltaTime);
}

void Engine::Render() {
	auto mainWindow = m_app->GetMainWindow();
	if (mainWindow == nullptr)
		return;

	auto graphics = mainWindow->GetGraphicsContext();

	if (m_app->imgui != nullptr)
		m_app->imgui->NewFrame();
	graphics->Prepare();

	graphics->Render();

	if (m_app->imgui != nullptr) {
		m_app->DrawIMGUI();
		m_app->imgui->Render();
	}

	graphics->Present();
}

void Engine::Shutdown() {
	if (m_app->imgui != nullptr) {
		m_app->imgui->Shutdown();
		m_app->imgui.release();
	}

	m_app->Shutdown();
	m_app->CloseAllWindows();

	if (m_renderer != nullptr)
		m_renderer->Shutdown();
}