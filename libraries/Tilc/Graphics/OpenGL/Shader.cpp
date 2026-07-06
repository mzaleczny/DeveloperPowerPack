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
#if FORCE_OPENGL_ES != 1
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
#else
        m_Pipeline = glCreateProgram();

        if (HasVertexShader)
        {
            glAttachShader(m_Pipeline, m_VertexShader);
        }
        if (HasFragmentShader)
        {
            glAttachShader(m_Pipeline, m_FragmentShader);
        }
        // Geometry shader is NOT supported in GLES 3.0
        glLinkProgram(m_Pipeline);

        GLint success = 0;
        glGetProgramiv(m_Pipeline, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[1024];
            glGetProgramInfoLog(m_Pipeline, 1024, nullptr, infoLog);
        }
#endif
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
#if FORCE_OPENGL_ES != 1
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
#endif
}

void Tilc::Graphics::OpenGL::TPipeline::Destroy()
{
#if FORCE_OPENGL_ES == 1
    // Delete program (monolithic program in GLES3)
    if (m_Pipeline)
    {
        glDeleteProgram(m_Pipeline);
        m_Pipeline = 0;
    }

    // Delete shaders
    if (m_VertexShader)
    {
        glDeleteShader(m_VertexShader);
        m_VertexShader = 0;
    }

    if (m_FragmentShader)
    {
        glDeleteShader(m_FragmentShader);
        m_FragmentShader = 0;
    }

    // Geometry shader is NOT supported in GLES3
    // Compute shader is NOT supported in GLES3

#else
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
#endif
}

GLuint Tilc::Graphics::OpenGL::CompileShader(const GLchar* SourceCode, GLenum Stage, const std::string& CompilationMessage)
{
#if FORCE_OPENGL_ES != 1
    GLuint ShaderProgram = glCreateShaderProgramv(Stage, 1, &SourceCode);
    std::string CompilationLog;
    CompilationLog.resize(2048);
    glGetProgramInfoLog(ShaderProgram, CompilationLog.size(), nullptr, CompilationLog.data());
    if (CompilationLog[0] != 0)
    {
        std::cout << CompilationMessage << ": " << CompilationLog << std::endl;
    }
    return ShaderProgram;

#else
    // 1. Create shader object
    GLuint shader = glCreateShader(Stage);

    // 2. Provide source
    glShaderSource(shader, 1, &SourceCode, nullptr);

    // 3. Compile
    glCompileShader(shader);

    // 4. Check shader compilation log
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    std::string log;
    log.resize(2048);
    glGetShaderInfoLog(shader, log.size(), nullptr, log.data());

    if (log[0] != 0)
    {
        std::cout << CompilationMessage << ": " << log << std::endl;
    }

    if (!success)
    {
        std::cout << "Shader compilation failed!" << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    // 5. Create program
    GLuint program = glCreateProgram();

    // 6. Attach shader
    glAttachShader(program, shader);

    // 7. Link program
    glLinkProgram(program);

    // 8. Check program link log
    GLint linkSuccess = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);

    std::string linkLog;
    linkLog.resize(2048);
    glGetProgramInfoLog(program, linkLog.size(), nullptr, linkLog.data());

    if (linkLog[0] != 0)
    {
        std::cout << CompilationMessage << " (link): " << linkLog << std::endl;
    }

    if (!linkSuccess)
    {
        std::cout << "Program linking failed!" << std::endl;
        glDeleteProgram(program);
        glDeleteShader(shader);
        return 0;
    }

    // 9. Shader can be deleted after linking
    glDeleteShader(shader);

    return program;
#endif
}
