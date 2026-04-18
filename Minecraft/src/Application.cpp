#include "Application.h"
#include "Logger.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/VertexArray.h"
#include "Renderer/VertexBuffer.h"
#include "Camera/Camera.h"
#include "ChunkMesher.h"
#include "mc.h"
#include <glm/gtc/matrix_transform.hpp>

Application& Application::Get()
{
    //Start the single application instance used by the program.
    static Application instance;
    return instance;
}

Application::Application()
{
    //Initialize
    Logger::Init();

	Init();
}

Application::~Application()
{
	Shutdown();
}

void Application::Run()
{
    //Main loop
    while (m_Running && m_Window && !m_Window->ShouldClose())
    {
        double currentTime = glfwGetTime();
        float dt = static_cast<float>(currentTime - m_LastFrameTime);
        m_LastFrameTime = currentTime;

        Update(dt);
        Render();

        m_Window->SwapBuffers();
        m_Window->PollEvents();
    }

}

void Application::Init()
{
    m_Window = std::make_unique<Window>(1280, 720, "Minecraft");

    //Check if window exists
    if (!m_Window->GetNative())
    {
        m_Running = false;
        return;
    }

    //Render screen
    glViewport(0, 0, m_Window->GetWidth(), m_Window->GetHeight());

    //Enable depth test. -> Which object is in front?
    glEnable(GL_DEPTH_TEST);


    //1 Layer Grass
    //3 Layer Dirt
    //Remainder Stone
    //Build the blocks!!!
    for (int z{}; z < Chunk::Depth; z++)
    {
        for (int x{}; x < Chunk::Width; x++)
        {
            for (int y{}; y < Chunk::Height; y++)
            {
                m_Chunk.SetBlock(x, y, z, BlockType::Grass);
                if (y == m_Chunk.Height - 1)
                {
                    //Render Grass
                    m_Chunk.SetBlock(x, y, z, BlockType::Grass);
                }
                //If height 16
                //12
                else if ((y >= m_Chunk.Height - 4))
                {
                    m_Chunk.SetBlock(x, y, z, BlockType::Dirt);
                }
                else
                {
                    m_Chunk.SetBlock(x, y, z, BlockType::Stone);
                }
            }
        }
    }

    //Vertex shader
    const std::string vertexSource = R"(
        #version 330 core
        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec2 a_TexCoord;

        uniform mat4 u_MVP;

        out vec2 v_TexCoord;

        void main()
        {
            v_TexCoord = a_TexCoord;
            gl_Position = u_MVP * vec4(a_Position, 1.0);
        }
    )";

    //Fragment shader
    const std::string fragmentSource = R"(
        #version 330 core
        in vec2 v_TexCoord;

        uniform sampler2D u_Texture;

        out vec4 FragColor;

        void main()
        {
            FragColor = texture(u_Texture, v_TexCoord);
        }
    )";

    //Create Camera
    m_Camera = std::make_unique<Camera>(glm::vec3(8.0f, 4.0f, 20.0f));

    BuildChunkMesher(m_Chunk);

    m_CubeShader = std::make_unique<Shader>(vertexSource, fragmentSource);

    //Textures
    m_CubeTexture = std::make_unique<Texture>("../images/dirt2.png");
    m_CubeTexture2 = std::make_unique<Texture>("../images/grass3.png");
    m_CubeTexture3 = std::make_unique<Texture>("../images/stone3.png");

    
    m_CubeShader->Bind();
    m_CubeShader->SetUniform1i("u_Texture", 0);

    m_Running = true;
    m_LastFrameTime = glfwGetTime();
}

void Application::UpdateCameraKeyboard(float deltaTime)
{
    float moveSpeed = 5.0f * deltaTime;
    float turnSpeed = 50.0f * deltaTime;

    if (m_Window->IsKeyPressed(GLFW_KEY_W))
    {
        m_Camera->MoveForward(moveSpeed);
    }

    if (m_Window->IsKeyPressed(GLFW_KEY_S))
    {
        m_Camera->MoveForward(-moveSpeed);
    }

    if (m_Window->IsKeyPressed(GLFW_KEY_D))
    {
        m_Camera->MoveRight(moveSpeed);
    }

    if (m_Window->IsKeyPressed(GLFW_KEY_A))
    {
        m_Camera->MoveRight(-moveSpeed);
    }

    if (m_Window->IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
    {
        m_Camera->MovePosition(glm::vec3(0.0f, -moveSpeed, 0.0f));
    }

    if (m_Window->IsKeyPressed(GLFW_KEY_SPACE))
    {
        m_Camera->MovePosition(glm::vec3(0.0f, moveSpeed, 0.0f));
    }
}

void Application::UpdateCameraMouse(float deltaX, float deltaY)
{
    float sensitivity = 0.1f;

    m_Camera->AddYaw(deltaX * sensitivity);
    m_Camera->AddPitch(deltaY * sensitivity);
}

void Application::Shutdown()
{
    //Tear down GPU resources in reverse ownership order.
    //Grass
    m_GrassMesh.IBO.reset();
    m_GrassMesh.VAO.reset();
    m_GrassMesh.VBO.reset();
    //Dirt
    m_DirtMesh.IBO.reset();
    m_DirtMesh.VAO.reset();
    m_DirtMesh.VBO.reset();

    //Stone
    m_StoneMesh.IBO.reset();
    m_StoneMesh.VAO.reset();
    m_StoneMesh.VBO.reset();

    //Rest everything
    m_CubeTexture.reset();
    m_CubeTexture2.reset();
    m_CubeTexture3.reset();

    m_CubeShader.reset();
    m_Camera.reset();
    m_Window.reset();

}

void Application::Update(float dt)
{

    UpdateCameraKeyboard(dt);

    double xpos, ypos;

    m_Window->GetMousePosition(xpos, ypos);

    if (m_FirstMouse)
    {
        m_LastX = xpos;
        m_LastY = ypos;
        m_FirstMouse = false;
    }

    float deltaX = xpos - m_LastX;
    float deltaY = m_LastY - ypos; //Reversed

    m_LastX = xpos;
    m_LastY = ypos;

    UpdateCameraMouse(deltaX, deltaY);
}

void Application::Render()
{
    Renderer::SetClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    Renderer::Clear();

    //Skip drawing chunk geometry until all required scene resources are ready.
    if (!m_GrassMesh.VAO || !m_GrassMesh.IBO || !m_DirtMesh.VAO || !m_DirtMesh.IBO || !m_StoneMesh.VAO || !m_StoneMesh.IBO || !m_CubeShader || !m_CubeTexture || !m_Camera)
    {
        return;
    }

    const float aspect = static_cast<float>(m_Window->GetWidth()) / static_cast<float>(m_Window->GetHeight());
    glm::mat4 projection = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 100.0f);
    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = projection * view * model;

    m_CubeShader->Bind();
    m_CubeShader->SetUniformMat4f("u_MVP", mvp);

    //Draw call. 3 SEPERATE!!!
    if (CheckValid(m_GrassMesh))
    {
        m_CubeTexture2->Bind(0);
        Renderer::Draw(*m_GrassMesh.VAO, *m_GrassMesh.IBO, *m_CubeShader);
    }

    if (CheckValid(m_DirtMesh))
    {
        m_CubeTexture->Bind(0);
        Renderer::Draw(*m_DirtMesh.VAO, *m_DirtMesh.IBO, *m_CubeShader);
    }

    if (CheckValid(m_StoneMesh))
    {
        m_CubeTexture3->Bind(0);
        Renderer::Draw(*m_StoneMesh.VAO, *m_StoneMesh.IBO, *m_CubeShader);
    }

    //Move Camera
    //m_Camera->AddYaw(1.0f);
    //->AddPitch(0.1f);
    //m_Camera->MoveForward(0.01f);
    //m_Camera->MoveRight(0.1f);
}

void Application::BuildChunkMesher(const Chunk& chunk)
{
    ChunkMeshes mesh = ::BuildChunkMesh(chunk);

    //Dirtmesh
    //Stonemesh
    //Grassmesh
    UploadMesh(mesh.Dirt, m_DirtMesh);
    UploadMesh(mesh.Grass, m_GrassMesh);
    UploadMesh(mesh.Stone, m_StoneMesh);

}

void Application::UploadMesh(const ChunkMesh& mesh, RenderMesh& gpu)
{

    //Check bnefore render.
    if (mesh.Vertices.empty() || mesh.Indices.empty())
    {
        gpu.VAO.reset();
        gpu.VBO.reset();
        gpu.IBO.reset();
        return;
    }

    gpu.VAO = std::make_unique<VertexArray>();
    gpu.VBO = std::make_unique<VertexBuffer>(
        mesh.Vertices.data(),
        static_cast<uint32_t>(mesh.Vertices.size() * sizeof(ChunkVertex))
    );
    gpu.IBO = std::make_unique<IndexBuffer>(
        mesh.Indices.data(),
        static_cast<uint32_t>(mesh.Indices.size())
    );

    BufferLayout layout;
    layout.Push<float>(3); // Position
    layout.Push<float>(2); // UV
    gpu.VAO->AddBuffer(*gpu.VBO, layout);

}

bool Application::CheckValid(const RenderMesh& gpu)
{
    if (gpu.VAO && gpu.VBO && gpu.IBO)
    {
        return true;
    }

    return false;
}
