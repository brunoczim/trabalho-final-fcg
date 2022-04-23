#include "blocks.hpp"

WorldBlockMatrix::WorldBlockMatrix(): blocks { BLOCK_AIR }
{
}

Block &WorldBlockMatrix::operator [] (glm::vec3 point)
{
    return blocks[(size_t) point.x][(size_t) point.y][(size_t) point.z];
}

Block const &WorldBlockMatrix::operator [] (glm::vec3 point) const
{
    return blocks[(size_t) point.x][(size_t) point.y][(size_t) point.z];
}
