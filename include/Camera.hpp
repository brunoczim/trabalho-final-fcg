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

    void RotateViewTheta(float dx);
    void RotateViewPhi(float dy);

    void MoveForewards();
    void MoveBackwards();
    void MoveLeftwards();
    void MoveRightwards();

    void OnScreenResize(int width, int height);
    void SetProjectionType(ProjectionType projection_type);
    void Zoom(float factor);

    glm::vec4 CenterPoint() const;
    glm::vec4 ViewVector() const;
    glm::mat4 ViewMatrix() const;
    glm::mat4 ProjectionMatrix() const;

private:
    ProjectionType projection_type;
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
    float screen_ratio;
    float field_of_view;

    glm::vec4 UVector() const;
    glm::vec4 WVector() const;
    glm::vec4 WVectorProjectedToXZ() const;
};

#endif // CAMERA_HPP
