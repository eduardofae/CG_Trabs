//////////////////////////////////////////////////////////////////////////////
//
//  Main.cpp
//
//////////////////////////////////////////////////////////////////////////////

#include <GL3/gl3.h>
#include <GL3/gl3w.h>

#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <vector>

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "./utils/matrices.hpp"
#include "./utils/printMatrices.hpp"
#include "./render/render.hpp"
#include "./render/cube.hpp"

enum VAO_IDs    { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, colorBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0, vColor = 1 };
enum types      { point = 0, line = 1, triangle = 2 };

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

float  g_ScreenRatio = 1.0f;
float  g_CameraDistance = 1.0f;
bool   g_UsePerspectiveProjection = true;
bool   g_lookAt = true;
int    g_chosenType = triangle;
double g_LastCursorPosX, g_LastCursorPosY;

typedef struct PressedKeys{
		bool w, a, s, d, space, shift;
} PressedKeys;

PressedKeys g_keys{false, false, false, false, false, false};

// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

glm::vec4 moveCam(glm::vec4 view_vec, glm::vec4 up_vec, glm::vec4 camera_pos){
    float delta = 50.0f;
    glm::vec4 side_vec = crossproduct(view_vec, up_vec);
    if(g_keys.w)
        camera_pos += delta * view_vec;
    if(g_keys.s)
        camera_pos -= delta * view_vec;
    if(g_keys.d)
        camera_pos += delta * side_vec;
    if(g_keys.a)
        camera_pos -= delta * side_vec;
    if(g_keys.space)
        camera_pos += delta * up_vec;
    if(g_keys.shift)
        camera_pos -= delta * up_vec;
    return camera_pos;
}


int main( int argc, char** argv )
{
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(800, 600, "CMP143", NULL, NULL);

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetWindowSize(window, 800, 600); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

    glfwMakeContextCurrent(window);
    gl3wInit();

    ShaderInfo  shaders[] =
    {
        { GL_VERTEX_SHADER,   "../shaders/main.vert" },
        { GL_FRAGMENT_SHADER, "../shaders/main.frag" },
        { GL_NONE, NULL }
    };

    GLuint program = LoadShaders(shaders);
    glUseProgram(program);

    glGenVertexArrays(NumVAOs, VAOs);
    glBindVertexArray(VAOs[Triangles]);
    ObjectInfo Obj = ReadObject("../objs/cow.in");

    std::vector<GLfloat> vertices;
    for(auto &obj : Obj.position){
        vertices.emplace_back(obj.x);
        vertices.emplace_back(obj.y);
        vertices.emplace_back(obj.z);
    }
    std::vector<GLfloat> colors;
    for(auto &obj : Obj.color){
        colors.emplace_back(obj.x);
        colors.emplace_back(obj.y);
        colors.emplace_back(obj.z);
    }

    glCreateBuffers(NumBuffers, Buffers);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), vertices.data(), 0);

    glVertexAttribPointer(vPosition, 3, GL_FLOAT,
        GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vPosition);

    glBindBuffer(GL_ARRAY_BUFFER, Buffers[colorBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, colors.size()*sizeof(GLfloat), colors.data(), 0);

    glVertexAttribPointer(vColor, 3, GL_FLOAT,
        GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vColor);

    GLint model_uniform      = glGetUniformLocation(program, "model");      // Variável da matriz "model"
    GLint view_uniform       = glGetUniformLocation(program, "view");       // Variável da matriz "view" em shader_vertex.glsl
    GLint projection_uniform = glGetUniformLocation(program, "projection"); // Variável da matriz "projection" em shader_vertex.glsl
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 model = Matrix_Scale(1, 1, 1) * Matrix_Translate(-Obj.center.x, -Obj.center.y, -Obj.center.z);

    glm::vec4 camera_position_c  = glm::vec4(100.0f, 200.0f, 1000.0f, 1.0f);        // Ponto "c", centro da câmera
    glm::vec4 camera_lookat_l    = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);               // Ponto "l", para onde a câmera (look-at) estará sempre olhando
    glm::vec4 camera_view_vector = normalize(camera_lookat_l - camera_position_c);  // Vetor "view", sentido para onde a câmera está virada
    glm::vec4 camera_up_vector   = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);               // Vetor "up" fixado para apontar para o "céu" (eito Y global)

    while (!glfwWindowShouldClose(window))
    {
        static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

        camera_position_c  = moveCam(camera_view_vector, camera_up_vector, camera_position_c); 
        
        if(g_lookAt)
            camera_view_vector = normalize(camera_lookat_l - camera_position_c);
        
        view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

        float nearplane = -0.0000000000001f;  // Posição do "near plane"
        float farplane  = -1000000000000.0f;  // Posição do "far plane"

        if (g_UsePerspectiveProjection) {
            float field_of_view = 3.141592f / 3.0f;
            projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);
        }
        else {
            float t = 1.5f*g_CameraDistance/2.5f;
            float b = -t;
            float r = t*g_ScreenRatio;
            float l = -r;
            projection = Matrix_Orthographic(l, r, b, t, nearplane, farplane);
        }

        glUniformMatrix4fv(model_uniform      , 1 , GL_FALSE , glm::value_ptr(model));
        glUniformMatrix4fv(view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

        glClearBufferfv(GL_COLOR, 0, black);

        glBindVertexArray(VAOs[Triangles]);

        switch (g_chosenType){
            case point:
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                break;

            case line:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;

            case triangle:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    g_ScreenRatio = (float)width / height;
}

// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{

}

// Função callback chamada sempre que o usuário movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
    float dx = xpos - g_LastCursorPosX;
    float dy = ypos - g_LastCursorPosY;

    // Atualizamos as variáveis globais para armazenar a posição atual do
    // cursor como sendo a última posição conhecida do cursor.
    g_LastCursorPosX = xpos;
    g_LastCursorPosY = ypos;
}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    g_CameraDistance -= 0.1f*yoffset;

    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (g_CameraDistance < verysmallnumber)
        g_CameraDistance = verysmallnumber;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        g_UsePerspectiveProjection = !g_UsePerspectiveProjection;

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
        g_lookAt = !g_lookAt;

    if (key == GLFW_KEY_E && action == GLFW_PRESS)
        g_chosenType = g_chosenType == triangle ? point : g_chosenType+1;

    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        g_keys.w = true;

    if (key == GLFW_KEY_W && action == GLFW_RELEASE)
        g_keys.w = false;

    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        g_keys.s = true;

    if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        g_keys.s = false;

    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        g_keys.a = true;

    if (key == GLFW_KEY_A && action == GLFW_RELEASE)
        g_keys.a = false;

    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        g_keys.d = true;

    if (key == GLFW_KEY_D && action == GLFW_RELEASE)
        g_keys.d = false;
    
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        g_keys.space = true;

    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
        g_keys.space = false;

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
        g_keys.shift = true;

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
        g_keys.shift = false;
}
