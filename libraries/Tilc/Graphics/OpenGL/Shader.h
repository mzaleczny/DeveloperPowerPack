#pragma once

#define GLAD_GL_IMPLEMENTATION
#ifdef APIENTRY
# undef APIENTRY
#endif
#include <glad/glad.h>
#ifdef APIENTRY
# undef APIENTRY
#endif
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
            glBindProgramPipeline(m_Pipeline);
        }
        // utility uniform functions
        // ------------------------------------------------------------------------
        void SetBool(GLuint Shader, const std::string& name, bool value) const
        {
            glProgramUniform1i(Shader, glGetUniformLocation(Shader, name.c_str()), (int)value);
        }
        // ------------------------------------------------------------------------
        void SetInt(GLuint Shader, const std::string& name, int value) const
        {
            glProgramUniform1i(Shader, glGetUniformLocation(Shader, name.c_str()), value);
        }
        // ------------------------------------------------------------------------
        void SetUInt(GLuint Shader, const std::string& name, unsigned int value) const
        {
            glProgramUniform1ui(Shader, glGetUniformLocation(Shader, name.c_str()), value);
        }
        // ------------------------------------------------------------------------
        void SetFloat(GLuint Shader, const std::string& name, float value) const
        {
            glProgramUniform1f(Shader, glGetUniformLocation(Shader, name.c_str()), value);
        }
        // ------------------------------------------------------------------------
        void SetVec2(GLuint Shader, const std::string& name, const glm::vec2& value) const
        {
            glProgramUniform2fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, &value[0]);
        }
        void SetVec2(GLuint Shader, const std::string& name, const Tilc::TVector2D<float>& value) const
        {
            glProgramUniform2fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, value.Coord);
        }
        void SetVec2(GLuint Shader, const std::string& name, float x, float y) const
        {
            glProgramUniform2f(Shader, glGetUniformLocation(Shader, name.c_str()), x, y);
        }
        // ------------------------------------------------------------------------
        void SetVec3(GLuint Shader, const std::string& name, const glm::vec3& value) const
        {
            glProgramUniform3fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, &value[0]);
        }
        void SetVec3(GLuint Shader, const std::string& name, const Tilc::TVector3D<float>& value) const
        {
            glProgramUniform3fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, value.Coord);
        }
        void SetVec3(GLuint Shader, const std::string& name, float x, float y, float z) const
        {
            glProgramUniform3f(Shader, glGetUniformLocation(Shader, name.c_str()), x, y, z);
        }
        // ------------------------------------------------------------------------
        void SetVec4(GLuint Shader, const std::string& name, const glm::vec4& value) const
        {
            glProgramUniform4fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, &value[0]);
        }
        void SetVec4(GLuint Shader, const std::string& name, const Tilc::TVector4D<float>& value) const
        {
            glProgramUniform4fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, value.Coord);
        }
        void SetVec4(GLuint Shader, const std::string& name, float x, float y, float z, float w)
        {
            glProgramUniform4f(Shader, glGetUniformLocation(Shader, name.c_str()), x, y, z, w);
        }
        // ------------------------------------------------------------------------
        void SetMat2(GLuint Shader, const std::string& name, const glm::mat2& mat) const
        {
            glProgramUniformMatrix2fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        void SetMat2(GLuint Shader, const std::string& name, const float* mat, bool Transpose = true) const
        {
            glProgramUniformMatrix2fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, Transpose, mat);
        }
        // ------------------------------------------------------------------------
        void SetMat3(GLuint Shader, const std::string& name, const glm::mat3& mat) const
        {
            glProgramUniformMatrix3fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        void SetMat3(GLuint Shader, const std::string& name, const float* mat, bool Transpose = true) const
        {
            glProgramUniformMatrix3fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, Transpose, mat);
        }
        // ------------------------------------------------------------------------
        void SetMat4(GLuint Shader, const std::string& name, const glm::mat4& mat) const
        {
            glProgramUniformMatrix4fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        void SetMat4(GLuint Shader, const std::string& name, const float* mat, bool Transpose = true) const
        {
            glProgramUniformMatrix4fv(Shader, glGetUniformLocation(Shader, name.c_str()), 1, Transpose, mat);
        }
        void SetMat4Array(GLuint Shader, const std::string& name, const float* mat, int count, bool Transpose = true) const
        {
            glProgramUniformMatrix4fv(Shader, glGetUniformLocation(Shader, name.c_str()), count, Transpose, mat);
        }
    };

    GLuint DECLSPEC CompileShader(const GLchar* SourceCode, GLenum Stage, const std::string& CompilationMessage);
}
