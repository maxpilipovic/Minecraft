#pragma once

#include "mc.h"

#include <unordered_map>

class Shader
{
public:
    Shader(const std::string& vertexSource, const std::string& fragmentSource);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void Bind() const;
    void Unbind() const;

    void SetUniform1i(const std::string& name, int value);
    void SetUniform1f(const std::string& name, float value);
    void SetUniform2f(const std::string& name, float v0, float v1);
    void SetUniform3f(const std::string& name, float v0, float v1, float v2);
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

private:
    uint32_t CreateShader(const std::string& vertexSource, const std::string& fragmentSource);
    uint32_t CompileShader(uint32_t type, const std::string& source);
    int GetUniformLocation(const std::string& name);

private:
    uint32_t m_RendererID = 0;
    std::unordered_map<std::string, int> m_UniformLocationCache;
};
