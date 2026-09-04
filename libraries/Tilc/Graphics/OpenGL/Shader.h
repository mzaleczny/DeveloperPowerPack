#pragma once

#include "Tilc/configure.h"

#include "Tilc/configure.h"
#include "Tilc/OpenGL.h"
#include <glm/glm.hpp>
#include <string>
#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/Vector.h"


namespace Tilc::Graphics::OpenGL
{
    class DECLSPEC TPipeline
    {
    public:
        GLuint m_VertexShader = 0;
        GLuint m_GeometryShader = 0;
        GLuint m_FragmentShader = 0;
        GLuint m_ComputeShader = 0;
        unsigned int m_Pipeline = 0;

        TPipeline(const char* VertexPathOrData = nullptr, const char* FragmentPathOrData = nullptr, const char* GeometryPathOrData = nullptr);
        TPipeline(const std::string& VertexPathOrData, const std::string& FragmentPathOrData, const std::string& GeometryPathOrData);
        ~TPipeline();

        void Load(const char* VertexPathOrData, const char* FragmentPathOrData, const char* GeometryPathOrData = nullptr);
        void Reload(const char* VertexPathOrData, const char* FragmentPathOrData, const char* GeometryPathOrData = nullptr);
        void LoadComputeShader(const char* PathOrData);
        void Destroy();

        // activate the shader
        // ------------------------------------------------------------------------
        void Bind()
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(m_Pipeline);
#else
            glBindProgramPipeline(m_Pipeline);
#endif
        }
        // utility uniform functions
        // ------------------------------------------------------------------------
        void SetBool(GLuint Shader, const std::string& name, bool value) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniform1i(glGetUniformLocation(Shader, name.c_str()), (int)value);
#else
            glProgramUniform1i(Shader, glGetUniformLocation(Shader, name.c_str()), (int)value);
#endif
        }
        // ------------------------------------------------------------------------
        void SetInt(GLuint Shader, const std::string& name, int value) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniform1i(glGetUniformLocation(Shader, name.c_str()), value);
#else
            glProgramUniform1i(Shader, glGetUniformLocation(Shader, name.c_str()), value);
#endif
        }
        // ------------------------------------------------------------------------
        void SetUInt(GLuint Shader, const std::string& name, unsigned int value) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniform1ui(glGetUniformLocation(Shader, name.c_str()), value);
#else
            glProgramUniform1ui(Shader, glGetUniformLocation(Shader, name.c_str()), value);
#endif
        }
        // ------------------------------------------------------------------------
        void SetFloat(GLuint Shader, const std::string& name, float value) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniform1f(glGetUniformLocation(Shader, name.c_str()), value);
#else
            glProgramUniform1f(Shader, glGetUniformLocation(Shader, name.c_str()), value);
#endif
        }
        // ------------------------------------------------------------------------
        void SetVec2(GLuint Shader, const std::string& name, const glm::vec2& value) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniform2fv(glGetUniformLocation(Shader, name.c_str()), 1, &value[0]);
#else
            glProgramUniform2fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, &value[0]);
#endif
        }
        void SetVec2(GLuint Shader, const std::string& name, const Tilc::TVector2D<float>& value) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniform2fv(glGetUniformLocation(Shader, name.c_str()), 1, value.Coord);
#else
            glProgramUniform2fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, value.Coord);
#endif
        }
        void SetVec2(GLuint Shader, const std::string& name, float x, float y) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniform2f(glGetUniformLocation(Shader, name.c_str()), x, y);
#else
            glProgramUniform2f(Shader, glGetUniformLocation(Shader, name.c_str()), x, y);
#endif
        }
        // ------------------------------------------------------------------------
        void SetVec3(GLuint Shader, const std::string& name, const glm::vec3& value) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniform3fv(glGetUniformLocation(Shader, name.c_str()), 1, &value[0]);
#else
            glProgramUniform3fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, &value[0]);
#endif
        }
        void SetVec3(GLuint Shader, const std::string& name, const Tilc::TVector3D<float>& value) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniform3fv(glGetUniformLocation(Shader, name.c_str()), 1, value.Coord);
#else
            glProgramUniform3fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, value.Coord);
#endif
        }
        void SetVec3(GLuint Shader, const std::string& name, float x, float y, float z) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniform3f(glGetUniformLocation(Shader, name.c_str()), x, y, z);
#else
            glProgramUniform3f(Shader, glGetUniformLocation(Shader, name.c_str()), x, y, z);
#endif
        }
        // ------------------------------------------------------------------------
        void SetVec4(GLuint Shader, const std::string& name, const glm::vec4& value) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniform4fv(glGetUniformLocation(Shader, name.c_str()), 1, &value[0]);
#else
            glProgramUniform4fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, &value[0]);
#endif
        }
        void SetVec4(GLuint Shader, const std::string& name, const Tilc::TVector4D<float>& value) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniform4fv(glGetUniformLocation(Shader, name.c_str()), 1, value.Coord);
#else
            glProgramUniform4fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, value.Coord);
#endif
        }
        void SetVec4(GLuint Shader, const std::string& name, float x, float y, float z, float w)
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniform4f(glGetUniformLocation(Shader, name.c_str()), x, y, z, w);
#else
            glProgramUniform4f(Shader, glGetUniformLocation(Shader, name.c_str()), x, y, z, w);
#endif
        }
        // ------------------------------------------------------------------------
        void SetMat2(GLuint Shader, const std::string& name, const glm::mat2& mat) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniformMatrix2fv(glGetUniformLocation(Shader, name.c_str()), 1, GL_FALSE, &mat[0][0]);
#else
            glProgramUniformMatrix2fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, GL_FALSE, &mat[0][0]);
#endif
        }
        void SetMat2(GLuint Shader, const std::string& name, const float* mat, bool Transpose = true) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniformMatrix2fv(glGetUniformLocation(Shader, name.c_str()), 1, Transpose, mat);
#else
            glProgramUniformMatrix2fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, Transpose, mat);
#endif
        }
        // ------------------------------------------------------------------------
        void SetMat3(GLuint Shader, const std::string& name, const glm::mat3& mat) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniformMatrix3fv(glGetUniformLocation(Shader, name.c_str()), 1, GL_FALSE, &mat[0][0]);
#else
            glProgramUniformMatrix3fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, GL_FALSE, &mat[0][0]);
#endif
        }
        void SetMat3(GLuint Shader, const std::string& name, const float* mat, bool Transpose = true) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniformMatrix3fv(glGetUniformLocation(Shader, name.c_str()), 1, Transpose, mat);
#else
            glProgramUniformMatrix3fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, Transpose, mat);
#endif
        }
        // ------------------------------------------------------------------------
        void SetMat4(GLuint Shader, const std::string& name, const glm::mat4& mat) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniformMatrix4fv(glGetUniformLocation(Shader, name.c_str()), 1, GL_FALSE, &mat[0][0]);
#else
            glProgramUniformMatrix4fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, GL_FALSE, &mat[0][0]);
#endif
        }
        void SetMat4(GLuint Shader, const std::string& name, const float* mat, bool Transpose = true) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniformMatrix4fv(glGetUniformLocation(Shader, name.c_str()), 1, Transpose, mat);
#else
            glProgramUniformMatrix4fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, Transpose, mat);
#endif
        }
        void SetMat4Array(GLuint Shader, const std::string& name, const float* mat, int count, bool Transpose = true) const
        {
#if FORCE_OPENGL_ES == 1
            glUseProgram(Shader);
            glUniformMatrix4fv(glGetUniformLocation(Shader, name.c_str()), count, Transpose, mat);
#else
            glProgramUniformMatrix4fv(Shader, glGetUniformLocation(Shader, name.c_str()), count, Transpose, mat);
#endif
        }
    };

    GLuint DECLSPEC CompileShader(const GLchar* SourceCode, GLenum Stage, const std::string& CompilationMessage);
}
