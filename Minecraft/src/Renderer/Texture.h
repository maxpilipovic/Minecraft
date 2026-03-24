#pragma once

#include "mc.h"

class Texture
{
public:
    explicit Texture(const std::string& path);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    void Bind(uint32_t slot = 0) const;
    void Unbind() const;

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    int GetChannels() const { return m_Channels; }

private:
    uint32_t m_RendererID = 0;
    std::string m_Path;
    int m_Width = 0;
    int m_Height = 0;
    int m_Channels = 0;
};
