#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include <glm/vec4.hpp>
#include "Camera.hpp"
#include "blocks.hpp"

struct CollisionFace {
    glm::vec3 block_position;
    size_t axis;
    int sign;
};

bool FacingNonAirBlock(CollisionFace &output, Camera const &camera, WorldBlockMatrix const &world_block_matrix);

#endif // COLLISIONS_HPP
