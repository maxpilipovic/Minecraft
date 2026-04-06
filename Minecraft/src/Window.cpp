#include "Window.h"
#include "Logger.h"

Window::Window(int width, int height, const std::string& title)
    : m_Width(width),
      m_Height(height),
      m_Title(title)
{
    Init();
}

Window::~Window()
{
    Shutdown();
}

void Window::PollEvents() const
{
    glfwPollEvents();
}

void Window::SwapBuffers() const
{
    if (m_Window)
    {
        glfwSwapBuffers(m_Window);
    }
}

bool Window::ShouldClose() const
{
    return m_Window && glfwWindowShouldClose(m_Window);
}

void Window::SetShouldClose(bool value)
{
    if (m_Window)
    {
        glfwSetWindowShouldClose(m_Window, value ? GLFW_TRUE : GLFW_FALSE);
    }
}

void Window::SetVSync(bool enabled)
{
    m_VSync = enabled;
    glfwSwapInterval(m_VSync ? 1 : 0);
}

void Window::Init()
{
    if (!glfwInit())
    {
        MC_CORE_ERROR("Failed to initialize GLFW");
        return;
    }

    m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
    if (!m_Window)
    {
        MC_CORE_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_Window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        MC_CORE_ERROR("Failed to initialize GLAD");
        return;
    }

    //Set input mode. Hides cursor, locks to window, infinite mouse movement
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    SetVSync(true);
}

void Window::Shutdown()
{
    if (m_Window)
    {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }

    glfwTerminate();
}

bool Window::IsKeyPressed(int key) const
{
    return glfwGetKey(m_Window, key) == GLFW_PRESS;
}

//Get Mouse Pos
void Window::GetMousePosition(double& x, double& y) const
{
    glfwGetCursorPos(m_Window, &x, &y);
}
