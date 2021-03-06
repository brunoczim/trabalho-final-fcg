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
bool coordenadaCruza(glm::vec3 a,float tA, glm::vec3 b, float tB, int axis);
bool colisaoCuboCubo(glm::vec3 centroCubo1, float t1, glm::vec3 centroCubo2, float t2);
bool IsPointInWorld(glm::vec3 point);
bool colisaoCuboPlanoOrdinais(glm::vec3 centroCubo, float t, int axis, float coordAxis);
//Apenas faz teste com o planos ortogonais aos eixos x,y,z pois s?o os unicos que usamos

#endif // COLLISIONS_HPP

