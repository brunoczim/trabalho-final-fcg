#ifndef GPU_HPP
#define GPU_HPP
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id);

#endif // GPU_HPP
