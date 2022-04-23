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

        void rotateViewTheta(int dx);
    void rotateViewPhi(int dy);

    void moveForewards();
    void moveBackwards();
    void moveLeftwards();
    void moveRightwards();

    void onScreenResize(int width, int height);
    void setProjectionType(ProjectionType projectionType);
    void move(float offset);

    glm::mat4 viewMatrix() const;
    glm::mat4 projectionMatrix() const;

private:
    ProjectionType projectionType;
    float distance;
    glm::vec4 centerPoint;
    float viewTheta;
    float viewPhi;
    float viewRho;
    glm::vec4 upVector;
    float moveSpeed;
    float rotateSpeed;
    float nearplane;
    float farplane;
    float fieldOfView;
    float screenRatio;

    glm::vec4 viewVector() const;
    glm::vec4 uVector() const;
    glm::vec4 wVector() const;
};

#endif // CAMERA_HPP
