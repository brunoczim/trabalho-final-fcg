#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <glm/vec3.hpp>

#define WORLD_SIZE_X 256
#define WORLD_SIZE_Y 128
#define WORLD_SIZE_Z 256

enum Block {
    BLOCK_AIR,
    BLOCK_STONE,
    BLOCK_GRASS
};

struct WorldBlockMatrix {
    Block blocks[WORLD_SIZE_X][WORLD_SIZE_Y][WORLD_SIZE_Z];

    WorldBlockMatrix();

    Block &operator [] (glm::vec3 point);

    Block const &operator [] (glm::vec3 point) const;
};

#endif // BLOCK_HPP
