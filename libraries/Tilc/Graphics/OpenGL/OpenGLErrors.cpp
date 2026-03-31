#include "OpenGLErrors.h"
#include <SDL3/SDL.h>
#ifdef APIENTRY
# undef APIENTRY
#endif
#include <glad/glad.h>
#ifdef APIENTRY
# undef APIENTRY
#endif

void PrintOpenGLError(const std::string& PrefixString)
{
	GLenum GLErrorCode;
	while ((GLErrorCode = glGetError()) != GL_NO_ERROR)
	{
        std::string ErrorStr;

        switch (GLErrorCode) {
        case GL_INVALID_OPERATION:      ErrorStr = "INVALID_OPERATION";      break;
        case GL_INVALID_ENUM:           ErrorStr = "INVALID_ENUM";           break;
        case GL_INVALID_VALUE:          ErrorStr = "INVALID_VALUE";          break;
        case GL_OUT_OF_MEMORY:          ErrorStr = "OUT_OF_MEMORY";          break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:  ErrorStr = "INVALID_FRAMEBUFFER_OPERATION";  break;
        }
		SDL_Log("%s OpenGL error %d: %s", PrefixString.c_str(), GLErrorCode, ErrorStr.c_str());
	}
}