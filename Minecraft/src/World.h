#pragma once
#include <unordered_map>
#include "Chunk.h"

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

	void GenerateChunk(ChunkPos pos);

	//Get Chunk (pointer because nullptr is valid?)
	Chunk* GetChunk(ChunkPos chunk);

	//Something to store chunks?? Vec or Hashmap??

private:
	std::unordered_map<ChunkPos, Chunk, ChunkPosHash> m_AllChunks;

};
