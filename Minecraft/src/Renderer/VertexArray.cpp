#include "VertexArray.h"
#include "Logger.h"

#include <cstdint>

uint32_t BufferElement::GetTypeSize() const
{
    switch (Type)
    {
        case GL_FLOAT: return 4;
        case GL_UNSIGNED_INT: return 4;
        case GL_UNSIGNED_BYTE: return 1;
        default:
            MC_CORE_ERROR("Unsupported buffer element type: {}", Type);
            return 0;
    }
}

BufferLayout::BufferLayout(std::initializer_list<BufferElement> elements)
    : m_Elements(elements)
{
    CalculateOffsetsAndStride();
}

void BufferLayout::CalculateOffsetsAndStride()
{
    uint32_t offset = 0;
    m_Stride = 0;

    for (BufferElement& element : m_Elements)
    {
        element.Offset = offset;
        const uint32_t elementSize = element.Count * element.GetTypeSize();
        offset += elementSize;
        m_Stride += elementSize;
    }
}

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray()
{
    if (m_RendererID != 0)
    {
        glDeleteVertexArrays(1, &m_RendererID);
    }
}

VertexArray::VertexArray(VertexArray&& other) noexcept
    : m_RendererID(other.m_RendererID)
{
    other.m_RendererID = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    if (m_RendererID != 0)
    {
        glDeleteVertexArrays(1, &m_RendererID);
    }

    m_RendererID = other.m_RendererID;
    other.m_RendererID = 0;
    return *this;
}

void VertexArray::AddBuffer(const VertexBuffer& vertexBuffer, const BufferLayout& layout)
{
    Bind();
    vertexBuffer.Bind();

    const std::vector<BufferElement>& elements = layout.GetElements();
    for (uint32_t i = 0; i < static_cast<uint32_t>(elements.size()); ++i)
    {
        const BufferElement& element = elements[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(
            i,
            element.Count,
            element.Type,
            element.Normalized ? GL_TRUE : GL_FALSE,
            static_cast<GLsizei>(layout.GetStride()),
            reinterpret_cast<const void*>(static_cast<uintptr_t>(element.Offset))
        );
    }
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}
