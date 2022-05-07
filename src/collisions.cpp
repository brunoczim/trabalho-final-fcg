#include "collisions.hpp"
#include "matrices.hpp"

bool FacingNonAirBlock(CollisionFace &output, Camera const &camera, WorldBlockMatrix const &world_block_matrix)
{
    float max_distance = 8.0f;
    float total_distance = 0.0f;
    bool block_selected = false;

    glm::vec4 view_vector = camera.ViewVector() / norm(camera.ViewVector());
    glm::vec4 target_point = camera.CenterPoint();

    glm::vec4 distances = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    size_t min_axis = 0;

    while (total_distance < max_distance && !block_selected) {
        min_axis = 0;
        distances = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

        for (size_t axis = 0; axis < 3; axis++) {
            if (view_vector[axis] > 0.0) {
                distances[axis] = ceil(target_point[axis] - 0.5) + 0.5 - target_point[axis];
            } else if (view_vector.x < 0.0) {
                distances[axis] = floor(target_point[axis] + 0.5) - 0.5 - target_point[axis];
            }
            distances[axis] = distances[axis] / view_vector[axis];

            if (distances[min_axis] > distances[axis]) {
                min_axis = axis;
            }
        }

        target_point = target_point + distances[min_axis] * view_vector;
        total_distance += norm(distances[min_axis] * view_vector);
        glm::vec3 matrix_position = target_point;
        matrix_position[min_axis] += distances[min_axis] / abs(distances[min_axis]) * 0.01f;
        block_selected = world_block_matrix[matrix_position] != BLOCK_AIR;
    }

    if (block_selected) {
        output.block_position = target_point;
        output.axis = min_axis;
        output.sign = (int) -(distances[min_axis] / abs(distances[min_axis]));
    }

    return block_selected;
}
