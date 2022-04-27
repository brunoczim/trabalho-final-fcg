#include "Camera.hpp"
#include "matrices.hpp"
#include <stdexcept>

Camera::Camera():
    projection_type(PERSPECTIVE_PROJ),
    center_point(-4.0f, 0.0f, -1.5f, 1.0f),
    view_theta(1.2f),
    view_phi(0.0f),
    view_rho(2.5f),
    up_vector(0.0f, 1.0f, 0.0f, 0.0f),
    move_speed(0.05f),
    rotate_speed(0.01f),
    zoom_speed(0.1f),
    nearplane(-0.1f),
    farplane(-10.0f),
    field_of_view_ratio(50.0f),
    screen_ratio(1.0f)
{
}

void Camera::RotateViewTheta(int dx)
{
    this->view_theta -= this->rotate_speed * dx;

    float pi = acos(-1.0f);

    while (this->view_theta > 2.0f * pi) {
        this->view_theta -= 2.0f * pi;
    }
    while (this->view_theta < -2.0f * pi) {
        this->view_theta += 2.0f * pi;
    }
}

void Camera::RotateViewPhi(int dy)
{
    this->view_phi -= this->rotate_speed * dy;

    float pi = acos(-1.0f);

    while (this->view_phi > 2.0f * pi) {
        this->view_phi -= 2.0f * pi;
    }
    while (this->view_phi < -2.0f * pi) {
        this->view_phi += 2.0f * pi;
    }
}

#include <iostream>

void Camera::MoveForewards()
{
    this->center_point += -this->WVector() * this->move_speed;
    std::cout << "Camera position: " << std::endl;
    PrintVector(this->center_point);
}

void Camera::MoveBackwards()
{
    this->center_point += this->WVector() * this->move_speed;
    std::cout << "Camera position: " << std::endl;
    PrintVector(this->center_point);
}

void Camera::MoveLeftwards()
{
    this->center_point += -this->UVector() * this->move_speed;
    std::cout << "Camera position: " << std::endl;
    PrintVector(this->center_point);
}

void Camera::MoveRightwards()
{
    this->center_point += this->UVector() * this->move_speed;
    std::cout << "Camera position: " << std::endl;
    PrintVector(this->center_point);
}

void Camera::SetProjectionType(ProjectionType projection_type)
{
    this->projection_type = projection_type;
}

void Camera::Zoom(float factor)
{
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    this->view_rho -= this->zoom_speed * factor;

    // Uma câmera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela está olhando, pois isto gera problemas de divisão por zero na
    // definição do sistema de coordenadas da câmera. Isto é, a variável abaixo
    // nunca pode ser zero. Versões anteriores deste código possuíam este bug,
    // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (this->view_rho < verysmallnumber) {
        this->view_rho = verysmallnumber;
    }
}

glm::vec4 Camera::ViewVector() const
{
    return glm::vec4(
        this->view_rho * sin(this->view_theta) * cos(this->view_phi),
        this->view_rho * sin(this->view_theta) * sin(this->view_phi),
        this->view_rho * cos(this->view_theta),
        0.0f
    );
}

glm::vec4 Camera::UVector() const
{
    glm::vec4 vector = crossproduct(this->up_vector, this->WVector());
    return vector * (1.0f / norm(vector));
}

glm::vec4 Camera::WVector() const
{
    glm::vec4 vector = -this->ViewVector();
    return vector * (1.0f / norm(vector));
}

void Camera::OnScreenResize(int width, int height)
{
    this->screen_ratio = (float) width / (float) height;
}

glm::mat4 Camera::ViewMatrix() const
{
    return Matrix_Camera_View(this->center_point, this->ViewVector(), this->up_vector);
}

glm::mat4 Camera::ProjectionMatrix() const
{
    float t = 1.5f * this->view_rho / 2.5f;
    float b = -t;
    float r = t * this->screen_ratio;
    float l = -r;
    float field_of_view = atanf(this->view_rho / this->field_of_view_ratio / fabs(this->nearplane))  * 2.0f;

    switch (this->projection_type) {
    case PERSPECTIVE_PROJ:
        return Matrix_Perspective(field_of_view, this->screen_ratio, this->nearplane, this->farplane);
    case ORTHOGRAPHIC_PROJ:
        return Matrix_Orthographic(l, r, b, t, this->nearplane, this->farplane);
    default:
        throw std::runtime_error("Invalid projection _type");
    }
}
