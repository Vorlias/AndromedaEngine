#pragma once
#include "Engine/Graphics/RendererAPI.h"

#if ANDROMEDA_OPENGL
namespace andromeda::graphics {
    class OpenGLRenderer : public Renderer {
    public:
		bool Initialize() override;
		void Clear() override;
		void SetClearColor(Color color) override;
		void Shutdown() override;
		const std::string GetAPIString() const override;
		inline Renderer::API GetAPI() override {
			return Renderer::API::OpenGL;
		}
        ~OpenGLRenderer() override;
        GraphicsContext* CreateWindowGraphicsContext(SDL_Window* window) override;
    private:
    };
}
#endif