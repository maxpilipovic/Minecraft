#include "Texture.h"
#include <filesystem>
#include "Logger.h"

Texture::Texture(const std::string& path)
    : m_Path(path)
{
    //Print working directory...
    MC_CORE_INFO("Working directory: {}", std::filesystem::current_path().string());

    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);
    if (!data)
    {
        const char* reason = stbi_failure_reason();
        MC_CORE_ERROR("Failed to load texture '{}'. Reason: {}", path, reason ? reason : "Unknown");
        return;
    }

    //Successfully loaded TEXTURE!!
    MC_CORE_INFO("Successfully LOADED TEXTURE!");

    GLenum internalFormat = 0;
    GLenum dataFormat = 0;

    //RGBA 4 bytes a pixel
    if (m_Channels == 4)
    {
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
    }

    //RGB - 3 bytes a pixel
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

    //Change according to different texture rules
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //Upload to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

    //Creates smaller versions of texture (full, half, quater)
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
    //Bind texture to slot for shader
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::Unbind() const
{
    //Unbind
    glBindTexture(GL_TEXTURE_2D, 0);
}
