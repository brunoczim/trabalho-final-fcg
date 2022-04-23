#include "Camera.hpp"
#include "matrices.hpp"

Camera::Camera():
    projection_type(PERSPECTIVE_PROJ),
    distance(2.5f),
    center_point(-4.0f, 0.0f, -1.5f, 1.0f),
    view_theta(1.2f),
    view_phi(0.0f),
    view_rho(1.0f),
    up_vector(0.0f, 1.0f, 0.0f, 0.0f),
    move_speed(0.05f),
    rotate_speed(0.01f),
    zoom_speed(0.1f),
    nearplane(-0.1f),
    farplane(-10.0f),
    field_of_view(acos(-1) / 3.0f),
    screen_ratio(1.0f)
{
}

void Camera::rotate_view_theta(int dx)
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

void Camera::rotate_view_phi(int dy)
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

void Camera::move_forewards()
{
    this->center_point += -this->w_vector() * this->move_speed;
}

void Camera::move_backwards()
{
    this->center_point += this->w_vector() * this->move_speed;
}

void Camera::move_leftwards()
{
    this->center_point += -this->u_vector() * this->move_speed;
}

void Camera::move_rightwards()
{
    this->center_point += this->u_vector() * this->move_speed;
}

void Camera::set_projection_type(ProjectionType projection_type)
{
    this->projection_type = projection_type;
}

void Camera::zoom(float factor)
{
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    this->distance -= this->zoom_speed * factor;

    // Uma câmera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela está olhando, pois isto gera problemas de divisão por zero na
    // definição do sistema de coordenadas da câmera. Isto é, a variável abaixo
    // nunca pode ser zero. Versões anteriores deste código possuíam este bug,
    // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (this->distance < verysmallnumber)
        this->distance = verysmallnumber;
}

glm::vec4 Camera::view_vector() const
{
    return glm::vec4(
        this->view_rho * sin(this->view_theta) * cos(this->view_phi),
        this->view_rho * sin(this->view_theta) * sin(this->view_phi),
        this->view_rho * cos(this->view_theta),
        0.0f
    );
}

glm::vec4 Camera::u_vector() const
{
    glm::vec4 vector = crossproduct(this->up_vector, this->w_vector());
    return vector* (1.0f / norm(vector));
}

glm::vec4 Camera::w_vector() const
{
    glm::vec4 vector = -this->view_vector();
    return vector * (1.0f / norm(vector));
}

void Camera::on_screen_resize(int width, int height)
{
    this->screen_ratio = (float) width / (float) height;
}

glm::mat4 Camera::view_matrix() const
{
    return Matrix_Camera_View(this->center_point, this->view_vector(), this->up_vector);
}

glm::mat4 Camera::projection_matrix() const
{
    float t = 1.5f * this->distance/2.5f;
    float b = -t;
    float r = t * this->screen_ratio;
    float l = -r;

    switch (this->projection_type) {
    case PERSPECTIVE_PROJ:
        return Matrix_Perspective(this->field_of_view, this->screen_ratio, this->nearplane, this->farplane);
    case ORTHOGRAPHIC_PROJ:
        return Matrix_Orthographic(l, r, b, t, this->nearplane, this->farplane);
    default:
        throw "Invalid projection _type";
    }
}
