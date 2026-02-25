#pragma once

#include "mc.h"

class Window
{
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    void PollEvents() const;
    void SwapBuffers() const;

    bool ShouldClose() const;
    void SetShouldClose(bool value);

    void SetVSync(bool enabled);

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

    GLFWwindow* GetNative() const { return m_Window; }

private:
    void Init();
    void Shutdown();

private:
    GLFWwindow* m_Window = nullptr;
    int m_Width;
    int m_Height;
    std::string m_Title;

    bool m_VSync = true;
};
