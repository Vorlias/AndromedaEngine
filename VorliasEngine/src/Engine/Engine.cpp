#include "Engine.h"
#include "Engine/Application.h"
#include "spdlog/spdlog.h"
#include "Engine/Log.h"
USING_ENGINE;

Engine* Engine::s_instance = nullptr;

void Engine::Run(Application* app) {
    if (m_app != nullptr) return;
    m_app = app;

    
    if (Initialize()) {
        print("Initialized " ANDROMEDA_VERSION_STRING);
        
        while (m_isRunning) {
            Update();
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

Engine::Engine() : m_app(nullptr) {}

bool Engine::Initialize() {
    if (!m_isInitialized) {
        m_isInitialized = true;
        m_isRunning = true;
        return m_app->Initialize();
    }

    return false;
}

void Engine::Update() {
    m_app->Update();

    // Nothing happens worth keeping the application open right now
    m_isRunning = false;
}

void Engine::Render() {
    m_app->Render();
}

void Engine::Shutdown() {
    m_app->Shutdown();
}