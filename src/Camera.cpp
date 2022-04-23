#include "Camera.hpp"
#include "matrices.hpp"

Camera::Camera():
    projectionType(PERSPECTIVE_PROJ),
    distance(2.5f),
    centerPoint(-4.0f, 0.0f, -1.5f, 1.0f),
    viewTheta(1.2f),
    viewPhi(0.0f),
    viewRho(1.0f),
    upVector(0.0f, 1.0f, 0.0f, 0.0f),
    moveSpeed(0.05f),
    rotateSpeed(0.01f),
    nearplane(-0.1f),
    farplane(-10.0f),
    fieldOfView(acos(-1) / 3.0f),
    screenRatio(1.0f)
{
}

void Camera::rotateViewTheta(int dx)
{
    this->viewTheta -= this->rotateSpeed * dx;

    float pi = acos(-1.0f);

    while (this->viewTheta > 2.0f * pi) {
        this->viewTheta -= 2.0f * pi;
    }
    while (this->viewTheta < -2.0f * pi) {
        this->viewTheta += 2.0f * pi;
    }
}

void Camera::rotateViewPhi(int dy)
{
    this->viewPhi -= this->rotateSpeed * dy;

    float pi = acos(-1.0f);

    while (this->viewPhi > 2.0f * pi) {
        this->viewPhi -= 2.0f * pi;
    }
    while (this->viewPhi < -2.0f * pi) {
        this->viewPhi += 2.0f * pi;
    }
}

void Camera::moveForewards()
{
    this->centerPoint += -this->wVector() * this->moveSpeed;
}

void Camera::moveBackwards()
{
    this->centerPoint += this->wVector() * this->moveSpeed;
}

void Camera::moveLeftwards()
{
    this->centerPoint += -this->uVector() * this->moveSpeed;
}

void Camera::moveRightwards()
{
    this->centerPoint += this->uVector() * this->moveSpeed;
}

void Camera::setProjectionType(ProjectionType projectionType)
{
    this->projectionType = projectionType;
}

void Camera::move(float offset)
{
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    this->distance -= 0.1f * offset;

    // Uma câmera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela está olhando, pois isto gera problemas de divisão por zero na
    // definição do sistema de coordenadas da câmera. Isto é, a variável abaixo
    // nunca pode ser zero. Versões anteriores deste código possuíam este bug,
    // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (this->distance < verysmallnumber)
        this->distance = verysmallnumber;
}

glm::vec4 Camera::viewVector() const
{
    return glm::vec4(
        this->viewRho * sin(this->viewTheta) * cos(this->viewPhi),
        this->viewRho * sin(this->viewTheta) * sin(this->viewPhi),
        this->viewRho * cos(this->viewTheta),
        0.0f
    );
}

glm::vec4 Camera::uVector() const
{
    glm::vec4 vector = crossproduct(this->upVector, this->wVector());
    return vector* (1.0f / norm(vector));
}

glm::vec4 Camera::wVector() const
{
    glm::vec4 vector = -this->viewVector();
    return vector * (1.0f / norm(vector));
}

void Camera::onScreenResize(int width, int height)
{
    this->screenRatio = (float) width / (float) height;
}

glm::mat4 Camera::viewMatrix() const
{
    return Matrix_Camera_View(this->centerPoint, this->viewVector(), this->upVector);
}

glm::mat4 Camera::projectionMatrix() const
{
    float t = 1.5f * this->distance/2.5f;
    float b = -t;
    float r = t * this->screenRatio;
    float l = -r;

    switch (this->projectionType) {
    case PERSPECTIVE_PROJ:
        return Matrix_Perspective(this->fieldOfView, this->screenRatio, this->nearplane, this->farplane);
    case ORTHOGRAPHIC_PROJ:
        return Matrix_Orthographic(l, r, b, t, this->nearplane, this->farplane);
    default:
        throw "Invalid projection type";
    }
}
