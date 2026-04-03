#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const void* data, uint32_t size)
{
    //Create buffer
    glGenBuffers(1, &m_RendererID);

    //Activate Buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);

    //Send buffer to GPU.
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    if (m_RendererID != 0)
    {
        glDeleteBuffers(1, &m_RendererID);
    }
}

//Move constructor
VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
    : m_RendererID(other.m_RendererID)
{
    other.m_RendererID = 0;
}

//Move assignment operator
VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
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
    other.m_RendererID = 0;
    return *this;
}

void VertexBuffer::SetData(const void* data, uint32_t size) const
{
    //Bind buffer (use it)
    Bind();

    //Updates buffer data
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::Unbind() const
{
    //Unbind buffer (dont use it)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
