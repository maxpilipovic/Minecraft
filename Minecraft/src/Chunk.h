#pragma once

#include <array>
#include <cstdint>

enum class BlockType : uint8_t
{
    Air = 0,
    Grass,
    Dirt,
    Stone
};

//
enum class BlockFace : uint8_t
{
    Top,
    Bottom,
    Left,
    Right,
    Front,
    Back
};

struct Chunk
{
    static constexpr int Width = 16;
    static constexpr int Height = 16;
    static constexpr int Depth = 16;
    static constexpr int BlockCount = Width * Height * Depth;

    //Type, Size
    std::array<BlockType, BlockCount> Blocks{};

    BlockType GetBlock(int x, int y, int z) const;
    void SetBlock(int x, int y, int z, BlockType block);
};
