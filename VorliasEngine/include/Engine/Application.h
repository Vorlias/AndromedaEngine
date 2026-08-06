#pragma once
#include "Window.h"
#include "Time.h"

namespace andromeda {
	class Application {
	public:
		virtual const WindowOptions GetWindowOptions() const {
			return WindowOptions();
		}

		virtual bool Initialize() {
			return true;
		}
		virtual void Update() {
#if ANDROMEDA_EDITOR
			std::cout << "Editor" << std::endl;
#endif
		}
		virtual void Render() {}
		virtual void Shutdown() {}

		virtual graphics::Renderer::API GetGraphicsAPI() const {
			return graphics::Renderer::API::None;
		}

		virtual void ApplicationQuit() {}
        
        float GetElapsedTime() const;
		float GetDeltaTime() const;

		void SetFramerate(uint32_t limit);
	private:
        friend class Engine;
        Time frameTime;

		float deltaTime;
		float elapsedTime;
	};
} // namespace andromeda
