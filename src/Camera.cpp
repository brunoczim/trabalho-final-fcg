#include "Camera.hpp"
#include "matrices.hpp"
#include "blocks.hpp"
#include <stdexcept>

float pi = acos(-1.0f);
Camera::Camera():
    projection_type(PERSPECTIVE_PROJ),
    center_point(4.0f, WORLD_SIZE_Y / 2.0f + 1.0f, 4.0f, 1.0f),
    view_theta(0.0f),
    view_phi(0.0f),
    view_rho(2.5f),
    up_vector(0.0f, 1.0f, 0.0f, 0.0f),
    move_speed(0.10001f),
    rotate_speed(0.005f),
    zoom_speed(0.1f),
    nearplane(-0.1f),
    farplane(-10.0f),
    screen_ratio(1.0f),
    field_of_view(pi/2),
    tamanho(0.5f)
{
}

void Camera::RotateViewTheta(float dx)
{
    this->view_theta -= this->rotate_speed * dx;
}

void Camera::RotateViewPhi(float dy)
{

    this->view_phi += this->rotate_speed * dy;

    float phimax = 3.141592f/2;
    float phimin = -phimax;

    if (view_phi > phimax)
        this->view_phi = phimax;

    if (view_phi < phimin)
        this->view_phi = phimin;
}

void Camera::MoveForewards()
{
    this->center_point += -this->WVectorProjectedToXZ() * this->move_speed;
}

void Camera::MoveBackwards()
{
    this->center_point += this->WVectorProjectedToXZ() * this->move_speed;
}


void Camera::MoveLeftwards()
{
    this->center_point += -this->UVector() * glm::vec4(1.0f, 0.0f, 1.0f, 1.0f) * this->move_speed;
}

void Camera::MoveRightwards()
{
    this->center_point += this->UVector() * glm::vec4(1.0f, 0.0f, 1.0f, 1.0f) * this->move_speed;
}

void Camera::MoveUpwards()
{
    this->center_point += glm::vec4(0,1,0,0) * this->move_speed;
}

void Camera::MoveDownwards()
{
    this->center_point -= glm::vec4(0,1,0,0) * this->move_speed;
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
    this->field_of_view -= this->zoom_speed * factor;

    float field_of_view_max = pi / 2;
    float field_of_view_min = pi / 8;

    if (this->field_of_view < field_of_view_min) {
        this->field_of_view = field_of_view_min;
    }

    if (this->field_of_view > field_of_view_max) {
        this->field_of_view = field_of_view_max;
    }

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

glm::vec4 Camera::CenterPoint() const
{
    return this->center_point;
}

glm::vec4 Camera::ViewVector() const
{
        float yViewVec = -sin(this->view_phi);
        float zViewVec = cos(this->view_phi)*cos(this->view_theta);
        float xViewVec = cos(this->view_phi)*sin(this->view_theta);
        return glm::vec4(xViewVec,yViewVec,zViewVec,0.0f);
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

glm::vec4 Camera::WVectorProjectedToXZ() const
{
    glm::vec4 w_vector = this->WVector();
    w_vector.y = 0.0f;
    return w_vector / norm(w_vector);
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
    float t = 1.5f*this->view_rho/2.5f;
    float b = -t;
    float r = t*this->screen_ratio;
    float l = -r;

    switch (this->projection_type) {
    case PERSPECTIVE_PROJ:
        return Matrix_Perspective(this->field_of_view, this->screen_ratio, this->nearplane, this->farplane);
    case ORTHOGRAPHIC_PROJ:
        return Matrix_Orthographic(l, r, b, t, this->nearplane, this->farplane);
    default:
        throw std::runtime_error("Invalid projection _type");
    }
}
