#pragma once
#include <unordered_map>
#include "Chunk.h"
#include <FastNoiseLite.h>
#include <glm/gtc/matrix_transform.hpp>
#include "mc.h"

enum class BiomeType
{
	Plains, 
	Desert,
	Mountains
};

struct ChunkPos
{
	int x;
	int z;

	bool operator==(const ChunkPos& other) const;
};

struct ChunkPosHash
{
	std::size_t operator()(const ChunkPos&) const;
};

class World
{
public:

	//Global RenderDistance
	static constexpr int RenderDistance = 8;

	World();

	//Get Chunk (pointer because nullptr is valid?)
	const Chunk* GetChunk(ChunkPos chunk) const;

	static ChunkPos fromWorldPosition(const glm::vec3& camPos);
	
	bool HasChunk(ChunkPos chunk) const;
	void UnloadChunk(ChunkPos chunk);

	void AddChunk(ChunkPos pos, Chunk chunk);
	Chunk CreateChunk(ChunkPos pos) const;

	//Biomes
	BiomeType getBiome(const FastNoiseLite& biomeNoise, int worldX, int worldZ) const;
	BlockType getSurfaceBlock(BiomeType biome) const;

	//Breaking/Placing
	BlockType GetBlockWorld(int worldX, int worldY, int worldZ) const;
	bool SetBlockWorld(int worldX, int worldY, int worldZ, BlockType block);


private:
	std::unordered_map<ChunkPos, Chunk, ChunkPosHash> m_AllChunks;
	FastNoiseLite m_BaseHeightNoise;
	FastNoiseLite m_MountainNoise;
	FastNoiseLite m_BiomeNoise;
	FastNoiseLite m_ContinentalnessNoise;
	FastNoiseLite m_ErosionNoise;
	FastNoiseLite m_RidgeNoise;
	FastNoiseLite m_DetailNoise;
};
