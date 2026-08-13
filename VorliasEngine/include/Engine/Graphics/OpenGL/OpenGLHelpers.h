#pragma once
#if ANDROMEDA_OPENGL
#include "glad/glad.h"
#include "spdlog/spdlog.h"

namespace andromeda::graphics {
	inline bool CheckGLError() {
		GLenum error = glGetError();
		bool shouldAssert = error != GL_NO_ERROR;

		while (error != GL_NO_ERROR) {
			std::string errText;
			switch (error) {
				case GL_INVALID_OPERATION: {
					errText = "GL_INVALID_OPERATION";
					break;
				}
				case GL_INVALID_ENUM: {
					errText = "GL_INVALID_ENUM";
					break;
				}
				case GL_INVALID_VALUE: {
					errText = "GL_INVALID_VALUE";
					break;
				}
				case GL_OUT_OF_MEMORY: {
					errText = "GL_OUT_OF_MEMORY";
					break;
				}
				case GL_INVALID_FRAMEBUFFER_OPERATION: {
					errText = "GL_INVALID_FRAMEBUFFER_OPERATION";
					break;
				}
				default:
					errText = std::to_string(error);
					break;
			}


			spdlog::error("OpenGL Error: {}", errText.c_str());
			error = glGetError();
			ANDROMEDA_ASSERT(false);
		}

		return shouldAssert;
	}
} // namespace andromeda::graphics


#	if ANDROMEDA_DEBUG
#		define ANDROMEDA_CHECK_GL_ERROR \
			{ \
				bool hasGLError = andromeda::graphics::CheckGLError(); \
				ANDROMEDA_ASSERT(!hasGLError); \
			}
#	else
#		define ANDROMEDA_CHECK_GL_ERROR (void)0
#	endif

#else
#	define ANDROMEDA_CHECK_GL_ERROR (void)0
#endif