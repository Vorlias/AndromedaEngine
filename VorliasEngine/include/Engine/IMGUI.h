#include "Engine/Graphics/RendererAPI.h"
#include "Engine/Window.h"

struct SDL_Window;
namespace andromeda {
    class ImWindow {
        static const ImWindow& Initialize(Window& window);
    private:
        SDL_Window* m_window;
        graphics::Renderer* m_renderer;
    };
}