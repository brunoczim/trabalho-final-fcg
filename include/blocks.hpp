#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <glm/vec3.hpp>

#define WORLD_SIZE_X 32
#define WORLD_SIZE_Y 32
#define WORLD_SIZE_Z 32

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

    inline Block &operator [] (WorldPoint point)
    {
        return this->blocks[point.x][point.y][point.z];
    }

    inline Block &operator [] (glm::vec3 point)
    {
        return (*this)[WorldPoint(point)];
    }

    inline Block const &operator [] (WorldPoint point) const
    {
        return this->blocks[point.x][point.y][point.z];
    }

    inline Block const &operator [] (glm::vec3 point) const
    {
        return (*this)[WorldPoint(point)];
    }

    bool IsPointInWorld(glm::vec3 point)const;


};

#endif // BLOCK_HPP
