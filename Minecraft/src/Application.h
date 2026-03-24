#pragma once

#include <memory>

#include "Window.h"

//Forward declarations?
class VertexArray;
class VertexBuffer;
class IndexBuffer;
class Shader;
class Texture;

class Application
{
public:
    static Application& Get();

    void Run();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

private:
    Application();
    ~Application();

    void Init();
    void Shutdown();
    void Update(float dt);
    void Render();

private:
    //Renderer resource ownership
    std::unique_ptr<Window> m_Window;
    std::unique_ptr<VertexArray> m_CubeVAO;
    std::unique_ptr<VertexBuffer> m_CubeVBO;
    std::unique_ptr<IndexBuffer> m_CubeIBO;
    std::unique_ptr<Shader> m_CubeShader;
    std::unique_ptr<Texture> m_CubeTexture;

    float m_CubeRotation = 0.0f;
    bool m_Running = false;
    double m_LastFrameTime = 0.0;
};