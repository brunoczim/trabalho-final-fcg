#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class Camera
{
public:
    enum ProjectionType {
        PERSPECTIVE_PROJ,
        ORTHOGRAPHIC_PROJ
    };

    Camera();

    void rotate_view_theta(int dx);
    void rotate_view_phi(int dy);

    void move_forewards();
    void move_backwards();
    void move_leftwards();
    void move_rightwards();

    void on_screen_resize(int width, int height);
    void set_projection_type(ProjectionType projection_type);
    void zoom(float factor);

    glm::mat4 view_matrix() const;
    glm::mat4 projection_matrix() const;

private:
    ProjectionType projection_type;
    float distance;
    glm::vec4 center_point;
    float view_theta;
    float view_phi;
    float view_rho;
    glm::vec4 up_vector;
    float move_speed;
    float rotate_speed;
    float zoom_speed;
    float nearplane;
    float farplane;
    float field_of_view;
    float screen_ratio;

    glm::vec4 view_vector() const;
    glm::vec4 u_vector() const;
    glm::vec4 w_vector() const;
};

#endif // CAMERA_HPP
