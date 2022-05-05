#include "blocks.hpp"
#include <math.h>

const WorldPoint WorldBlockMatrix::SIZE = WorldPoint(WORLD_SIZE_X, WORLD_SIZE_Y, WORLD_SIZE_Z);

WorldPoint::WorldPoint(size_t x, size_t y, size_t z): x(x), y(y), z(z)
{
}

WorldPoint::WorldPoint(glm::vec3 point):
    WorldPoint((size_t) round(point.x), (size_t) round(point.y), (size_t) round(point.z))
{
}

glm::vec3 WorldPoint::ToGlm()
{
    return glm::vec3(this->x, this->y, this->z);
}

WorldBlockMatrix::WorldBlockMatrix(): blocks { BLOCK_AIR }
{
}

Block &WorldBlockMatrix::operator [] (WorldPoint point)
{
    return this->blocks[point.x][point.y][point.z];
}

Block &WorldBlockMatrix::operator [] (glm::vec3 point)
{
    return (*this)[WorldPoint(point)];
}

Block const &WorldBlockMatrix::operator [] (WorldPoint point) const
{
    return this->blocks[point.x][point.y][point.z];
}

Block const &WorldBlockMatrix::operator [] (glm::vec3 point) const
{
    return (*this)[WorldPoint(point)];
}
Block WorldBlockMatrix::getPoint(int x,int y,int z){
    return this->blocks[x][y][z];
}

void WorldBlockMatrix::setPoint(int x,int y,int z, Block blockType){
    this->blocks[x][y][z] = blockType;
}
