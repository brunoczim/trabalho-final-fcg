#ifndef SCENE_HPP
#define SCENE_HPP

#include <glad/glad.h>
#include "Camera.hpp"

class SceneObject
{
public:
    const char*  name;        // Nome do objeto
    void*        first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTriangles()
    int          num_indices; // Número de índices do objeto dentro do vetor indices[] definido em BuildTriangles()
    GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)

    GLuint vertex_array_object_id;

    virtual void Build(float diameter = 1.0) = 0;

    void Draw() const;
};

class CubeEdgeSceneObject : public SceneObject
{
public:
    virtual void Build(float diameter = 1.0);
};

class CubeFaceSceneObject : public SceneObject
{
public:
    virtual void Build(float diameter);
};

struct CubeSceneObjects
{
public:
    CubeFaceSceneObject face_object;
    CubeEdgeSceneObject edge_object;

    void Build(float diameter = 1.0);

    void Draw(
        Camera camera,
        GLint model_uniform,
        GLint view_uniform,
        GLint projection_uniform,
        GLint normal_uniform,
        GLint render_as_black_uniform,
        glm::vec4 center = glm::vec4(0.0, 0.0, 0.0, 1.0),
        float scale = 1.0f
    ) const;
};

#endif // SCENE_HPP
