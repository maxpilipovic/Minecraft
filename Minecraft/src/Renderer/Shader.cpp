#include "Shader.h"
#include "Logger.h"

#include <glm/gtc/type_ptr.hpp>
#include <vector>

Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource)
{
    m_RendererID = CreateShader(vertexSource, fragmentSource);
}

Shader::~Shader()
{
    if (m_RendererID != 0)
    {
        glDeleteProgram(m_RendererID);
    }
}

Shader::Shader(Shader&& other) noexcept
    : m_RendererID(other.m_RendererID),
      m_UniformLocationCache(std::move(other.m_UniformLocationCache))
{
    other.m_RendererID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    if (m_RendererID != 0)
    {
        glDeleteProgram(m_RendererID);
    }

    m_RendererID = other.m_RendererID;
    m_UniformLocationCache = std::move(other.m_UniformLocationCache);
    other.m_RendererID = 0;
    return *this;
}

void Shader::Bind() const
{
    glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
    glUniform2f(GetUniformLocation(name), v0, v1);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
    glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

uint32_t Shader::CreateShader(const std::string& vertexSource, const std::string& fragmentSource)
{
    const uint32_t program = glCreateProgram();
    const uint32_t vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    const uint32_t fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glValidateProgram(program);

    int linkStatus = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        int logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> message(static_cast<size_t>(logLength));
        glGetProgramInfoLog(program, logLength, &logLength, message.data());
        MC_CORE_ERROR("Shader program link failed: {}", message.data());

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(program);
        return 0;
    }

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

uint32_t Shader::CompileShader(uint32_t type, const std::string& source)
{
    const uint32_t shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int compileStatus = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE)
    {
        int logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> message(static_cast<size_t>(logLength));
        glGetShaderInfoLog(shader, logLength, &logLength, message.data());

        const char* shaderType = type == GL_VERTEX_SHADER ? "vertex" : "fragment";
        MC_CORE_ERROR("{} shader compilation failed: {}", shaderType, message.data());
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

int Shader::GetUniformLocation(const std::string& name)
{
    const auto it = m_UniformLocationCache.find(name);
    if (it != m_UniformLocationCache.end())
    {
        return it->second;
    }

    const int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
    {
        MC_CORE_ERROR("Uniform '{}' not found in shader", name);
    }

    m_UniformLocationCache[name] = location;
    return location;
}
