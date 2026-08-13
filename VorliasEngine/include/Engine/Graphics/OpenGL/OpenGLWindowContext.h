#pragma once
#include "Engine/Graphics/GraphicsContext.h"
#include "SDL3/SDL.h"

#if ANDROMEDA_OPENGL
namespace andromeda::graphics {
    class OpenGLWindowContext : public GraphicsContext {
    public:
        OpenGLWindowContext(SDL_Window* window);
        void Initialize() override;
		void Shutdown() override;
		void Resized(int width, int height) override;
    private:
        SDL_GLContext m_glContext{nullptr};
        SDL_Window* m_window;
    };
}
#endif