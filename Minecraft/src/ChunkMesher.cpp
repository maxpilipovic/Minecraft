#include "ChunkMesher.h"
#include "ChunkMesh.h"

namespace
{
	float ToFloat(int value)
	{
		return static_cast<float>(value);
	}

	bool IsAirOrOutOfBounds(const Chunk& chunk, int x, int y, int z)
	{
		if (x < 0 || x >= Chunk::Width
			|| y < 0 || y >= Chunk::Height
			|| z < 0 || z >= Chunk::Depth)
		{
			return true;
		}

		return chunk.GetBlock(x, y, z) == BlockType::Air;
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

ChunkMeshes BuildChunkMesh(const Chunk& chunk)
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
						mesh.Dirt = DirtMesh(mesh.Dirt, chunk, x, y, z);
						break;
					case BlockType::Grass:
						mesh.Grass = GrassMesh(mesh.Grass, chunk, x, y, z);
						break;
					case BlockType::Stone:
						mesh.Stone = StoneMesh(mesh.Stone, chunk, x, y, z);
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

ChunkMesh GrassMesh(ChunkMesh& mesh, const Chunk& chunk, int x, int y, int z)
{

	AddFaces(mesh, chunk, x, y, z);
	return mesh;
}

ChunkMesh StoneMesh(ChunkMesh& mesh, const Chunk& chunk, int x, int y, int z)
{
	AddFaces(mesh, chunk, x, y, z);
	return mesh;
}

ChunkMesh DirtMesh(ChunkMesh& mesh, const Chunk& chunk, int x, int y, int z)
{
	AddFaces(mesh, chunk, x, y, z);
	return mesh;
}

void AddFaces(ChunkMesh& mesh, const Chunk& chunk, int x, int y, int z)
{
	if (IsAirOrOutOfBounds(chunk, x, y, z + 1))
	{
		AddFrontFace(mesh, x, y, z);
	}

	if (IsAirOrOutOfBounds(chunk, x, y, z - 1))
	{
		AddBackFace(mesh, x, y, z);
	}

	if (IsAirOrOutOfBounds(chunk, x + 1, y, z))
	{
		AddRightFace(mesh, x, y, z);
	}

	if (IsAirOrOutOfBounds(chunk, x - 1, y, z))
	{
		AddLeftFace(mesh, x, y, z);
	}

	if (IsAirOrOutOfBounds(chunk, x, y + 1, z))
	{
		AddTopFace(mesh, x, y, z);
	}

	if (IsAirOrOutOfBounds(chunk, x, y - 1, z))
	{
		AddBottomFace(mesh, x, y, z);
	}
}
