#include "World.h"
#include <algorithm>
#include <cmath>

namespace
{
    constexpr int kRenderDistance = 8;
    constexpr int kTerrainSeed = 1337;
    constexpr float kTerrainFrequency = 0.035f;
    constexpr int kTerrainMinHeight = 4;
    constexpr int kSurfaceDepth = 4;

    int getSurfaceHeight(const FastNoiseLite& noise, int worldX, int worldZ)
    {
        const float noiseValue = noise.GetNoise(static_cast<float>(worldX), static_cast<float>(worldZ));
        const float normalizedNoise = (noiseValue + 1.0f) * 0.5f;
        const float scaledHeight = normalizedNoise * static_cast<float>(Chunk::Height - 1 - kTerrainMinHeight);
        const int height = kTerrainMinHeight + static_cast<int>(scaledHeight + 0.5f);

        return std::clamp(height, kTerrainMinHeight, Chunk::Height - 1);
    }
}

World::World()
    : m_TerrainNoise(kTerrainSeed)
{
    m_TerrainNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_TerrainNoise.SetFrequency(kTerrainFrequency);
    m_TerrainNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_TerrainNoise.SetFractalOctaves(4);
    m_TerrainNoise.SetFractalLacunarity(2.0f);
    m_TerrainNoise.SetFractalGain(0.5f);
}

bool ChunkPos::operator==(const ChunkPos& other) const
{
    return x == other.x && z == other.z;
}

std::size_t ChunkPosHash::operator()(const ChunkPos& pos) const
{
	std::size_t x = std::hash<int>()(pos.x);
	std::size_t y = std::hash<int>()(pos.z);

	return x ^ (y << 1);
}

void World::GenerateChunk(ChunkPos pos)
{

    //iterators returns key, value pair
    auto [it, inserted] = m_AllChunks.try_emplace(pos);

    //If false
    if (!inserted)
    {
        return;
    }

    //Grab the curr chunk
    Chunk& currChunk = it->second;

    //Generate
    for (int z{}; z < Chunk::Depth; z++)
    {
        for (int x{}; x < Chunk::Width; x++)
        {
            const int worldX = pos.x * Chunk::Width + x;
            const int worldZ = pos.z * Chunk::Depth + z;
            const int surfaceY = getSurfaceHeight(m_TerrainNoise, worldX, worldZ);

            for (int y{}; y <= surfaceY; y++)
            {
                if (y == surfaceY)
                {
                    currChunk.SetBlock(x, y, z, BlockType::Sand);
                }
                else if (y >= surfaceY - (kSurfaceDepth - 1))
                {
                    currChunk.SetBlock(x, y, z, BlockType::Dirt);
                }
                else
                {
                    currChunk.SetBlock(x, y, z, BlockType::Stone);
                }
            }
        }
    }
}

const Chunk* World::GetChunk(ChunkPos chunk) const
{

    //Grabs the chunk
    auto it = m_AllChunks.find(chunk);
    
    if (it == m_AllChunks.end())
    {
        //Return empty chunk
        return nullptr;
    }

    return &it->second;
}

ChunkPos World::fromWorldPosition(const glm::vec3& camPos)
{
    int x = static_cast<int>(floor(camPos.x / Chunk::Width));
    int z = static_cast<int>(floor(camPos.z / Chunk::Depth));

    return { x, z };
}

bool World::HasChunk(ChunkPos chunk) const
{

    if (m_AllChunks.contains(chunk))
    {
        return true;
    }

    return false;
}

void World::UnloadChunk(ChunkPos chunk)
{
    m_AllChunks.erase(chunk);
}