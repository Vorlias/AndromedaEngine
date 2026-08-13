#include "Engine/Graphics/OpenGL/OpenGLRenderer.h"
#if ANDROMEDA_OPENGL
#	include "glad/glad.h"
#	include "SDL3/SDL.h"
#	include "Engine/Log.h"
#	include "Engine/Graphics/OpenGL/OpenGLWindowContext.h"

using namespace andromeda::graphics;

bool OpenGLRenderer::Initialize() {
	print("Load OpenGLRenderer");

	return true;
}

void OpenGLRenderer::Clear() {}

void OpenGLRenderer::SetClearColor(Color color) {}

const std::string OpenGLRenderer::GetAPIString() const {
	return "OpenGL";
}

GraphicsContext* OpenGLRenderer::CreateWindowGraphicsContext(SDL_Window* window) {
	andromeda::print("Create OpenGL window context");
	return new OpenGLWindowContext(window);
}

void OpenGLRenderer::Shutdown() {}

OpenGLRenderer::~OpenGLRenderer() {}

#endif