#include "Engine/Graphics/OpenGL/OpenGLWindowContext.h"
#include "Engine/Log.h"

#if ANDROMEDA_OPENGL
#	include "glad/glad.h"
using namespace andromeda::graphics;

OpenGLWindowContext::OpenGLWindowContext(SDL_Window* window) : m_window(window) {}

void OpenGLWindowContext::Initialize() {
	m_glContext = SDL_GL_CreateContext(m_window);

#	ifdef ANDROMEDA_PLATFORM_MAC
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#	endif
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	if (m_glContext == nullptr) {
		andromeda::error("Failed to create GL context");
		return;
	}

	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

    
    print("Loaded OpenGL for window");
}
void OpenGLWindowContext::Shutdown() {
	SDL_GL_DestroyContext(m_glContext);
	m_glContext = nullptr;
	m_window = nullptr;
}

void OpenGLWindowContext::Resized(int width, int height) {}
#endif