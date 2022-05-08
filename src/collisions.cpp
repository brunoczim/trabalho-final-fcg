#include "collisions.hpp"
#include "matrices.hpp"
#include <cmath>
#include <iostream>
#include "blocks.hpp"

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

bool IsPointInWorld(glm::vec3 point){
    return (point.x > -0.5
             && point.y > -0.5
             && point.z > -0.5
             && point.x < WORLD_SIZE_X -0.5
             && point.y < WORLD_SIZE_Y -0.5
             && point.z < WORLD_SIZE_Z -0.5
              );

}

bool coordenadaCruza(glm::vec3 a,float tA, glm::vec3 b, float tB, int axis){
    if((a[axis] > b[axis])&&(a[axis] - tA < b[axis] + tB)){
        return true;
    }
    if((a[axis] < b[axis])&&(a[axis] + tA > b[axis] - tB)){
        return true;
    }
    if(a[axis]==b[axis]){
        return true;
    }
    return false;
}

bool colisaoCuboCubo(glm::vec3 centroCubo1, float t1, glm::vec3 centroCubo2, float t2){
    for(int axis = 0;axis < 3;axis++){
        if(coordenadaCruza(centroCubo1,t1,centroCubo2,t2,axis) == false){
            return false;
        }
    }
    return true;
}

bool colisaoCuboPlanoOrdinais(glm::vec3 centroCubo, float t, int axis, float coordAxis){
    if((centroCubo[axis] > coordAxis)&&(centroCubo[axis] - t < coordAxis)){
        return true;
    }
    if((centroCubo[axis] < coordAxis)&&(centroCubo[axis] + t > coordAxis)){
        return true;
    }
    if(centroCubo[axis]==coordAxis){
        return true;
    }
    return false;
}

