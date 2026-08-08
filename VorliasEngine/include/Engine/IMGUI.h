#include "Engine/Graphics/RendererAPI.h"
#include "Engine/Window.h"

struct SDL_Window;
namespace andromeda {
    class ImWindowContext {
    public:
        static SharedRef<ImWindowContext> Create(graphics::Renderer* renderer);
        virtual bool Initialize(Window& window) = 0;
        void Shutdown();

        virtual void NewFrame() = 0;
        virtual void Render() = 0;
    protected: 
        virtual void CleanupContext() = 0;
    private:
        bool m_cleanup;
        SDL_Window* m_window;
        graphics::Renderer* m_renderer;
    };
}