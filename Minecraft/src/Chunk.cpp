#include "Chunk.h"

#include <cassert>
#include <cstddef>

namespace
{
    constexpr bool IsInBounds(int x, int y, int z)
    {
        return x >= 0 && x < Chunk::Width
            && y >= 0 && y < Chunk::Height
            && z >= 0 && z < Chunk::Depth;
    }

    constexpr std::size_t GetBlockIndex(int x, int y, int z)
    {
        return static_cast<std::size_t>(x + (y * Chunk::Width) + (z * Chunk::Width * Chunk::Height));
    }
}

BlockType Chunk::GetBlock(int x, int y, int z) const
{
    //Should be valid. IF NOT CRASH
    assert(IsInBounds(x, y, z));

    return Blocks[GetBlockIndex(x, y, z)];
}

void Chunk::SetBlock(int x, int y, int z, BlockType block)
{
    assert(IsInBounds(x, y, z));

    Blocks[GetBlockIndex(x, y, z)] = block;
}
