#include "World.h"

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
            for (int y{}; y < Chunk::Height; y++)
            {
                currChunk.SetBlock(x, y, z, BlockType::Sand);
                if (y == currChunk.Height - 1)
                {
                    //Render Grass
                    currChunk.SetBlock(x, y, z, BlockType::Sand);
                }
                //If height 16
                //12
                else if ((y >= currChunk.Height - 4))
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

Chunk* World::GetChunk(ChunkPos chunk)
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