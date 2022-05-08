#include "collisions.hpp"
#include "matrices.hpp"
#include <cmath>
#include <iostream>

bool FacingNonAirBlock(CollisionFace &output, Camera const &camera, WorldBlockMatrix const &world_block_matrix)
{
    float max_distance = 5.0f;
    float total_distance = 0.0f;
    bool block_selected = false;

    glm::vec4 view_vector = camera.ViewVector() / norm(camera.ViewVector());
    glm::vec4 target_point = camera.CenterPoint();
    glm::vec3 matrix_position;

    glm::vec4 distances = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    size_t min_axis = 0;

    while (total_distance < max_distance && !block_selected) {
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
        matrix_position = target_point;
        matrix_position[min_axis] += view_vector[min_axis] / abs(view_vector[min_axis]) * 0.01f;
        block_selected = world_block_matrix[target_point] != BLOCK_AIR;
        std::cout<<total_distance<<std::endl;
    }

    if (block_selected) {
        output.block_position = target_point;
        output.axis = min_axis;
        output.sign = (int) -(distances[min_axis] / abs(distances[min_axis]));
    }
    return block_selected;
}