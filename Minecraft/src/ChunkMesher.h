#pragma once

#include "Chunk.h"
#include "ChunkMesh.h"
#include "World.h"

struct ChunkMeshes
{
	ChunkMesh Sand;
	ChunkMesh Stone;
	ChunkMesh Dirt;
};

ChunkMeshes BuildChunkMesh(const Chunk& chunk, ChunkPos pos, const World& world);

//Block Type Meshes
ChunkMesh SandMesh(ChunkMesh& mesh, const Chunk& chunk, int x, int y, int z, ChunkPos pos, const World& world);
ChunkMesh StoneMesh(ChunkMesh& mesh, const Chunk& chunk, int x, int y, int z, ChunkPos pos, const World& world);
ChunkMesh DirtMesh(ChunkMesh& mesh, const Chunk& chunk, int x, int y, int z, ChunkPos pos, const World& world);

//Add Faces
void AddFaces(ChunkMesh& mesh, const Chunk& chunk, int x, int y, int z, ChunkPos pos, const World& world);
