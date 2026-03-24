#pragma once

#include "mc.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"

class Renderer
{
public:
    static void SetClearColor(float r, float g, float b, float a);
    static void Clear();
    static void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader);
};
