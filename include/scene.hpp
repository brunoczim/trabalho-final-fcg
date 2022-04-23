#ifndef SCENE_HPP
#define SCENE_HPP

#include <glad/glad.h>

struct SceneObject
{
    const char*  name;        // Nome do objeto
    void*        first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTriangles()
    int          num_indices; // Número de índices do objeto dentro do vetor indices[] definido em BuildTriangles()
    GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)

    void Draw() const;
};

struct CubeSceneObject
{
public:
    SceneObject faces;
    SceneObject edges;

    GLuint Build();

    void Draw(GLint render_as_black_uniform) const;
};

struct VirtualScene
{
    CubeSceneObject cube;
};

#endif // SCENE_HPP
