#include "Texture.h"

#include "Logger.h"

Texture::Texture(const std::string& path)
    : m_Path(path)
{
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);
    if (!data)
    {
        MC_CORE_ERROR("Failed to load texture '{}'", path);
        return;
    }

    GLenum internalFormat = 0;
    GLenum dataFormat = 0;
    if (m_Channels == 4)
    {
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
    }
    else if (m_Channels == 3)
    {
        internalFormat = GL_RGB8;
        dataFormat = GL_RGB;
    }
    else
    {
        MC_CORE_ERROR("Unsupported texture format (channels: {}) for '{}'", m_Channels, path);
        stbi_image_free(data);
        return;
    }

    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

Texture::~Texture()
{
    if (m_RendererID != 0)
    {
        glDeleteTextures(1, &m_RendererID);
    }
}

Texture::Texture(Texture&& other) noexcept
    : m_RendererID(other.m_RendererID),
      m_Path(std::move(other.m_Path)),
      m_Width(other.m_Width),
      m_Height(other.m_Height),
      m_Channels(other.m_Channels)
{
    other.m_RendererID = 0;
    other.m_Width = 0;
    other.m_Height = 0;
    other.m_Channels = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    if (m_RendererID != 0)
    {
        glDeleteTextures(1, &m_RendererID);
    }

    m_RendererID = other.m_RendererID;
    m_Path = std::move(other.m_Path);
    m_Width = other.m_Width;
    m_Height = other.m_Height;
    m_Channels = other.m_Channels;

    other.m_RendererID = 0;
    other.m_Width = 0;
    other.m_Height = 0;
    other.m_Channels = 0;
    return *this;
}

void Texture::Bind(uint32_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
