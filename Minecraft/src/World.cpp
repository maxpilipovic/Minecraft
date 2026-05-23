#include "World.h"
#include <algorithm>
#include <cmath>

namespace
{
    constexpr int kRenderDistance = 8;

    constexpr int kBaseHeightNoise = 1337;
    constexpr int kMountainNoise = 2001;
    constexpr int kBiomeNoise = 9001;
    constexpr int kContinentalnessNoise = 4201;
    constexpr int kErosionNoise = 5101;
    constexpr int kRidgeNoise = 6203;
    constexpr int kDetailNoise = 7307;

    constexpr int kDirtDepth = 4;

    constexpr float kBaseHeightTerrainFrequency = 0.018f;
    constexpr float kMountainTerrainFrequency = 0.035f;
    constexpr float kBiomeTerrainFrequency = 0.0035f;
    constexpr float kContinentalnessFrequency = 0.0028f;
    constexpr float kErosionFrequency = 0.0065f;
    constexpr float kRidgeFrequency = 0.012f;
    constexpr float kDetailFrequency = 0.055f;

    constexpr float kDesertBiomeThreshold = 0.38f;
    constexpr float kMountainBiomeThreshold = 0.88f;

    struct TerrainColumn
    {
        int surfaceY;
        BiomeType biome;
    };

    //Coverts into a nice range
    float noise01(const FastNoiseLite& noise, int worldX, int worldZ)
    {
        return (noise.GetNoise(static_cast<float>(worldX), static_cast<float>(worldZ)) + 1.0f) * 0.5f;
    }

    float lerp(float a, float b, float t)
    {
        return a + (b - a) * t;
    }

    //Creates a soft 0..1 transition instead of a hard cutoff at one value.
    float smoothstep(float edge0, float edge1, float value)
    {
        float t = std::clamp((value - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }

    //Maps the broad biome noise value to the biome used for block selection.
    BiomeType getBiomeFromValue(float biome)
    {
        if (biome < kDesertBiomeThreshold)
        {
            return BiomeType::Desert;
        }

        if (biome > kMountainBiomeThreshold)
        {
            return BiomeType::Mountains;
        }

        return BiomeType::Plains;
    }

    //Combines climate, continent, erosion, ridge, and detail noise into one terrain column.
    TerrainColumn getTerrainColumn(
        const FastNoiseLite& baseNoise,
        const FastNoiseLite& mountainNoise,
        const FastNoiseLite& biomeNoise,
        const FastNoiseLite& continentalnessNoise,
        const FastNoiseLite& erosionNoise,
        const FastNoiseLite& ridgeNoise,
        const FastNoiseLite& detailNoise,
        int worldX,
        int worldZ)
    {
        const float base = noise01(baseNoise, worldX, worldZ);
        const float mountain = noise01(mountainNoise, worldX, worldZ);
        const float biome = noise01(biomeNoise, worldX, worldZ);
        const float continentalness = noise01(continentalnessNoise, worldX, worldZ);
        const float erosion = noise01(erosionNoise, worldX, worldZ);
        const float detail = noise01(detailNoise, worldX, worldZ);

        const float ridgeRaw = ridgeNoise.GetNoise(static_cast<float>(worldX), static_cast<float>(worldZ));
        const float ridge = 1.0f - std::abs(ridgeRaw);
        const float sharpRidge = ridge * ridge * ridge;

        const float desertToPlains = smoothstep(kDesertBiomeThreshold - 0.12f, kDesertBiomeThreshold + 0.12f, biome);
        const float mountainClimate = smoothstep(kMountainBiomeThreshold - 0.16f, kMountainBiomeThreshold + 0.10f, biome);
        const float inland = smoothstep(0.24f, 0.82f, continentalness);
        const float lowErosion = 1.0f - smoothstep(0.28f, 0.78f, erosion);
        const float mountainWeight = std::clamp(mountainClimate * inland * (0.35f + lowErosion * 0.65f), 0.0f, 1.0f);

        const float lowlandHeight = lerp(13.5f + base * 1.5f, 15.0f + base * 3.0f, desertToPlains);
        const float continentLift = lerp(-3.5f, 4.0f, inland);
        const float erosionFlatten = lerp(2.8f, 0.8f, erosion);
        const float rollingLand = (base - 0.5f) * erosionFlatten + (detail - 0.5f) * 0.8f;

        const float foothills = 17.0f + mountain * 6.0f + sharpRidge * 5.0f;
        const float peaks = 22.0f + mountain * 13.0f + sharpRidge * 12.0f + lowErosion * 6.0f;
        const float mountainHeight = lerp(foothills, peaks, smoothstep(0.40f, 0.95f, mountainWeight));

        const float baseHeight = lowlandHeight + continentLift + rollingLand;
        const float height = lerp(baseHeight, mountainHeight, mountainWeight);

        return {
            std::clamp((int)(height + 0.5f), 1, Chunk::Height - 1),
            getBiomeFromValue(biome)
        };
    }
}

World::World()
    : m_BaseHeightNoise(kBaseHeightNoise),
      m_MountainNoise(kMountainNoise),
      m_BiomeNoise(kBiomeNoise),
      m_ContinentalnessNoise(kContinentalnessNoise),
      m_ErosionNoise(kErosionNoise),
      m_RidgeNoise(kRidgeNoise),
      m_DetailNoise(kDetailNoise)
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

    m_ContinentalnessNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_ContinentalnessNoise.SetFrequency(kContinentalnessFrequency);
    m_ContinentalnessNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_ContinentalnessNoise.SetFractalOctaves(4);

    m_ErosionNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_ErosionNoise.SetFrequency(kErosionFrequency);
    m_ErosionNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_ErosionNoise.SetFractalOctaves(3);

    m_RidgeNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_RidgeNoise.SetFrequency(kRidgeFrequency);
    m_RidgeNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);
    m_RidgeNoise.SetFractalOctaves(4);

    m_DetailNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_DetailNoise.SetFrequency(kDetailFrequency);
    m_DetailNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_DetailNoise.SetFractalOctaves(2);
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

// Returns the biome at a world position using the same climate noise as terrain generation.
BiomeType World::getBiome(const FastNoiseLite& biomeNoise, int worldX, int worldZ) const
{
    return getBiomeFromValue(noise01(biomeNoise, worldX, worldZ));
}

//Chooses the visible top block for each biome.
BlockType World::getSurfaceBlock(BiomeType biome) const
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

//FIX Chunk is to big for stack
void World::AddChunk(ChunkPos pos, Chunk chunk)
{
    m_AllChunks.emplace(pos, std::move(chunk));
}

//FIX Chunk is to big for stack
//Builds one chunk by generating each terrain column and filling blocks up to its surface.
Chunk World::CreateChunk(ChunkPos pos) const
{

    Chunk chunk;

    //Generate TERRAIN GENERATION!!
    for (int z{}; z < Chunk::Depth; z++)
    {
        for (int x{}; x < Chunk::Width; x++)
        {
            const int worldX = pos.x * Chunk::Width + x;
            const int worldZ = pos.z * Chunk::Depth + z;

            TerrainColumn terrain = getTerrainColumn(m_BaseHeightNoise, m_MountainNoise, m_BiomeNoise, m_ContinentalnessNoise, m_ErosionNoise, m_RidgeNoise, m_DetailNoise, worldX, worldZ);
            BiomeType biome = terrain.biome;
            BlockType surfaceBlock = getSurfaceBlock(biome);

            const int surfaceY = terrain.surfaceY;

            for (int y{}; y <= surfaceY; y++)
            {
                if (y == surfaceY)
                {
                    chunk.SetBlock(x, y, z, surfaceBlock);
                }
                else if (y >= surfaceY - (kDirtDepth - 1))
                {
                    chunk.SetBlock(x, y, z, biome == BiomeType::Desert ? BlockType::Sand : BlockType::Dirt);
                }
                else
                {
                    chunk.SetBlock(x, y, z, BlockType::Stone);
                }
            }
        }
    }

    return chunk;
}

BlockType World::GetBlockWorld(int worldX, int worldY, int worldZ) const
{

    //Check outside y bounds
    if (worldY < 0 || worldY >= Chunk::Height)
    {
        return BlockType::Air;
	}

    //Grab chunk position
    ChunkPos chunkPos = fromWorldPosition({ static_cast<float>(worldX), 0.0f, static_cast<float>(worldZ) });

    //Retrieve it
    const Chunk* chunk = GetChunk(chunkPos);

    //If its air
    if (chunk == nullptr)
    {
        return BlockType::Air;
    }

    //If its a solid block, get its local position and return it
    int localX = worldX - chunkPos.x * Chunk::Width;
    int localZ = worldZ - chunkPos.z * Chunk::Depth;

    return chunk->GetBlock(localX, worldY, localZ);
}

bool World::SetBlockWorld(int worldX, int worldY, int worldZ, BlockType block)
{

    //Check outside y bounds
    if (worldY < 0 || worldY >= Chunk::Height)
    {
        return false;
    }

    //Grab chunk position
    ChunkPos chunkPos = fromWorldPosition({ static_cast<float>(worldX), 0.0f, static_cast<float>(worldZ) });

    //Find the chunk
    auto it = m_AllChunks.find(chunkPos);
    if (it == m_AllChunks.end())
    {
        return false;
    }

	//If its a solid block, get its local position and set it
    int localX = worldX - chunkPos.x * Chunk::Width;
    int localZ = worldZ - chunkPos.z * Chunk::Depth;

    //Set it!!!
    it->second.SetBlock(localX, worldY, localZ, block);

    return true;
}
