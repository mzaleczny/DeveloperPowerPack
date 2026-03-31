#include "Tilc/Graphics/OpenGL/Shader.h"
#include "Tilc/Game.h"
#include "Tilc/Utils/FileUtils.h"
#include "Tilc/Utils/ExtString.h"
#include <fstream>
#include <sstream>
#include <iostream>

#include "SDL3/SDL.h"


Tilc::Graphics::OpenGL::TPipeline::TPipeline(const char* VertexPathOrData, const char* FragmentPathOrData, const char* GeometryPathOrData)
{
    Load(VertexPathOrData, FragmentPathOrData, GeometryPathOrData);
}

Tilc::Graphics::OpenGL::TPipeline::TPipeline(const std::string& VertexPathOrData, const std::string& FragmentPathOrData, const std::string& GeometryPathOrData)
{
    Load(VertexPathOrData.c_str(), FragmentPathOrData.c_str(), GeometryPathOrData.c_str());
}

Tilc::Graphics::OpenGL::TPipeline::~TPipeline()
{
    Destroy();
}

// generates the shader on the fly
// ------------------------------------------------------------------------
void Tilc::Graphics::OpenGL::TPipeline::Load(const char* VertexPathOrData, const char* FragmentPathOrData, const char* GeometryPathOrData)
{
    bool HasVertexShader = (VertexPathOrData && strcmp(VertexPathOrData, "") != 0);
    bool HasFragmentShader = (FragmentPathOrData && strcmp(FragmentPathOrData, "") != 0);
    bool HasGeometryShader = (GeometryPathOrData && strcmp(GeometryPathOrData, "") != 0);
    const GLchar* CodePointer = nullptr;
    // 1. retrieve the vertex/fragment source code from filePath
    Tilc::TExtString Code;
    try 
    {
        // if vertex shader is present, load it
        if (HasVertexShader)
        {
            if (strstr(VertexPathOrData, "#version"))
            {
                Code.assign(VertexPathOrData);
            }
            else
            {
                Tilc::TFile File(VertexPathOrData);
                File.ReadContent(Code);
            }
            CodePointer = Code.c_str();
            m_VertexShader = CompileShader(CodePointer, GL_VERTEX_SHADER, "VERTEX SHADER COMPILATION LOG");
        }

        // if fragment shader is present, load it
        if (HasFragmentShader)
        {
            if (strstr(FragmentPathOrData, "#version"))
            {
                Code.assign(FragmentPathOrData);
            }
            else
            {
                Tilc::TFile File(FragmentPathOrData);
                File.ReadContent(Code);
            }
            Tilc::GameObject->ApplyLightingStuffToShaderCode(Code);
            CodePointer = Code.c_str();
            m_FragmentShader = CompileShader(CodePointer, GL_FRAGMENT_SHADER, "FRAGMENT SHADER COMPILATION LOG");
        }

        // if geometry shader is present, load it
        if (HasGeometryShader)
        {
            if (strstr(GeometryPathOrData, "#version"))
            {
                Code.assign(GeometryPathOrData);
            }
            else
            {
                Tilc::TFile File(GeometryPathOrData);
                File.ReadContent(Code);
            }
            CodePointer = Code.c_str();
            m_GeometryShader = CompileShader(CodePointer, GL_GEOMETRY_SHADER, "GEOMETRY SHADER COMPILATION LOG");
        }

        // shader Program
        glCreateProgramPipelines(1, &m_Pipeline);
        if (HasVertexShader)
        {
            glUseProgramStages(m_Pipeline, GL_VERTEX_SHADER_BIT, m_VertexShader);
        }
        if (HasFragmentShader)
        {
            glUseProgramStages(m_Pipeline, GL_FRAGMENT_SHADER_BIT, m_FragmentShader);
        }
        if (HasGeometryShader)
        {
            glUseProgramStages(m_Pipeline, GL_GEOMETRY_SHADER_BIT, m_GeometryShader);
        }
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ::" << e.what() << std::endl;
    }
}

void Tilc::Graphics::OpenGL::TPipeline::Reload(const char* VertexPathOrData, const char* FragmentPathOrData, const char* GeometryPathOrData)
{
    Destroy();
    Load(VertexPathOrData, FragmentPathOrData, GeometryPathOrData);
}

void Tilc::Graphics::OpenGL::TPipeline::LoadComputeShader(const char* PathOrData)
{
    Tilc::TExtString Code;
    const GLchar* CodePointer = nullptr;
    try
    {
        if (strstr(PathOrData, "#version"))
        {
            Code.assign(PathOrData);
        }
        else
        {
            Tilc::TFile File(PathOrData);
            File.ReadContent(Code);
        }
        CodePointer = Code.c_str();
        m_ComputeShader = CompileShader(CodePointer, GL_COMPUTE_SHADER, "COMPUTE SHADER COMPILATION LOG");
        glUseProgramStages(m_Pipeline, GL_COMPUTE_SHADER_BIT, m_ComputeShader);
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ::" << e.what() << std::endl;
    }
}

void Tilc::Graphics::OpenGL::TPipeline::Destroy()
{
    if (m_Pipeline)
    {
        glDeleteProgramPipelines(1, &m_Pipeline);
        m_Pipeline = 0;
    }
    if (m_FragmentShader)
    {
        glDeleteProgram(m_FragmentShader);
    }
    if (m_GeometryShader)
    {
        glDeleteProgram(m_GeometryShader);
    }
    if (m_VertexShader)
    {
        glDeleteProgram(m_VertexShader);
    }
    if (m_ComputeShader)
    {
        glDeleteProgram(m_ComputeShader);
    }
}

GLuint Tilc::Graphics::OpenGL::CompileShader(const GLchar* SourceCode, GLenum Stage, const std::string& CompilationMessage)
{
    GLuint ShaderProgram = glCreateShaderProgramv(Stage, 1, &SourceCode);
    std::string CompilationLog;
    CompilationLog.resize(2048);
    glGetProgramInfoLog(ShaderProgram, CompilationLog.size(), nullptr, CompilationLog.data());
    if (CompilationLog[0] != 0)
    {
        std::cout << CompilationMessage << ": " << CompilationLog << std::endl;
    }
    return ShaderProgram;
}
