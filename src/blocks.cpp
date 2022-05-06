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
    for(size_t x = 0;x<WORLD_SIZE_X;x++){
        for(size_t y = 0;y<WORLD_SIZE_Y/2;y++){
            for(size_t z = 0;z<WORLD_SIZE_Z;z++){
                (*this)[WorldPoint(x,y,z)] = BLOCK_STONE;
            }
        }
    }
}



