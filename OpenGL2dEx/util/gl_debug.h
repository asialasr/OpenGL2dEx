#ifndef GL_DEBUG_H
#define GL_DEBUG_H

#include "logging.h"
#include <glad/glad.h>
#include <string>

//#define UTIL_GL_DEBUG

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
		bool gl_error_not_found = true;
		auto error = glGetError();
		while (error != GL_NO_ERROR)
		{
			LOG("OpenGL Error: " + std::string{ error_string(error) });
			gl_error_not_found = false;
			error = glGetError();
		}
		ASSERT(gl_error_not_found, "OpenGL Errors found and logged.  Exiting...");
	}

	static void APIENTRY gl_debug_output(GLenum source,
								  GLenum type,
								  GLuint id,
								  GLenum severity,
								  GLsizei length,
								  const GLchar *message,
								  const void *user_param)
	{
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
			return;

		std::string msg{ "Debug message (" + std::to_string(id) + "): " + message };

		switch(source)
		{
		case GL_DEBUG_SOURCE_API:
			msg += "\nSource: API";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   
			msg += "\nSource: Window System"; 
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: 
			msg += "Source: Shader Compiler"; 
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     
			msg += "\nSource: Third Party"; 
			break;
		case GL_DEBUG_SOURCE_APPLICATION:     
			msg += "\nSource: Application"; 
			break;
		case GL_DEBUG_SOURCE_OTHER:           
			msg += "\nSource: Other"; 
			break;
		default:
			msg += "\nSource: UNKNOWN";
			LOG("Unknown source type");
			break;
		}

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:               
			msg += "\nType: Error"; 
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: 
			msg += "\nType: Deprecated Behaviour"; 
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  
			msg += "\nType: Undefined Behaviour"; 
			break;
		case GL_DEBUG_TYPE_PORTABILITY:         
			msg += "\nType: Portability"; 
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:         
			msg += "\nType: Performance"; 
			break;
		case GL_DEBUG_TYPE_MARKER:              
			msg += "\nType: Marker"; 
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          
			msg += "\nType: Push Group"; 
			break;
		case GL_DEBUG_TYPE_POP_GROUP:           
			msg += "\nType: Pop Group"; 
			break;
		case GL_DEBUG_TYPE_OTHER:               
			msg += "\nType: Other"; 
			break;
		default:
			msg += "\nType: UNKNOWN";
			LOG("Unknown type");
			break;
		}

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         
			msg += "\nSeverity: high"; 
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:       
			msg += "\nSeverity: medium"; 
			break;
		case GL_DEBUG_SEVERITY_LOW:          
			msg += "\nSeverity: low"; 
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: 
			msg += "\nSeverity: notification"; 
			break;
		default:
			msg += "\nSeverity: UNKNOWN";
			LOG("Unknown Severity");
			break;
		}
		msg += "\n";

		LOG(msg);
	}

} // namespace util

#endif // GL_DEBUG_H