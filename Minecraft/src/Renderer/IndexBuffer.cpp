#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(const uint32_t* indices, uint32_t count)
    : m_Count(count)
{
    //Assign a RendererID
    glGenBuffers(1, &m_RendererID);
    
    //Bind
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);

    //Send to GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
    if (m_RendererID != 0)
    {
        glDeleteBuffers(1, &m_RendererID);
    }
}

IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept
    : m_RendererID(other.m_RendererID), m_Count(other.m_Count)
{
    other.m_RendererID = 0;
    other.m_Count = 0;
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    if (m_RendererID != 0)
    {
        glDeleteBuffers(1, &m_RendererID);
    }

    m_RendererID = other.m_RendererID;
    m_Count = other.m_Count;
    other.m_RendererID = 0;
    other.m_Count = 0;
    return *this;
}

void IndexBuffer::Bind() const
{
    //Bind
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void IndexBuffer::Unbind() const
{
    //Unbind
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
