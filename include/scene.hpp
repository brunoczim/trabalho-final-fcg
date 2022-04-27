#ifndef SCENE_HPP
#define SCENE_HPP

#include <glad/glad.h>
#include <tiny_obj_loader.h>
#include <Camera.hpp>

class SceneObject
{
public:
    std::string  name;        // Nome do objeto
    size_t       first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    size_t       num_indices; // Número de índices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
    GLuint       vertex_array_object_id; // ID do VAO onde estão armazenados os atributos do modelo
    glm::vec3    bbox_min; // Axis-Aligned Bounding Box do objeto
    glm::vec3    bbox_max;

    void Draw(GLint bbox_min_uniform, GLint bbox_max_uniform) const;
};

struct VirtualScene {
private:
    std::map<std::string, SceneObject> objects;
public:
    VirtualScene();

    void insert(SceneObject new_scene_object);

    SceneObject const &operator [] (char const *name) const;
};

struct ObjModel
{
private:
    tinyobj::attrib_t                 attrib;
    std::vector<tinyobj::shape_t>     shapes;
    std::vector<tinyobj::material_t>  materials;

public:
    // Este construtor lê o modelo de um arquivo utilizando a biblioteca tinyobjloader.
    // Veja: https://github.com/syoyo/tinyobjloader
    ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true);

    void ComputeNormals();

    void BuildTriangles(VirtualScene &target_virtual_scene) const;

    static void NewIntoVirtualScene(
        VirtualScene &target_virtual_scene,
        const char* filename,
        const char* basepath = NULL,
        bool triangulate = true
    );
};

#endif // SCENE_HPP
