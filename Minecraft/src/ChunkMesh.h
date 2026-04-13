#pragma once

#include <vector>
#include <cstdint>

struct ChunkVertex
{
	float x, y, z;
	float U, V;
};

struct ChunkMesh
{
	std::vector<ChunkVertex> Vertices;
	std::vector<uint32_t> Indices;
};
