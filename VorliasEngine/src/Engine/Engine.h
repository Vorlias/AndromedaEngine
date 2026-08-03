#pragma once
#include "Engine/Window.h"
#include "Engine/Common.h"

// #define ANDROMEDA_MEMORY_DEBUG
#ifdef ANDROMEDA_MEMORY_DEBUG
static int s_allocationCount = 0;

void* operator new(size_t size) {
	s_allocationCount += 1;

	std::cout << s_allocationCount << " allocations" << std::endl;
	return malloc(size);
}

void operator delete(void* p) {
	s_allocationCount -= 1;
	free(p);
}
#endif

namespace ENGINE_NS {
	namespace graphics {
		class GraphicsContext;
		class Renderer;
	}

	class Application;
	class Engine {
	public:
		static Engine& GetInstance();

		void SetGraphicsAPI(graphics::Renderer::API api);
		void Run(Application* app);
		void Quit();

	private:
		static Engine* s_instance;
		Engine();

        bool Initialize();
		void Update();
		void Render();
        void Shutdown();

		Application* m_app = nullptr;

		bool m_isRunning = false;
		bool m_isInitialized = false;

		ScopeRef<Window> m_main_window = nullptr;
		ScopeRef<graphics::Renderer> m_renderer = nullptr;

		graphics::Renderer::API m_currentAPI = graphics::Renderer::API::None;
	};
} // namespace ENGINE_NS