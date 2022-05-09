#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;

// Posição do vértice atual no sistema de coordenadas local do modelo.
in vec4 position_model;

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identificador que define qual objeto está sendo desenhado no momento
#define OBJ_BLOCK 0
#define OBJ_COW 1
uniform int object_id;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Variáveis para acesso das imagens de textura
uniform sampler2D selected_texture;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923
float pi = 3.1415f;
float abertura = pi/6.0f;

void main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(vec4(1.0,1.0,0.0,0.0));

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Coordenadas de textura U e V
    float U = 0.0;
    float V = 0.0;

    vec3 Kd;

    vec4 sentidoLuz = normalize(vec4(0.0,1.0,0.0,0.0));
    float beta = dot(p-l,sentidoLuz);
    float alpha = cos(abertura);

    vec3 Ka;
    vec3 Ks;
    float q;
    vec4 r = -l+2*n*dot(l,n);

    if (object_id == OBJ_BLOCK) {
        // Coordenadas de textura do bloco, obtidas do arquivo OBJ.
        if (abs(position_model.x) == 0.5) {
            U = position_model.z;
            V = position_model.y;
        } else if (abs(position_model.y) == 0.5) {
            U = position_model.z;
            V = position_model.x;
        } else if (abs(position_model.z) == 0.5) {
            U = position_model.x;
            V = position_model.y;
        }

        U += 0.5;
        V += 0.5;

        U = (floor(U * 16.0f) - 0.5) / 16.0f;
        V = (floor(V * 16.0f) - 0.5) / 16.0f;
        Kd = texture(selected_texture, vec2(U,V)).rgb;
        float lambert = max(0,dot(n,l));
        color.rgb = Kd * (lambert + 0.01);



    }
    else if (object_id == OBJ_COW){
        Kd = vec3(1.0,1.0,0.0);
        Ks = vec3(0.8,0.8,0.8);
        Ka = vec3(0.2,0.2,0.2);
        q = 12;

        vec3 I = vec3(1.0,1.0,1.0);
        vec3 lambert_diffuse_term = I*Kd*max(0,dot(l,n));

        vec3 Ia = vec3(0.0,0.0,0.0);
        vec3 ambient_term = Ka*Ia;

        vec3 phong_specular_term  = Ks*I*pow(max(0,dot(v,normalize(r))),q);


        if(beta<alpha){
            lambert_diffuse_term = vec3(0.0,0.0,0.0);
            phong_specular_term = vec3(0.0,0.0,0.0);
        }
        color.rgb = lambert_diffuse_term + ambient_term + phong_specular_term;
    }







//até aqui nao deu erro


    // NOTE: Se você quiser fazer o rendering de objetos transparentes, é
    // necessário:
    // 1) Habilitar a operação de "blending" de OpenGL logo antes de realizar o
    //    desenho dos objetos transparentes, com os comandos abaixo no código C++:
    //      glEnable(GL_BLEND);
    //      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 2) Realizar o desenho de todos objetos transparentes *após* ter desenhado
    //    todos os objetos opacos; e
    // 3) Realizar o desenho de objetos transparentes ordenados de acordo com
    //    suas distâncias para a câmera (desenhando primeiro objetos
    //    transparentes que estão mais longe da câmera).
    // Alpha default = 1 = 100% opaco = 0% transparente
    color.a = 1;

    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
}

