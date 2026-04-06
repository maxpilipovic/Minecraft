#include "Renderer.h"

void Renderer::SetClearColor(float r, float g, float b, float a)
{
    //Set color
    glClearColor(r, g, b, a);
}

void Renderer::Clear()
{
    //Clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader)
{
    //Draw and bind everything
    shader.Bind();
    vertexArray.Bind();
    indexBuffer.Bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexBuffer.GetCount()), GL_UNSIGNED_INT, nullptr);
}
