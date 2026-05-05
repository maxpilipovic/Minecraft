#pragma once

#include <memory>
#include <unordered_set>
#include "Window.h"
#include "Chunk.h"
#include "World.h"

#include <mutex>
#include <queue>
#include <thread>
#include <atomic>
#include <condition_variable>

//Forward declarations?
class VertexArray;
class VertexBuffer;
class IndexBuffer;
class Shader;
class Texture;
class Camera;
class World;
struct ChunkMesh;

struct FinishedChunk
{
    ChunkPos pos;
    Chunk chunk;
};

struct RenderMesh
{
    std::unique_ptr<VertexArray> VAO;
    std::unique_ptr<VertexBuffer> VBO;
    std::unique_ptr<IndexBuffer> IBO;
};

//Stores Render Chunk Info
struct RenderRecord
{
    ChunkPos pos;
    RenderMesh grass;
    RenderMesh dirt;
    RenderMesh stone;
    RenderMesh sand;
};

class Application
{
public:
    static Application& Get();

    void Run();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

private:
    Application();
    ~Application();

    void Init();
    void Shutdown();
    void UnloadShutDownChunk(ChunkPos pos);
    void Update(float dt);
    void Render();

    //Update Camera
    void UpdateCameraKeyboard(float deltaTime);
    void UpdateCameraMouse(float deltaX, float deltaY);

    //Chunk Mesh
    void BuildChunkMesher(ChunkPos pos, const Chunk&, const World& world);
    void GenerateChunksAroundCamera(ChunkPos cameraPos);
    void UnloadChunskAroundCamera(ChunkPos cameraPos);

    bool CheckValid(const RenderMesh& gpu);
    void UploadMesh(const ChunkMesh& mesh, RenderMesh& gpu);


private:
    //Renderer resource ownership
    std::unique_ptr<Window> m_Window;
    std::unique_ptr<Shader> m_CubeShader;
    std::unique_ptr<Texture> m_CubeTexture;
    std::unique_ptr<Texture> m_CubeTexture2;
    std::unique_ptr<Texture> m_CubeTexture3;
    std::unique_ptr<Texture> m_CubeTexture4;
    std::unique_ptr<Camera> m_Camera;

    //World
    std::unique_ptr<World> m_World;

    //Mouse stuff
    float m_LastX = 0.0f;
    float m_LastY = 0.0f;
    bool m_FirstMouse = true;

    float m_CubeRotation = 0.0f;
    bool m_Running = false;
    double m_LastFrameTime = 0.0;
    Chunk m_Chunk;

    //MeshedChunks
    std::unordered_set<ChunkPos, ChunkPosHash> m_MeshedChunks;

    //This holds GPU pipeline data.
    std::vector<RenderRecord> m_ChunkData;

    //THREAD SHIT
    std::thread m_ChunkWorker;
    std::mutex m_ChunkMutex;

    std::queue<ChunkPos> m_ChunkJob;
    std::queue<FinishedChunk> m_FinishedChunk;

    std::unordered_set<ChunkPos, ChunkPosHash> m_RequestedChunk;
    std::atomic<bool> m_StopChunkWorker = false;
    std::condition_variable m_ChunkCondition;
};
