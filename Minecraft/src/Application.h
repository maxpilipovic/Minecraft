#pragma once

#include <memory>

#include "Window.h"

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
    std::unique_ptr<Window> m_Window;
    bool m_Running = false;
    double m_LastFrameTime = 0.0;
};
