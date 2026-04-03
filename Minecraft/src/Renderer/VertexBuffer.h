#pragma once

#include "mc.h"

//Raw data.  Vertex Buffer is just raw memory on the GPU.
class VertexBuffer
{
public:
    VertexBuffer(const void* data, uint32_t size);
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    VertexBuffer(VertexBuffer&& other) noexcept;
    VertexBuffer& operator=(VertexBuffer&& other) noexcept;

    void SetData(const void* data, uint32_t size) const;

    void Bind() const;
    void Unbind() const;

private:
    uint32_t m_RendererID = 0;
};
