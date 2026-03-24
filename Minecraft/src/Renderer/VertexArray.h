#pragma once

#include "mc.h"
#include "VertexBuffer.h"

#include <vector>

struct BufferElement
{
    uint32_t Count = 0;
    uint32_t Type = 0;
    bool Normalized = false;
    uint32_t Offset = 0;

    uint32_t GetTypeSize() const;
};

class BufferLayout
{
public:
    BufferLayout() = default;
    BufferLayout(std::initializer_list<BufferElement> elements);

    template<typename T>
    void Push(uint32_t count)
    {
        static_assert(sizeof(T) == 0, "Unsupported buffer element type");
    }

    const std::vector<BufferElement>& GetElements() const { return m_Elements; }
    uint32_t GetStride() const { return m_Stride; }

private:
    void CalculateOffsetsAndStride();

private:
    std::vector<BufferElement> m_Elements;
    uint32_t m_Stride = 0;
};

template<>
inline void BufferLayout::Push<float>(uint32_t count)
{
    m_Elements.push_back({ count, GL_FLOAT, false, 0 });
    CalculateOffsetsAndStride();
}

template<>
inline void BufferLayout::Push<uint32_t>(uint32_t count)
{
    m_Elements.push_back({ count, GL_UNSIGNED_INT, false, 0 });
    CalculateOffsetsAndStride();
}

template<>
inline void BufferLayout::Push<unsigned char>(uint32_t count)
{
    m_Elements.push_back({ count, GL_UNSIGNED_BYTE, true, 0 });
    CalculateOffsetsAndStride();
}

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    VertexArray(VertexArray&& other) noexcept;
    VertexArray& operator=(VertexArray&& other) noexcept;

    void AddBuffer(const VertexBuffer& vertexBuffer, const BufferLayout& layout);

    void Bind() const;
    void Unbind() const;

private:
    uint32_t m_RendererID = 0;
};
