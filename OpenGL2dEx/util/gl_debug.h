#ifndef GL_DEBUG_H
#define GL_DEBUG_H

#include "logging.h"
#include <glad/glad.h>
#include <string>

namespace util {

	static const char * error_string(GLenum error)
	{
		switch (error)
		{
		case GL_NO_ERROR:
			return "GL_NO_ERROR";
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";
		case GL_STACK_OVERFLOW:
			return "GL_STACK_OVERFLOW";
		case GL_STACK_UNDERFLOW:
			return "GL_STACK_UNDERFLOW";
		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY";
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
		default:
			return "UNKNOWN OpenGL Error";
		}
	}

	static void check_for_gl_errors()
	{
		const auto error = glGetError();
		ASSERT(error == GL_NO_ERROR, "OpenGL Error: " + std::string{ error_string(error) });
	}

} // namespace util

#endif // GL_DEBUG_H