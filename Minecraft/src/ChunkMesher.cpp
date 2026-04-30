#include "ChunkMesher.h"
#include "ChunkMesh.h"

namespace
{
	float ToFloat(int value)
	{
		return static_cast<float>(value);
	}

	bool IsNeighborTransparent(const Chunk& chunk, int x, int y, int z, ChunkPos pos, const World& world)
	{
		//No vertical chunk neighbors
		if (y < 0 || y >= Chunk::Height)
		{
			return true;
		}

		//Neighbor same chunk unless proven otherwise
		ChunkPos neighborPos = pos;
		int localX = x;
		int localZ = z;

		if (localX < 0)
		{
			neighborPos.x -= 1;
			localX += Chunk::Width;
		}
		else if (localX >= Chunk::Width)
		{
			neighborPos.x += 1;
			localX -= Chunk::Width;
		}

		if (localZ < 0)
		{
			neighborPos.z -= 1;
			localZ += Chunk::Depth;
		}
		else if (localZ >= Chunk::Depth)
		{
			neighborPos.z += 1;
			localZ -= Chunk::Depth;
		}

		if (neighborPos == pos)
		{
			return chunk.GetBlock(localX, y, localZ) == BlockType::Air;
		}

		//Treat missing chunk as empty space
		const Chunk* neighborChunk = world.GetChunk(neighborPos);
		if (!neighborChunk)
		{
			return true;
		}

		return neighborChunk->GetBlock(localX, y, localZ) == BlockType::Air;
	}

	void AddFrontFace(ChunkMesh& mesh, int x, int y, int z)
	{
		const uint32_t baseIndex = static_cast<uint32_t>(mesh.Vertices.size());

		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x), ToFloat(y), ToFloat(z + 1), 0.0f, 0.0f });
		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x + 1), ToFloat(y), ToFloat(z + 1), 1.0f, 0.0f });
		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x + 1), ToFloat(y + 1), ToFloat(z + 1), 1.0f, 1.0f });
		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x), ToFloat(y + 1), ToFloat(z + 1), 0.0f, 1.0f });

		mesh.Indices.push_back(baseIndex + 0);
		mesh.Indices.push_back(baseIndex + 1);
		mesh.Indices.push_back(baseIndex + 2);
		mesh.Indices.push_back(baseIndex + 2);
		mesh.Indices.push_back(baseIndex + 3);
		mesh.Indices.push_back(baseIndex + 0);
	}

	void AddBackFace(ChunkMesh& mesh, int x, int y, int z)
	{
		const uint32_t baseIndex = static_cast<uint32_t>(mesh.Vertices.size());

		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x + 1), ToFloat(y), ToFloat(z), 0.0f, 0.0f });
		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x), ToFloat(y), ToFloat(z), 1.0f, 0.0f });
		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x), ToFloat(y + 1), ToFloat(z), 1.0f, 1.0f });
		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x + 1), ToFloat(y + 1), ToFloat(z), 0.0f, 1.0f });

		mesh.Indices.push_back(baseIndex + 0);
		mesh.Indices.push_back(baseIndex + 1);
		mesh.Indices.push_back(baseIndex + 2);
		mesh.Indices.push_back(baseIndex + 2);
		mesh.Indices.push_back(baseIndex + 3);
		mesh.Indices.push_back(baseIndex + 0);
	}

	void AddRightFace(ChunkMesh& mesh, int x, int y, int z)
	{
		const uint32_t baseIndex = static_cast<uint32_t>(mesh.Vertices.size());

		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x + 1), ToFloat(y), ToFloat(z + 1), 0.0f, 0.0f });
		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x + 1), ToFloat(y), ToFloat(z), 1.0f, 0.0f });
		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x + 1), ToFloat(y + 1), ToFloat(z), 1.0f, 1.0f });
		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x + 1), ToFloat(y + 1), ToFloat(z + 1), 0.0f, 1.0f });

		mesh.Indices.push_back(baseIndex + 0);
		mesh.Indices.push_back(baseIndex + 1);
		mesh.Indices.push_back(baseIndex + 2);
		mesh.Indices.push_back(baseIndex + 2);
		mesh.Indices.push_back(baseIndex + 3);
		mesh.Indices.push_back(baseIndex + 0);
	}

	void AddLeftFace(ChunkMesh& mesh, int x, int y, int z)
	{
		const uint32_t baseIndex = static_cast<uint32_t>(mesh.Vertices.size());

		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x), ToFloat(y), ToFloat(z), 0.0f, 0.0f });
		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x), ToFloat(y), ToFloat(z + 1), 1.0f, 0.0f });
		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x), ToFloat(y + 1), ToFloat(z + 1), 1.0f, 1.0f });
		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x), ToFloat(y + 1), ToFloat(z), 0.0f, 1.0f });

		mesh.Indices.push_back(baseIndex + 0);
		mesh.Indices.push_back(baseIndex + 1);
		mesh.Indices.push_back(baseIndex + 2);
		mesh.Indices.push_back(baseIndex + 2);
		mesh.Indices.push_back(baseIndex + 3);
		mesh.Indices.push_back(baseIndex + 0);
	}

	void AddTopFace(ChunkMesh& mesh, int x, int y, int z)
	{
		const uint32_t baseIndex = static_cast<uint32_t>(mesh.Vertices.size());

		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x), ToFloat(y + 1), ToFloat(z + 1), 0.0f, 0.0f });
		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x + 1), ToFloat(y + 1), ToFloat(z + 1), 1.0f, 0.0f });
		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x + 1), ToFloat(y + 1), ToFloat(z), 1.0f, 1.0f });
		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x), ToFloat(y + 1), ToFloat(z), 0.0f, 1.0f });

		mesh.Indices.push_back(baseIndex + 0);
		mesh.Indices.push_back(baseIndex + 1);
		mesh.Indices.push_back(baseIndex + 2);
		mesh.Indices.push_back(baseIndex + 2);
		mesh.Indices.push_back(baseIndex + 3);
		mesh.Indices.push_back(baseIndex + 0);
	}

	void AddBottomFace(ChunkMesh& mesh, int x, int y, int z)
	{
		const uint32_t baseIndex = static_cast<uint32_t>(mesh.Vertices.size());

		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x), ToFloat(y), ToFloat(z), 0.0f, 0.0f });
		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x + 1), ToFloat(y), ToFloat(z), 1.0f, 0.0f });
		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x + 1), ToFloat(y), ToFloat(z + 1), 1.0f, 1.0f });
		mesh.Vertices.push_back(ChunkVertex{ ToFloat(x), ToFloat(y), ToFloat(z + 1), 0.0f, 1.0f });

		mesh.Indices.push_back(baseIndex + 0);
		mesh.Indices.push_back(baseIndex + 1);
		mesh.Indices.push_back(baseIndex + 2);
		mesh.Indices.push_back(baseIndex + 2);
		mesh.Indices.push_back(baseIndex + 3);
		mesh.Indices.push_back(baseIndex + 0);
	}
}

ChunkMeshes BuildChunkMesh(const Chunk& chunk, ChunkPos pos, const World& world)
{
	ChunkMeshes mesh;

	for (int z{}; z < Chunk::Depth; z++)
	{
		for (int y{}; y < Chunk::Height; y++)
		{
			for (int x{}; x < Chunk::Width; x++)
			{

				BlockType Block = chunk.GetBlock(x, y, z);

				switch (Block)
				{
					case BlockType::Dirt:
						mesh.Dirt = DirtMesh(mesh.Dirt, chunk, x, y, z, pos, world);
						break;
					case BlockType::Sand:
						mesh.Sand = SandMesh(mesh.Sand, chunk, x, y, z, pos, world);
						break;
					case BlockType::Stone:
						mesh.Stone = StoneMesh(mesh.Stone, chunk, x, y, z, pos, world);
						break;
					case BlockType::Grass:
						mesh.Grass = GrassMesh(mesh.Grass, chunk, x, y, z, pos, world);
						break;
					case BlockType::Air:
						//Do nothing
						break;
				}
			}
		}
	}

	return mesh;
}

ChunkMesh SandMesh(ChunkMesh& mesh, const Chunk& chunk, int x, int y, int z, ChunkPos pos, const World& world)
{

	AddFaces(mesh, chunk, x, y, z, pos, world);
	return mesh;
}

ChunkMesh StoneMesh(ChunkMesh& mesh, const Chunk& chunk, int x, int y, int z, ChunkPos pos, const World& world)
{
	AddFaces(mesh, chunk, x, y, z, pos, world);
	return mesh;
}

ChunkMesh DirtMesh(ChunkMesh& mesh, const Chunk& chunk, int x, int y, int z, ChunkPos pos, const World& world)
{
	AddFaces(mesh, chunk, x, y, z, pos, world);
	return mesh;
}

ChunkMesh GrassMesh(ChunkMesh& mesh, const Chunk& chunk, int x, int y, int z, ChunkPos pos, const World& world)
{
	AddFaces(mesh, chunk, x, y, z, pos, world);
	return mesh;
}

void AddFaces(ChunkMesh& mesh, const Chunk& chunk, int x, int y, int z, ChunkPos pos, const World& world)
{
	if (IsNeighborTransparent(chunk, x, y, z + 1, pos, world))
	{
		AddFrontFace(mesh, x, y, z);
	}

	if (IsNeighborTransparent(chunk, x, y, z - 1, pos, world))
	{
		AddBackFace(mesh, x, y, z);
	}

	if (IsNeighborTransparent(chunk, x + 1, y, z, pos, world))
	{
		AddRightFace(mesh, x, y, z);
	}

	if (IsNeighborTransparent(chunk, x - 1, y, z, pos, world))
	{
		AddLeftFace(mesh, x, y, z);
	}

	if (IsNeighborTransparent(chunk, x, y + 1, z, pos, world))
	{
		AddTopFace(mesh, x, y, z);
	}

	if (IsNeighborTransparent(chunk, x, y - 1, z, pos, world))
	{
		AddBottomFace(mesh, x, y, z);
	}
}
