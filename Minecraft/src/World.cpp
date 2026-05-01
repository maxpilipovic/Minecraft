#include "World.h"
#include <algorithm>
#include <cmath>

namespace
{
    constexpr int kRenderDistance = 8;

    constexpr int kBaseHeightNoise = 1337;
    constexpr int kMountainNoise = 2001;
    constexpr int kBiomeNoise = 9001;

    constexpr int kDirtDepth = 4;

    constexpr float kBaseHeightTerrainFrequency = 0.035f;
    constexpr float kMountainTerrainFrequency = 0.06f;
    constexpr float kBiomeTerrainFrequency = 0.012f;

    constexpr float kDesertBiomeThreshold = 0.38f;
    constexpr float kMountainBiomeThreshold = 0.88f;

    int getSurfaceHeight(const FastNoiseLite& baseNoise, const FastNoiseLite& mountainNoise, const FastNoiseLite& biomeNoise, BiomeType biome, int worldX, int worldZ)
    {
        float base = baseNoise.GetNoise(static_cast<float>(worldX), static_cast<float>(worldZ));
        base = (base + 1.0f) * 0.5f;

        float mountain = mountainNoise.GetNoise(static_cast<float>(worldX), static_cast<float>(worldZ));
        mountain = (mountain + 1.0f) * 0.5f;

        float height;

        switch (biome)
        {
            case BiomeType::Desert:
                height = 14.0f + base * 4.0f; //height 16 - 23 blocks
                break;
            case BiomeType::Mountains:
                height = 22.0f + mountain * 34.0f; //height 14 - 18 blocks
                break;
            case BiomeType::Plains:
            default:
                height = 16.0f + base * 7.0f; //height 22 - 56 blocks
                break;
        }

        float plainsHeight = 8.0f + base * 5.0f;
        float mountainHeight = 18.0f + mountain * 32.0f;

        return std::clamp((int)(height + 0.5f), 1, Chunk::Height - 1);
    }
}

World::World()
    : m_BaseHeightNoise(kBaseHeightNoise), m_MountainNoise(kMountainNoise), m_BiomeNoise(kBiomeNoise)
{
    m_BaseHeightNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_BaseHeightNoise.SetFrequency(kBaseHeightTerrainFrequency);
    m_BaseHeightNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_BaseHeightNoise.SetFractalOctaves(3);

    m_MountainNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_MountainNoise.SetFrequency(kMountainTerrainFrequency);
    m_MountainNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);
    m_MountainNoise.SetFractalOctaves(4);

    m_BiomeNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_BiomeNoise.SetFrequency(kBiomeTerrainFrequency);
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
            
            BiomeType biome = getBiome(m_BiomeNoise, worldX, worldZ);
            BlockType surfaceBlock = getSurfaceBlock(biome);

            const int surfaceY = getSurfaceHeight(m_BaseHeightNoise, m_MountainNoise, m_BiomeNoise, biome, worldX, worldZ);

            for (int y{}; y <= surfaceY; y++)
            {
                if (y == surfaceY)
                {
                    currChunk.SetBlock(x, y, z, surfaceBlock);
                }
                else if (y >= surfaceY - (kDirtDepth - 1))
                {
                    currChunk.SetBlock(x, y, z, biome == BiomeType::Desert ? BlockType::Sand : BlockType::Dirt);
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

BiomeType World::getBiome(const FastNoiseLite& biomeNoise, int worldX, int worldZ)
{
    float value = biomeNoise.GetNoise(static_cast<float>(worldX), static_cast<float>(worldZ));
    value = (value + 1.0f) * 0.5f;

    if (value < kDesertBiomeThreshold)
    {
        return BiomeType::Desert;
    }
    else if (value > kMountainBiomeThreshold)
    {
        return BiomeType::Mountains;
    }

    return BiomeType::Plains;
}

BlockType World::getSurfaceBlock(BiomeType biome)
{
    switch (biome)
    {
    case BiomeType::Desert:
        return BlockType::Sand;
    case BiomeType::Mountains:
        return BlockType::Stone;
    case BiomeType::Plains:
    default:
        return BlockType::Grass;
    }
}
