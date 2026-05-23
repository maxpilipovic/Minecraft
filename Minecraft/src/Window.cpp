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

void Window::PollEvents()
{
    setPreviousRightMouse(getCurrentRightMouse());
    setPreviousLeftMouse(getCurrentLeftMouse());
    glfwPollEvents();
    setCurrentRightMouse(glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
    setCurrentLeftMouse(glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
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

bool Window::IsMouseButtonPressed(int button) const
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        return getCurrentRightMouse();
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        return getCurrentLeftMouse();
    }

    return glfwGetMouseButton(m_Window, button) == GLFW_PRESS;
}

bool Window::IsMouseButtonClicked(int button) const
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        return getCurrentRightMouse() && !getPreviousRightMouse();
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        return getCurrentLeftMouse() && !getPreviousLeftMouse();
    }

    return false;
}

bool Window::getPreviousRightMouse() const
{
    return m_PreviousRightMouse;
}

bool Window::getCurrentRightMouse() const
{
    return m_CurrentRightMouse;
}

void Window::setPreviousRightMouse(bool pressed)
{
    m_PreviousRightMouse = pressed;
}

void Window::setCurrentRightMouse(bool pressed)
{
    m_CurrentRightMouse = pressed;
}

bool Window::getPreviousLeftMouse() const
{
    return m_PreviousLeftMouse;
}

bool Window::getCurrentLeftMouse() const
{
    return m_CurrentLeftMouse;
}

void Window::setPreviousLeftMouse(bool pressed)
{
    m_PreviousLeftMouse = pressed;
}

void Window::setCurrentLeftMouse(bool pressed)
{
    m_CurrentLeftMouse = pressed;
}

//Get Mouse Pos
void Window::GetMousePosition(double& x, double& y) const
{
    glfwGetCursorPos(m_Window, &x, &y);
}
