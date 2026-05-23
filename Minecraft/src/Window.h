#pragma once

#include "mc.h"

class Window
{
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    void PollEvents();
    void SwapBuffers() const;

    bool ShouldClose() const;
    void SetShouldClose(bool value);

    void SetVSync(bool enabled);

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

    //Key stuff
    bool IsKeyPressed(int key) const;
    void GetMousePosition(double& x, double& y) const;

    //Mouse stuff
    bool IsMouseButtonPressed(int button) const;
    bool IsMouseButtonClicked(int button) const;

    GLFWwindow* GetNative() const { return m_Window; }

    bool getPreviousRightMouse() const;
    bool getCurrentRightMouse() const;
    bool getPreviousLeftMouse() const;
    bool getCurrentLeftMouse() const;

    void setPreviousRightMouse(bool pressed);
    void setCurrentRightMouse(bool pressed);
    void setPreviousLeftMouse(bool pressed);
    void setCurrentLeftMouse(bool pressed);

private:
    void Init();
    void Shutdown();

private:
    GLFWwindow* m_Window = nullptr;
    int m_Width;
    int m_Height;
    std::string m_Title;
    
    //Mouse Position
    double x;
    double y;

    //Mouse Clicks
    bool m_PreviousRightMouse = false;
    bool m_CurrentRightMouse = false;
    bool m_PreviousLeftMouse = false;
    bool m_CurrentLeftMouse = false;

    bool m_VSync = true;
};
