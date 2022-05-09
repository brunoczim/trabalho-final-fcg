
#include "gpu.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional


#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "matrices.hpp"
#include "MatrixStack.hpp"
#include "Camera.hpp"
#include "scene.hpp"
#include "stb_image.h"
#include "blocks.hpp"
#include "collisions.hpp"

#define OBJ_BLOCK 0
#define OBJ_COW 1
#define OBJ_EYE 2

#define INVENTORY_MAX 64

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

// Declaração de funções auxiliares para renderizar texto dentro da janela
// OpenGL. Estas funções estão definidas no arquivo "textrendering.cpp".
void TextRendering_Init();
float TextRendering_LineHeight(GLFWwindow* window);
float TextRendering_CharWidth(GLFWwindow* window);
void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrix(GLFWwindow* window, glm::mat4 M, float x, float y, float scale = 1.0f);
void TextRendering_PrintVector(GLFWwindow* window, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProduct(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductMoreDigits(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductDivW(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);

void TextRendering_ShowFramesPerSecond(GLFWwindow* window);
void TextRendering_ShowCameraPosition(GLFWwindow* window);
void TextRendering_ShowInventory(GLFWwindow* window);

void LoadShader(const char* filename, GLuint shader_id);
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
GLFWwindow *CreateGLFWWindow(void);
void SetupInputCallbacks(GLFWwindow *window);
void PrintGlInfo();
void SetupFramebufferSize(GLFWwindow *window);

void CorrectCursorPos(GLFWwindow *window, int *out_window_center_x = NULL, int *out_window_center_y = NULL);

GLuint LoadTextureImage(char const *path, char const *name, GLuint program_id);

// Variável que controla se o texto informativo será mostrado na tela.
bool g_ShowInfoText = true;

Camera g_Camera;
MatrixStack g_MatrixStack;
WorldBlockMatrix g_WorldBlockMatrix;

unsigned char g_StonesInInventory = 0;

int main(int argc, char const *argv[])
{
    int success = glfwInit();
    if (!success)
    {
        std::cerr << "ERROR: glfwInit() failed." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impressão de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    GLFWwindow *window = CreateGLFWWindow();
    if (!window)
    {
        glfwTerminate();
        std::cerr << "ERROR: glfwCreateWindow() failed." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    SetupInputCallbacks(window);

    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    SetupFramebufferSize(window);

    PrintGlInfo();

    CorrectCursorPos(window);

    // Carregamos os shaders de vértices e de fragmentos que serão utilizados
    // para renderização. Veja slides 176-196 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
    //
    // Note que o caminho para os arquivos "shader_vertex.glsl" e
    // "shader_fragment.glsl" estão fixados, sendo que assumimos a existência
    // da seguinte estrutura no sistema de arquivos:
    //
    //    /
    //    |
    //    +--+ bin/
    //    |  |
    //    |  +--+ Release/  (ou Debug/ ou Linux/)
    //    |     |
    //    |     o-- main.exe
    //    |
    //    +--+ src/
    //       |
    //       o-- shader_vertex.glsl
    //       |
    //       o-- shader_fragment.glsl
    //       |
    //       o-- ...
    //
    GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

    // Criamos um programa de GPU utilizando os shaders carregados acima
    GLuint program_id = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

    GLuint stone_texture_id = LoadTextureImage("../../data/stone.png", "stone_texture_image", program_id);
    GLuint cow_texture_id = LoadTextureImage("../../data/cow_texture.jpg", "cow_texture_image", program_id);
    GLuint eye_texture_id = LoadTextureImage("../../data/eye.jpg", "eye_texture_image", program_id);

    // Buscamos o endereço das variáveis definidas dentro do Vertex Shader.
    // Utilizaremos estas variáveis para enviar dados para a placa de vídeo
    // (GPU)! Veja arquivo "shader_vertex.glsl".
    GLint model_uniform             = glGetUniformLocation(program_id, "model"); // Variável da matriz "model"
    GLint view_uniform              = glGetUniformLocation(program_id, "view"); // Variável da matriz "view" em shader_vertex.glsl
    GLint projection_uniform        = glGetUniformLocation(program_id, "projection"); // Variável da matriz "projection" em shader_vertex.glsl
    GLint object_id_uniform         = glGetUniformLocation(program_id, "object_id");
    GLint selected_texture_uniform  = glGetUniformLocation(program_id, "selected_texture");
    GLint bbox_min_uniform          = glGetUniformLocation(program_id, "bbox_min");
    GLint bbox_max_uniform          = glGetUniformLocation(program_id, "bbox_max");


    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.2
    glEnable(GL_DEPTH_TEST);

    VirtualScene virtual_scene;

    TextRendering_Init();


    while (!glfwWindowShouldClose(window)) {
        // Aqui executamos as operações de renderização

        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor é
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto é:
        // Vermelho, Verde, Azul, Alpha (valor de transparência).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
        //
        //           R     G     B     A
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
        // e também resetamos todos os pixels do Z-buffer (depth buffer).
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de vértice e fragmentos).
        glUseProgram(program_id);

        glm::mat4 model = Matrix_Identity();
        glm::mat4 view = g_Camera.ViewMatrix();
        glm::mat4 projection = g_Camera.ProjectionMatrix();

        glUniformMatrix4fv(view_uniform, 1, GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(projection_uniform, 1, GL_FALSE , glm::value_ptr(projection));

        //Renderiza os blocos do chao
        glUniform1i(object_id_uniform, OBJ_BLOCK);

        for (size_t x = 0; x < WORLD_SIZE_X; x++) {
            for (size_t y = 0; y < WORLD_SIZE_Y; y++) {
                for (size_t z = 0; z < WORLD_SIZE_Z; z++) {
                    if (g_WorldBlockMatrix[WorldPoint(x,y,z)] == BLOCK_STONE) {
                        glUniform1i(selected_texture_uniform, stone_texture_id);
                        model = Matrix_Translate(x,y,z);
                        glUniformMatrix4fv(model_uniform, 1, GL_FALSE , glm::value_ptr(model));
                        virtual_scene["block"].Draw(bbox_min_uniform, bbox_max_uniform);
                    }
                }
            }
        }
        //Desenhar vaca
        glUniform1i(object_id_uniform, OBJ_COW);
        glUniform1i(selected_texture_uniform, cow_texture_id);
        model = Matrix_Translate(0,17,0);
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE , glm::value_ptr(model));
        virtual_scene["cow"].Draw(bbox_min_uniform, bbox_max_uniform);

        /*
        glUniform1i(object_id_uniform, OBJ_EYE);
        glUniform1i(selected_texture_uniform, eye_texture_id);
        model = Matrix_Translate(0,17,0);
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE , glm::value_ptr(model));
        virtual_scene["eye"].Draw(bbox_min_uniform, bbox_max_uniform);
        */

        TextRendering_ShowFramesPerSecond(window);
        TextRendering_ShowCameraPosition(window);
        TextRendering_ShowInventory(window);

        // O framebuffer onde OpenGL executa as operações de renderização não
        // é o mesmo que está sendo mostrado para o usuário, caso contrário
        // seria possível ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuário
        // tudo que foi renderizado pelas funções acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma interação do
        // usuário (teclado, mouse, ...). Caso positivo, as funções de callback
        // definidas anteriormente usando glfwSet*Callback() serão chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();

    }

    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();
    return 0;
}

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    std::cerr << "ERROR: GLFW (" << error << "): " << description << std::endl;
}

// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        CollisionFace output;

        if (FacingNonAirBlock(output,g_Camera,g_WorldBlockMatrix)) {
            glm::vec4 point(round(output.block_position.x),round(output.block_position.y),round(output.block_position.z),0.0f);
            PrintVector(point);
            std::cout<<output.axis<<std::endl;
            std::cout<<output.sign<<std::endl;
            g_WorldBlockMatrix[WorldPoint(output.block_position)] = BLOCK_AIR;
            if (g_StonesInInventory < INVENTORY_MAX) {
                g_StonesInInventory++;
            }
        }
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        CollisionFace output;

        if (FacingNonAirBlock(output,g_Camera,g_WorldBlockMatrix)) {
            glm::vec4 point(round(output.block_position.x),round(output.block_position.y),round(output.block_position.z),0.0f);
            PrintVector(point);
            glm::vec3 position = output.block_position;
            position[output.axis] -= output.sign;
            g_WorldBlockMatrix[WorldPoint(position)] = BLOCK_STONE;
            g_StonesInInventory--;
        }

    }
}

// Função callback chamada sempre que o usuário movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    int window_center_x;
    int window_center_y;

    CorrectCursorPos(window, &window_center_x, &window_center_y);

    // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
    float dx = xpos - window_center_x;
    float dy = ypos - window_center_y;

    g_Camera.RotateViewTheta(dx);
    g_Camera.RotateViewPhi(dy);
}

void CorrectCursorPos(GLFWwindow *window, int *out_window_center_x, int *out_window_center_y)
{
    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);

    int window_center_x = window_width / 2;
    int window_center_y = window_height / 2;

    glfwSetCursorPos(window, window_center_x, window_center_y);

    if (out_window_center_x != NULL) {
        *out_window_center_x = window_center_x;
    }
    if (out_window_center_y != NULL) {
        *out_window_center_y = window_center_y;
    }
}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    g_Camera.Zoom(yoffset);
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
        // ===============
    // Não modifique este loop! Ele é utilizando para correção automatizada dos
    // laboratórios. Deve ser sempre o primeiro comando desta função KeyCallback().
    for (int i = 0; i < 10; ++i)
        if (key == GLFW_KEY_0 + i && action == GLFW_PRESS && mod == GLFW_MOD_SHIFT)
            std::exit(100 + i);
    // ===============

    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // O código abaixo implementa a seguinte lógica:
    //   Se apertar tecla X       então g_AngleX += delta;
    //   Se apertar tecla shift+X então g_AngleX -= delta;
    //   Se apertar tecla Y       então g_AngleY += delta;
    //   Se apertar tecla shift+Y então g_AngleY -= delta;
    //   Se apertar tecla Z       então g_AngleZ += delta;
    //   Se apertar tecla shift+Z então g_AngleZ -= delta;

    /******* Movimento da Câmera Para Frente/Trás/Lados *******/
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_W) {
            g_Camera.MoveForewards();
        } else if (key == GLFW_KEY_S) {
            g_Camera.MoveBackwards();
        } else if (key == GLFW_KEY_A) {
            g_Camera.MoveLeftwards();
        } else if (key == GLFW_KEY_D) {
            g_Camera.MoveRightwards();
        }else if (key == GLFW_KEY_SPACE) {
            g_Camera.MoveUpwards();
        }else if (key == GLFW_KEY_F) {
            g_Camera.MoveDownwards();
        }
    }

    //Se o usuário apertar a tecla P, utilizamos projeção perspectiva.
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        g_Camera.SetProjectionType(Camera::PERSPECTIVE_PROJ);
    }

    // Se o usuário apertar a tecla O, utilizamos projeção ortográfica.
    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        g_Camera.SetProjectionType(Camera::ORTHOGRAPHIC_PROJ);
    }

    // Se o usuário apertar a tecla H, fazemos um "toggle" do texto informativo mostrado na tela.
    if (key == GLFW_KEY_H && action == GLFW_PRESS) {
        g_ShowInfoText = !g_ShowInfoText;
    }
}

// Definição da função que será chamada sempre que a janela do sistema
// operacional for redimensionada, por consequência alterando o tamanho do
// "framebuffer" (região de memória onde são armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda região do framebuffer. A
    // função "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula ({+ViewportMapping2+}).
    glViewport(0, 0, width, height);

    // Atualizamos também a razão que define a proporção da janela (largura /
    // altura), a qual será utilizada na definição das matrizes de projeção,
    // tal que não ocorra distorções durante o processo de "Screen Mapping"
    // acima, quando NDC é mapeado para coordenadas de pixels. Veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    //
    // O cast para float é necessário pois números inteiros são arredondados ao
    // serem divididos!
    g_Camera.OnScreenResize(width, height);
}

// Carrega um Vertex Shader de um arquivo. Veja definição de LoadShader() abaixo.
GLuint LoadShader_Vertex(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos vértices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, vertex_shader_id);

    // Retorna o ID gerado acima
    return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo. Veja definição de LoadShader() abaixo.
GLuint LoadShader_Fragment(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos fragmentos.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, fragment_shader_id);

    // Retorna o ID gerado acima
    return fragment_shader_id;
}

// Função auxilar, utilizada pelas duas funções acima. Carrega código de GPU de
// um arquivo GLSL e faz sua compilação.
void LoadShader(const char* filename, GLuint shader_id)
{
    // Lemos o arquivo de texto indicado pela variável "filename"
    // e colocamos seu conteúdo em memória, apontado pela variável
    // "shader_string".
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch ( std::exception& e ) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>( str.length() );

    // Define o código do shader GLSL, contido na string "shader_string"
    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

    // Compila o código do shader GLSL (em tempo de execução)
    glCompileShader(shader_id);

    // Verificamos se ocorreu algum erro ou "warning" durante a compilação
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    // Alocamos memória para guardar o log de compilação.
    // A chamada "new" em C++ é equivalente ao "malloc()" do C.
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    // Imprime no terminal qualquer erro ou "warning" de compilação
    if (log_length != 0) {
        std::string  output;

        if (!compiled_ok) {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        std::cerr << output;
    }

    // A chamada "delete" em C++ é equivalente ao "free()" do C
    delete [] log;
}

GLFWwindow *CreateGLFWWindow(void)
{
        // Pedimos para utilizar OpenGL versão 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Pedimos para utilizar o perfil "core", isto é, utilizaremos somente as
    // funções modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional, com 800 colunas e 600 linhas
    // de pixels, e com título "INF01047 ...".
    GLFWwindow* window;
    int window_width = 800;
    int window_height = 600;
    window = glfwCreateWindow(window_width, window_height, "INF01047 - 313985 e 314256 - Bruno C Zimmermann e João G Zandoná", NULL, NULL);
    return window;
}

void SetupInputCallbacks(GLFWwindow *window)
{
    // Definimos a função de callback que será chamada sempre que o usuário
    // pressionar alguma tecla do teclado ...
    glfwSetKeyCallback(window, KeyCallback);
    // ... ou clicar os botões do mouse ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... ou movimentar o cursor do mouse em cima da janela ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    // ... ou rolar a "rodinha" do mouse.
    glfwSetScrollCallback(window, ScrollCallback);
}

void PrintGlInfo()
{
    // Imprimimos no terminal informações sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    std::cout   << "GPU: "
                << vendor
                << ", "
                << renderer
                << ", OpenGL: "
                << glversion
                << ", GLSL: "
                << glslversion
                << std::endl;
}

void SetupFramebufferSize(GLFWwindow *window)
{
    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, 800, 600); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.
}

// Função que carrega uma imagem para ser utilizada como textura
GLuint LoadTextureImage(char const *path, char const *name, GLuint program_id)
{
    static GLuint loaded_textures = 0;

    std::cout << "Carregando imagem \"" << path << "\"..." << std::endl;

    // Primeiro fazemos a leitura da imagem do disco
    stbi_set_flip_vertically_on_load(true);
    int width;
    int height;
    int channels;
    unsigned char *data = stbi_load(path, &width, &height, &channels, 3);

    if (data == NULL) {
        std::cerr << "ERROR: Cannot open image file \"" << path << "\"." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cout << "Ok (" << width << "x" << height << ")" << std::endl;

    // Agora criamos objetos na GPU com OpenGL para armazenar a textura
    GLuint texture_id;
    GLuint sampler_id;
    glGenTextures(1, &texture_id);
    glGenSamplers(1, &sampler_id);

    // Veja slides 95-96 do documento Aula_20_Mapeamento_de_Texturas.pdf
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Parâmetros de amostragem da textura.
    glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Agora enviamos a imagem lida do disco para a GPU
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    GLuint textureunit = loaded_textures;
    glActiveTexture(GL_TEXTURE0 + textureunit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindSampler(textureunit, sampler_id);

    stbi_image_free(data);

    loaded_textures += 1;

    glUseProgram(program_id);
    glUniform1i(glGetUniformLocation(program_id, name), textureunit);
    glUseProgram(0);

    return textureunit;
}

// Escrevemos na tela o número de quadros renderizados por segundo (frames per
// second).
void TextRendering_ShowFramesPerSecond(GLFWwindow* window)
{
    if (!g_ShowInfoText)
        return;

    // Variáveis estáticas (static) mantém seus valores entre chamadas
    // subsequentes da função!
    static float old_seconds = (float)glfwGetTime();
    static int   ellapsed_frames = 0;
    static char  buffer[20] = "?? fps";
    static int   numchars = 7;

    ellapsed_frames += 1;

    // Recuperamos o número de segundos que passou desde a execução do programa
    float seconds = (float)glfwGetTime();

    // Número de segundos desde o último cálculo do fps
    float ellapsed_seconds = seconds - old_seconds;

    if ( ellapsed_seconds > 1.0f )
    {
        numchars = snprintf(buffer, 20, "%.2f fps", ellapsed_frames / ellapsed_seconds);

        old_seconds = seconds;
        ellapsed_frames = 0;
    }

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    TextRendering_PrintString(window, buffer, 1.0f-(numchars + 1)*charwidth, 1.0f-lineheight, 1.0f);
}

void TextRendering_ShowCameraPosition(GLFWwindow* window)
{
     if (!g_ShowInfoText)
        return;

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    char buffer[17*3] = {0};

    size_t numchars = snprintf(
             buffer,
             17*3,
             "X=%.2f  Y=%.2f  Z=%.2f",
             g_Camera.CenterPoint().x,
             g_Camera.CenterPoint().y,
             g_Camera.CenterPoint().z
    );

    TextRendering_PrintString(window, buffer, 1.0f-(numchars + 1)*charwidth, 1.0 - lineheight * 1.25 * 2, 1.0f);
}

void TextRendering_ShowInventory(GLFWwindow* window)
{
    if (!g_ShowInfoText)
        return;

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    char buffer[20] = "INVENTORY";
    size_t numchars = strlen(buffer);

    TextRendering_PrintString(window, buffer, 1.0f-(numchars + 1)*charwidth, 1.0 - lineheight * 1.25 * 4, 1.0f);

    numchars = snprintf(buffer, 20, "STONES: %u", g_StonesInInventory);
    TextRendering_PrintString(window, buffer, 1.0f-(numchars + 1)*charwidth, 1.0 - lineheight * 1.25 * 5, 1.0f);
}
