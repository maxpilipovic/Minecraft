#include "Application.h"
#include "Logger.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/VertexArray.h"
#include "Renderer/VertexBuffer.h"
#include "mc.h"

#include <glm/gtc/matrix_transform.hpp>

Application& Application::Get()
{
    // Expose the single application instance used by the program.
    static Application instance;
    return instance;
}

Application::Application()
{
    // Initialize logging before any startup code can emit messages.
    Logger::Init();

    // Build the window and rendering state up front.
	Init();
}

Application::~Application()
{
    // Release graphics resources before the process exits.
	Shutdown();
}

void Application::Run()
{
    // Drive the main loop until the app is stopped or the window closes.
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
    // Create the window and initialize the render resources for the cube scene.
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

    const float cubeVertices[] =
    {
        // Front face
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
        // Back face
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        // Left face
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
        // Right face
         0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
        // Top face
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
        // Bottom face
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f
    };

    const uint32_t cubeIndices[] =
    {
        0, 1, 2, 2, 3, 0,       // Front
        4, 5, 6, 6, 7, 4,       // Back
        8, 9, 10, 10, 11, 8,    // Left
        12, 13, 14, 14, 15, 12, // Right
        16, 17, 18, 18, 19, 16, // Top
        20, 21, 22, 22, 23, 20  // Bottom
    };

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

    //Create objects
    m_CubeVAO = std::make_unique<VertexArray>();
    m_CubeVBO = std::make_unique<VertexBuffer>(cubeVertices, static_cast<uint32_t>(sizeof(cubeVertices)));
    m_CubeIBO = std::make_unique<IndexBuffer>(cubeIndices, static_cast<uint32_t>(std::size(cubeIndices)));

    BufferLayout layout;
    layout.Push<float>(3); // Position
    layout.Push<float>(2); // UV
    m_CubeVAO->AddBuffer(*m_CubeVBO, layout);

    m_CubeShader = std::make_unique<Shader>(vertexSource, fragmentSource);
    m_CubeTexture = std::make_unique<Texture>("../images/dirt.png");

    m_CubeShader->Bind();
    m_CubeShader->SetUniform1i("u_Texture", 0);

    m_Running = true;
    m_LastFrameTime = glfwGetTime();
}

void Application::Shutdown()
{
    // Tear down GPU-backed resources in reverse ownership order.
    m_CubeTexture.reset();
    m_CubeShader.reset();
    m_CubeIBO.reset();
    m_CubeVBO.reset();
    m_CubeVAO.reset();
    m_Window.reset();
}

void Application::Update(float dt)
{
    // Advance the cube rotation using frame time for smooth animation.
    m_CubeRotation += dt * 35.0f;
}

void Application::Render()
{
    // Skip rendering until all required scene resources are ready.
    if (!m_CubeVAO || !m_CubeIBO || !m_CubeShader || !m_CubeTexture)
    {
        return;
    }

    Renderer::SetClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    Renderer::Clear();

    const float aspect = static_cast<float>(m_Window->GetWidth()) / static_cast<float>(m_Window->GetHeight());
    glm::mat4 projection = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 100.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(m_CubeRotation), glm::vec3(0.5f, 1.0f, 0.0f));
    glm::mat4 mvp = projection * view * model;

    m_CubeShader->Bind();
    m_CubeShader->SetUniformMat4f("u_MVP", mvp);
    m_CubeTexture->Bind(0);
    Renderer::Draw(*m_CubeVAO, *m_CubeIBO, *m_CubeShader);
}
