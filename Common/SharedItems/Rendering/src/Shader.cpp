#include "Rendering/include/Shader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <Rendering/include/Renderer.h>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};


Shader::Shader(const std::string& filepath)
    : m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
    GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLoactionCache.find(name) != m_UniformLoactionCache.end())
        return m_UniformLoactionCache[name];

    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1) // invalid
        std::cout << "Warning: uniform " << name << " doesn't exist" << std::endl;

    m_UniformLoactionCache[name] = location;

    return location;
}

bool Shader::CompileShader()
{
    return false;
}


unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram(); // Create shader program
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader); // Compile vertex shader
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader); // Compile fragment shader

    GLCall(glAttachShader(program, vs)); // Attach vertex shader
    GLCall(glAttachShader(program, fs)); // Attach fragment shader
    GLCall(glLinkProgram(program));      // Link shaders into program
    GLCall(glValidateProgram(program));  // Validate program
    GLCall(glDeleteShader(vs)); // Free vertex shader after linking
    GLCall(glDeleteShader(fs)); // Free fragment shader after linking

    return program; // Return program ID
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type); // Create shader object (vertex or fragment)
    const char* src = source.c_str();       // Convert C++ string to C-style string
    GLCall(glShaderSource(id, 1, &src, nullptr));   // Attach source code to shader
    GLCall(glCompileShader(id));                    // Compile the shader

    // Error handling
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result)); // Check compile status
    if (!result)
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length)); // Get error log length
        char* message = (char*)alloca(length * sizeof(char)); // Allocate buffer on stack
        GLCall(glGetShaderInfoLog(id, length, &length, message)); // Retrieve error log
        std::cout << "Failed to compile shader " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
        return 0;
    }

    return id; // Return compiled shader ID
}


ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);
    if (!stream)
        std::cout << "file not loaded\n";

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2]; // One for vertex One for fragment
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
            ss[(int)type] << line << '\n';
    }

    return ShaderProgramSource{ ss[0].str(), ss[1].str() };
}

