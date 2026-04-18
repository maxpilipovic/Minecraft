#pragma once

#include "Chunk.h"
#include "ChunkMesh.h"

struct ChunkMeshes
{
	ChunkMesh Grass;
	ChunkMesh Stone;
	ChunkMesh Dirt;
};

ChunkMeshes BuildChunkMesh(const Chunk& chunk);

//Block Type Meshes
ChunkMesh GrassMesh(ChunkMesh& mesh, const Chunk& chunk, int x, int y, int z);
ChunkMesh StoneMesh(ChunkMesh& mesh, const Chunk& chunk, int x, int y, int z);
ChunkMesh DirtMesh(ChunkMesh& mesh, const Chunk& chunk, int x, int y, int z);

//Add Faces
void AddFaces(ChunkMesh& mesh, const Chunk& chunk, int x, int y, int z);
