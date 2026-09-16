#pragma once
#include "Engine/Window.h"
#include "Engine/Common.h"
#include <thread>

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

    enum class Platform {
		Unknown,
        Windows,
		MacOS,
		Linux,
		Android,
		iOS,
    };

	inline std::string PlatformToString(Platform platform) {
		switch (platform) {
			using enum Platform;
			case Windows:
				return "Windows";
			case MacOS:
				return "MacOS";
			case Linux:
				return "Linux";
			case Android:
				return "Android";
			case iOS:
				return "iOS";
			default:
				assert(false);
		}
	}

	inline Platform StringToPlatform(std::string_view value) {
		if (value == "Windows") return Platform::Windows;
		if (value == "MacOS") return Platform::MacOS;
		if (value == "Linux") return Platform::Linux;
		if (value == "Android") return Platform::Android;
		if (value == "iOS") return Platform::iOS;
		assert(false);
	}

	class Application;
	class Engine {
	public:
		static Engine& GetInstance();

		void SetGraphicsAPI(graphics::Renderer::API api);
		void Run(Application* app);
		void Quit();

		inline graphics::Renderer::API GetCurrentAPI() const { return m_currentAPI; }
		inline const ScopeRef<graphics::Renderer>& GetRenderer() const { return m_renderer; }
		SharedRef<Window> GetMainWindow() const;
	private:
		static Engine* s_instance;
		Engine();

        bool Initialize();
		void Update();
		void FixedUpdate();
		void Render();
        void Shutdown();

		Application* m_app = nullptr;

		bool m_isRunning = false;
		bool m_isInitialized = false;

		ScopeRef<graphics::Renderer> m_renderer = nullptr;

		uint32_t lastTime;

		graphics::Renderer::API m_currentAPI = graphics::Renderer::API::None;

		std::vector<std::thread> m_threads{};
	};
} // namespace ENGINE_NS