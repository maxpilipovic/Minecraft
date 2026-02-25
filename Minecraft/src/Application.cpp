#include "Application.h"
#include "Logger.h"
#include "mc.h"

Application& Application::Get()
{
    static Application instance;
    return instance;
}

Application::Application()
{
    //Logger Init
    Logger::Init();

    //Application Init
	Init();
}

Application::~Application()
{
	Shutdown();
}

void Application::Run()
{
    while (m_Running && m_Window && !m_Window->ShouldClose())
    {
        double currentTime = glfwGetTime();
        float dt = static_cast<float>(currentTime - m_LastFrameTime);
        m_LastFrameTime = currentTime;

        Update(dt);
        Render();

        m_Window->SwapBuffers();
        m_Window->PollEvents();
    }

}

void Application::Init()
{
    m_Window = std::make_unique<Window>(1280, 720, "Minecraft");
    m_Running = true;
    m_LastFrameTime = glfwGetTime();
}

void Application::Shutdown()
{
}

void Application::Update(float dt)
{
}

void Application::Render()
{
}