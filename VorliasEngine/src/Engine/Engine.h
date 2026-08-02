#pragma once
#include "Engine/Window.h"
#include "Engine/Common.h"

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