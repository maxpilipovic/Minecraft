#pragma once

#include <memory>

#include "Window.h"

//Forward declarations?
class VertexArray;
class VertexBuffer;
class IndexBuffer;
class Shader;
class Texture;
class Camera;

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

    //Update Camera
    void UpdateCameraKeyboard(float deltaTime);
    void UpdateCameraMouse(float deltaX, float deltaY);

private:
    //Renderer resource ownership
    std::unique_ptr<Window> m_Window;
    std::unique_ptr<VertexArray> m_CubeVAO;
    std::unique_ptr<VertexBuffer> m_CubeVBO;
    std::unique_ptr<IndexBuffer> m_CubeIBO;
    std::unique_ptr<Shader> m_CubeShader;
    std::unique_ptr<Texture> m_CubeTexture;
    std::unique_ptr<Camera> m_Camera;

    //Mouse stuff
    float m_LastX = 0.0f;
    float m_LastY = 0.0f;
    bool m_FirstMouse = true;

    float m_CubeRotation = 0.0f;
    bool m_Running = false;
    double m_LastFrameTime = 0.0;
};