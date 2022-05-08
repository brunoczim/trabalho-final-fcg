#include "collisions.hpp"
#include "matrices.hpp"
#include <cmath>
#include <iostream>

bool FacingNonAirBlock(CollisionFace &output, Camera const &camera, WorldBlockMatrix const &world_block_matrix)
{
    float max_distance = 5.0f;
    float total_distance = 0.0f;
    bool block_selected = false;
    bool pointInWorld = true;

    glm::vec4 view_vector = camera.ViewVector() / norm(camera.ViewVector());
    glm::vec4 target_point = camera.CenterPoint();
    glm::vec3 matrix_position;

    glm::vec4 distances = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    size_t min_axis = 0;

    while (total_distance < max_distance && !block_selected && pointInWorld) {
        min_axis = 0;
        distances = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

        for (size_t axis = 0; axis < 3; axis++) {
            if (view_vector[axis] > 0.0) {
                distances[axis] = ceil(target_point[axis] - 0.5) + 0.5 - target_point[axis];
            } else if (view_vector[axis] < 0.0) {
                distances[axis] = floor(target_point[axis] + 0.5) - 0.5 - target_point[axis];
            }
            if((view_vector[axis] != 0.0)){
                distances[axis] = distances[axis] / view_vector[axis];
            }
            else{
                distances[axis] = fabs(INFINITY);
            }
            if(distances[axis]==0.0){
                distances[axis] = 0.0001;
            }

            if (distances[min_axis] > distances[axis]) {
                min_axis = axis;
            }
        }

        target_point = target_point + distances[min_axis] * view_vector;
        total_distance += norm(distances[min_axis] * view_vector);
        pointInWorld = world_block_matrix.IsPointInWorld(target_point);
        if(pointInWorld){
            block_selected = world_block_matrix[target_point] != BLOCK_AIR;
        }
    }

    if (block_selected && pointInWorld) {
        output.block_position = target_point;
        output.axis = min_axis;
        output.sign = view_vector[min_axis]/fabs(view_vector[min_axis]);
    }

    return block_selected;
}
