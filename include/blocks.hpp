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

struct WorldPoint {
public:
    size_t x;
    size_t y;
    size_t z;

    WorldPoint(size_t x, size_t y, size_t z);
    WorldPoint(glm::vec3 point);

    glm::vec3 ToGlm();
};

struct WorldBlockMatrix {
private:
    Block blocks[WORLD_SIZE_X][WORLD_SIZE_Y][WORLD_SIZE_Z];

public:
    WorldBlockMatrix();

    static const WorldPoint SIZE;

    Block &operator [] (glm::vec3 point);

    Block &operator [] (WorldPoint point);

    Block const &operator [] (glm::vec3 point) const;

    Block const &operator [] (WorldPoint point) const;
};

#endif // BLOCK_HPP
