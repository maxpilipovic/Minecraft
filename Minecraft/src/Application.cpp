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
#include "World.h"
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
    m_Gui.Init(*m_Window);
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

        //Update gui
        m_Gui.Update(m_DebugMenuOpen);

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

    //Make World
    m_World = std::make_unique<World>();

    //Make thread worker
    m_ChunkWorker = std::thread([this]()
        {
            while (!m_StopChunkWorker)
            {
                ChunkPos pos{};

                {
                    std::unique_lock lock(m_ChunkMutex);
                    m_ChunkCondition.wait(lock, [this]()
                        {
                            return m_StopChunkWorker || !m_ChunkJob.empty();
                        });

                    if (m_StopChunkWorker)
                        break;

                    pos = m_ChunkJob.front();
                    m_ChunkJob.pop();
                }

                Chunk chunk = m_World->CreateChunk(pos);

                {
                    std::lock_guard lock(m_ChunkMutex);
                    m_FinishedChunk.push({ pos, std::move(chunk) });
                }
            }
        });

    //Create Camera
    m_Camera = std::make_unique<Camera>(glm::vec3(8.0f, 20.0f, 20.0f));

    //Set camera pos.
    m_Camera->SetPosition(m_Player.position + glm::vec3(0.0f, m_Player.eyeHeight, 0.0f));

    ChunkPos cameraChunk = World::fromWorldPosition(m_Camera->GetPosition());
    
    m_CubeShader = std::make_unique<Shader>(vertexSource, fragmentSource);

    //Preload first chunk or FALL through world
    ChunkPos spawnChunk = World::fromWorldPosition(m_Player.position);
    Chunk spawn = m_World->CreateChunk(spawnChunk);

    m_World->AddChunk(spawnChunk, std::move(spawn));

    const Chunk* chunk = m_World->GetChunk(spawnChunk);

    if (chunk)
    {
        BuildChunkMesher(spawnChunk, *chunk, *m_World);
        m_MeshedChunks.insert(spawnChunk);
    }

    //Generate remainder chunks
    GenerateChunksAroundCamera(cameraChunk);

    //Textures
    m_CubeTexture = std::make_unique<Texture>("../images/dirt2.png");
    m_CubeTexture2 = std::make_unique<Texture>("../images/sand.png");
    m_CubeTexture3 = std::make_unique<Texture>("../images/stone3.png");
    m_CubeTexture4 = std::make_unique<Texture>("../images/grass4.png");
    m_CubeTexture5 = std::make_unique<Texture>("../images/oak_log.png");
    m_CubeTexture6 = std::make_unique<Texture>("../images/oak_leaves.png");
    
    m_CubeShader->Bind();
    m_CubeShader->SetUniform1i("u_Texture", 0);

    m_Running = true;
    m_LastFrameTime = glfwGetTime();
}

void Application::UpdatePlayer(float deltaTime)
{
    float moveSpeed = 5.0f;
    float gravity = 25.0f;
    float jumpSpeed = 8.0f;
    float eyeHeight = m_Player.eyeHeight;

    //Grab cameras forward direction. Does not allow move up
    glm::vec3 forward = m_Camera->getFront();
    forward.y = 0.0f;

    //Forward length 1, movement is consistent.
    if (glm::length(forward) > 0.0f)
    {
        forward = glm::normalize(forward);
    }

    //Sidways direction
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

    //Wishdir
    glm::vec3 wishDir({ 0.0f });

    if (m_Window->IsKeyPressed(GLFW_KEY_W))
    {
        wishDir += forward;
    }

    if (m_Window->IsKeyPressed(GLFW_KEY_S))
    {
        wishDir -= forward;
    }

    if (m_Window->IsKeyPressed(GLFW_KEY_D))
    {
        wishDir += right;
    }

    if (m_Window->IsKeyPressed(GLFW_KEY_A))
    {
        wishDir -= right;
    }

    if (m_Window->IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
    {
        //Drop eye height
        eyeHeight = 1.2f;

        //Drop move speed
        moveSpeed = 2.5f;
    }
    

    if (glm::length(wishDir) > 0.0f)
    {
        wishDir = glm::normalize(wishDir);
    }

    m_Player.velocity.x = wishDir.x * moveSpeed;
    m_Player.velocity.z = wishDir.z * moveSpeed;

    m_Player.velocity.y -= gravity * deltaTime;

    if (m_Player.onGround && m_Window->IsKeyPressed(GLFW_KEY_SPACE))
    {
        m_Player.velocity.y = jumpSpeed;
        m_Player.onGround = false;
    }

    m_Player.onGround = false;

    MovePlayerAxis(glm::vec3(m_Player.velocity.x * deltaTime, 0.0f, 0.0f));
    MovePlayerAxis(glm::vec3(0.0f, m_Player.velocity.y * deltaTime, 0.0f));
    MovePlayerAxis(glm::vec3(0.0f, 0.0f, m_Player.velocity.z * deltaTime));

    m_Camera->SetPosition(m_Player.position + glm::vec3(0.0f, eyeHeight, 0.0f));

    //Bring back from shift
    eyeHeight = 1.6f;
}

bool Application::IsPlayerColliding(glm::vec3 position)
{
    float halfWidth = m_Player.width * 0.5f;

    int minX = static_cast<int>(std::floor(position.x - halfWidth));
    int maxX = static_cast<int>(std::floor(position.x + halfWidth));

    int minY = static_cast<int>(std::floor(position.y));
    int maxY = static_cast<int>(std::floor(position.y + m_Player.height));

    int minZ = static_cast<int>(std::floor(position.z - halfWidth));
    int maxZ = static_cast<int>(std::floor(position.z + halfWidth));

    for (int z = minZ; z <= maxZ; z++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            for (int x = minX; x <= maxX; x++)
            {
                if (m_World->GetBlockWorld(x, y, z) != BlockType::Air)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void Application::MovePlayerAxis(const glm::vec3& movement)
{
    glm::vec3 nextPosition = m_Player.position + movement;

    if (!IsPlayerColliding(nextPosition))
    {
        m_Player.position = nextPosition;
        return;
    }

    if (movement.y < 0.0f)
    {
        m_Player.onGround = true;
        m_Player.velocity.y = 0.0f;
    }
    else if (movement.y > 0.0f)
    {
        m_Player.velocity.y = 0.0f;
    }
}

void Application::UpdateBlockInteraction()
{
    //BREAK BLOCK!!!
    if (m_Window->IsMouseButtonClicked(GLFW_MOUSE_BUTTON_LEFT))
    {
        //Break block
        glm::vec3 currentChunk = m_Camera->GetPosition();

        //3 block distance
        auto hit = RaycastBlock(*m_World, currentChunk, getFront(), 3.0f);

        if (hit)
        {
            if (m_World->SetBlockWorld(hit->blockPos.x, hit->blockPos.y, hit->blockPos.z, BlockType::Air))
            {
                //Remesh the edited chunk
                ChunkPos rayChunk = World::fromWorldPosition(glm::vec3(hit->blockPos.x, hit->blockPos.y, hit->blockPos.z));

                //REMESH!!!
                RebuildChunkMesher(rayChunk);

                //Log
                MC_CORE_INFO("LEFT CLICK PRESSED");
            }
        }
    }

    //PLACE BLOCK!!!
    if (m_Window->IsMouseButtonClicked(GLFW_MOUSE_BUTTON_RIGHT))
    {
        glm::vec3 currentChunk = m_Camera->GetPosition();

        //3 block distance
        auto hit = RaycastBlock(*m_World, currentChunk, getFront(), 3.0f);

        if (hit)
        {
            if (m_World->SetBlockWorld(hit->previousPos.x, hit->previousPos.y, hit->previousPos.z, BlockType::Stone))
            {
                //Remesh the edited chunk
                ChunkPos rayChunk = World::fromWorldPosition(glm::vec3(hit->previousPos.x, hit->previousPos.y, hit->previousPos.z));

                //REMESH!!!
                RebuildChunkMesher(rayChunk);

                //Log
                MC_CORE_INFO("RIGHT CLICK PRESSED");

            }
        }
    }
}

std::optional<BlockHit> Application::RaycastBlock(const World& world, const glm::vec3& start, const glm::vec3& direction, float maxDistance)
{
    //1.0f is size of one block. 0.1f is 1/10th checks. Good for diagonals
    constexpr float stepSize = 0.1f;

    glm::ivec3 previousBlock
    {
        static_cast<int>(std::floor(start.x)),
        static_cast<int>(std::floor(start.y)),
        static_cast<int>(std::floor(start.z))
    };

    for (float distance = 0.0f; distance <= maxDistance; distance += stepSize)
    {
        glm::vec3 point = start + direction * distance;

        glm::ivec3 blockPos
        {
            static_cast<int>(std::floor(point.x)),
            static_cast<int>(std::floor(point.y)),
            static_cast<int>(std::floor(point.z))
        };

        if (previousBlock == blockPos)
        {
            //skip
            continue;
        }

        BlockType Block = world.GetBlockWorld(blockPos.x, blockPos.y, blockPos.z);

        if (Block != BlockType::Air)
        {
            return BlockHit
            {
                blockPos,
                previousBlock
            };
        }

        previousBlock = blockPos;
    }

    return std::nullopt;
}

void Application::UpdateCameraMouse(float deltaX, float deltaY)
{
    float sensitivity = 0.1f;

    m_Camera->AddYaw(deltaX * sensitivity);
    m_Camera->AddPitch(deltaY * sensitivity);
}

void Application::Shutdown()
{
    //Set the stop chunk worker.
    m_StopChunkWorker = true;

    //Notify a change. So it does not sleep
    m_ChunkCondition.notify_all();


    if (m_ChunkWorker.joinable())
    {
        m_ChunkWorker.join();
    }

    //Tear down GPU resources in reverse ownership order.
    //Grass
    for (RenderRecord& record : m_ChunkData)
    {
        record.grass.IBO.reset();
        record.grass.VAO.reset();
        record.grass.VBO.reset();

        record.dirt.IBO.reset();
        record.dirt.VAO.reset();
        record.dirt.VBO.reset();

        record.sand.IBO.reset();
        record.sand.VAO.reset();
        record.sand.VBO.reset();

        record.stone.IBO.reset();
        record.stone.VAO.reset();
        record.stone.VBO.reset();
    }

    //Rest everything
    m_CubeTexture.reset();
    m_CubeTexture2.reset();
    m_CubeTexture3.reset();
    m_CubeTexture4.reset();
    m_CubeTexture5.reset();
    m_CubeTexture6.reset();

    m_CubeShader.reset();
    m_Camera.reset();
    m_Window.reset();

}

void Application::UnloadShutDownChunk(ChunkPos pos)
{
    int targetX = pos.x;
    int targetZ = pos.z;

    for (auto it = m_ChunkData.begin(); it != m_ChunkData.end();)
    {
        if (it->pos.x == targetX && it->pos.z == targetZ)
        {

            it = m_ChunkData.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void Application::Update(float dt)
{
    const bool debugTogglePressed = m_Window->IsKeyPressed(GLFW_KEY_Q);
    if (debugTogglePressed && !m_DebugMenuTogglePressed)
    {
        //Flips opposite
        m_DebugMenuOpen = !m_DebugMenuOpen;
    }
    m_DebugMenuTogglePressed = debugTogglePressed;

    //Update Keyboard
    UpdatePlayer(dt);

    //Update Mouse
    UpdateBlockInteraction();

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

    ChunkPos cameraChunk = World::fromWorldPosition(m_Camera->GetPosition());
    GenerateChunksAroundCamera(cameraChunk);

    constexpr int maxChunkUploadsPerFrame = 2;
    int uploadedThisFrame = 0;

    while (uploadedThisFrame < maxChunkUploadsPerFrame)
    {
        FinishedChunk finished;

        {
            std::lock_guard lock(m_ChunkMutex);

            if (m_FinishedChunk.empty())
            {
                break;
            }

            finished = std::move(m_FinishedChunk.front());
            m_FinishedChunk.pop();
        }

        if (m_MeshedChunks.contains(finished.pos))
        {
            continue;
        }

        //Check if finished chunk is out of render distance
        if (abs(finished.pos.x - cameraChunk.x) > World::RenderDistance || abs(finished.pos.z - cameraChunk.z) > World::RenderDistance)
        {
            m_RequestedChunk.erase(finished.pos);
            continue;
        }

        m_World->AddChunk(finished.pos, std::move(finished.chunk));

        const Chunk* chunk = m_World->GetChunk(finished.pos);
        if (chunk)
        {
            BuildChunkMesher(finished.pos, *chunk, *m_World);
            m_MeshedChunks.insert(finished.pos);
            uploadedThisFrame++;
        }
    }

    UnloadChunskAroundCamera(cameraChunk);

    UpdateCameraMouse(deltaX, deltaY);
}

void Application::Render()
{
    Renderer::SetClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    Renderer::Clear();

    //Skip drawing chunk geometry until all required scene resources are ready.
    if (!m_Window || !m_CubeShader || !m_CubeTexture || !m_CubeTexture2 || !m_CubeTexture3 || !m_CubeTexture4 || !m_CubeTexture5 || !m_CubeTexture6 || !m_Camera)
    {
        return;
    }

    const float aspect = static_cast<float>(m_Window->GetWidth()) / static_cast<float>(m_Window->GetHeight());
    glm::mat4 projection = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 100.0f); //Increase from 100 for farPlane. View distance dos not match chunk being rendered
    glm::mat4 view = m_Camera->GetViewMatrix();

    m_CubeShader->Bind();

    //Draw call. 3 SEPERATE!!!
    for (const RenderRecord& record : m_ChunkData)
    {

        glm::vec3 chunkOffset(
            static_cast<float>(record.pos.x * Chunk::Width),
            0.0f,
            static_cast<float>(record.pos.z * Chunk::Depth)
        );

        glm::mat4 model = glm::translate(glm::mat4(1.0f), chunkOffset);
        glm::mat4 mvp = projection * view * model;
        m_CubeShader->SetUniformMat4f("u_MVP", mvp);

        if (CheckValid(record.sand))
        {
            m_CubeTexture2->Bind(0);
            Renderer::Draw(*record.sand.VAO, *record.sand.IBO, *m_CubeShader);
        }

        if (CheckValid(record.dirt))
        {
            m_CubeTexture->Bind(0);
            Renderer::Draw(*record.dirt.VAO, *record.dirt.IBO, *m_CubeShader);
        }

        if (CheckValid(record.stone))
        {
            m_CubeTexture3->Bind(0);
            Renderer::Draw(*record.stone.VAO, *record.stone.IBO, *m_CubeShader);
        }

        if (CheckValid(record.grass))
        {
            m_CubeTexture4->Bind(0);
            Renderer::Draw(*record.grass.VAO, *record.grass.IBO, *m_CubeShader);
        }

        if (CheckValid(record.log))
        {
            m_CubeTexture5->Bind(0);
            Renderer::Draw(*record.log.VAO, *record.log.IBO, *m_CubeShader);
        }

        if (CheckValid(record.leaves))
        {
            m_CubeTexture6->Bind(0);
            Renderer::Draw(*record.leaves.VAO, *record.leaves.IBO, *m_CubeShader);
        }

    }

    //Move Camera
    //m_Camera->AddYaw(1.0f);
    //->AddPitch(0.1f);
    //m_Camera->MoveForward(0.01f);
    //m_Camera->MoveRight(0.1f);
}

void Application::BuildChunkMesher(ChunkPos pos, const Chunk& chunk, const World& world)
{

    ChunkMeshes mesh = ::BuildChunkMesh(chunk, pos, world);

    RenderRecord record{};
    record.pos = pos;


    //Dirtmesh
    //Stonemesh
    //Grassmesh
    UploadMesh(mesh.Dirt, record.dirt);
    UploadMesh(mesh.Sand, record.sand);
    UploadMesh(mesh.Stone, record.stone);
    UploadMesh(mesh.Grass, record.grass);
    UploadMesh(mesh.Log, record.log);
    UploadMesh(mesh.Leaves, record.leaves);

    m_ChunkData.push_back(std::move(record));
}

void Application::RebuildChunkMesher(ChunkPos pos)
{
    UnloadShutDownChunk(pos);

    const Chunk* chunk = m_World->GetChunk(pos);

    if (chunk)
    {
        BuildChunkMesher(pos, *chunk, *m_World);
    }

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

void Application::GenerateChunksAroundCamera(ChunkPos cameraChunk)
{
    int renderDistance = World::RenderDistance;

    //Renders a square. Probably deeper look at this...
    for (int z = cameraChunk.z - renderDistance; z <= cameraChunk.z + renderDistance; z++)
    {
        for (int x = cameraChunk.x - renderDistance; x <= cameraChunk.x + renderDistance; x++)
        {
            ChunkPos pos{ x, z };

            //Check Duplicate
            if (m_World->HasChunk(pos) || m_RequestedChunk.contains(pos))
            {
                continue;
            }

            //Lock and unlock out of scope
            {
                std::lock_guard lock(m_ChunkMutex);
                m_ChunkJob.push(pos);
            }

            //Insert requested chunk
            m_RequestedChunk.insert(pos);
            m_ChunkCondition.notify_one();
        }
    }
}

void Application::UnloadChunskAroundCamera(ChunkPos cameraChunk)
{
    int renderDistance = World::RenderDistance;

    for (auto it = m_MeshedChunks.begin(); it != m_MeshedChunks.end();)
    {
        const ChunkPos& pos = *it;

        //Check the distance.
        if (abs(pos.x - cameraChunk.x) > renderDistance || abs(pos.z - cameraChunk.z) > renderDistance)
        {
            //START UNLOADING EVERYTHING
            m_World->UnloadChunk(pos);
            UnloadShutDownChunk(pos);
            m_RequestedChunk.erase(pos);
            it = m_MeshedChunks.erase(it);
        }
        else
        {
            it++;
        }
    }
}

glm::vec3 Application::getFront() const
{
    return m_Camera->getFront();
}
