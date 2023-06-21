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
#include "./GUI/GUI.hpp"

enum VAO_IDs    { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NormalBuffer, ColorBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0, vNormal = 1, vColor = 2 };

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

float  g_ScreenRatio = 1.0f;
float  g_CameraDistance = 1000.0f;
int    g_projectionType = perspective;
int    g_camStyle = camLookAt;
bool   g_reset = false;
int    g_windingOrder = cw;
bool   g_backFaceCulling = true;
int    g_renderType = triangle;
double g_LastCursorPosX, g_LastCursorPosY;
bool   g_rotateCam = false;

glm::vec4 g_cameraInitialPosition = glm::vec4(0.0f, 1000.0f, 1500.0f, 1.0f);

typedef struct {
		bool w, a, s, d, space, shift;
} PressedKeys;

typedef struct {
		int height, width;
} WindowSize;

typedef struct {
		float angleX, angleY;
} Angles;

PressedKeys g_keys{false, false, false, false, false, false};
WindowSize g_windowSize{800, 600};
Angles g_angles{0.0f, 0.0f};

// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

glm::vec4 moveCam(glm::vec4 view_vec, glm::vec4 up, glm::vec4 camera_pos){
    float delta = 30.0f;
    glm::vec4 side_vec = crossproduct(view_vec, up);
    glm::vec4 up_vec = crossproduct(side_vec, view_vec);
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
    if(g_reset)
        camera_pos = g_cameraInitialPosition;
    return camera_pos;
}

glm::vec4 rotateCam(glm::vec4 view_vec, glm::vec4 up){
    if(g_angles.angleX == 0 && g_angles.angleY == 0) return view_vec;

    float cam_look_speed = 0.3f;

    glm::vec4 side_vec = crossproduct(view_vec, up);
    glm::vec4 up_vec = crossproduct(side_vec, view_vec);

	// ROTAÇÃO VERTICAL
    glm::vec4 lado = crossproduct(up_vec, view_vec); // Calcula o lado, para rotacionar verticalmente
    glm::vec4 aux = view_vec * Matrix_Rotate(-g_angles.angleY * cam_look_speed, lado);   // Rotação no eixo lado (vertical)

    // TRAVA DA ROTAÇÃO VERTICAL
    if(dotproduct(lado, crossproduct(up, aux)) > 0) { // Testa se o novo valor de lado é igual ao antigo
        view_vec = aux;                                   // Caso seja, salva o novo camera_view (permite a rotação)
	}
    // ROTAÇÃO HORIZONTAL
    view_vec = view_vec * Matrix_Rotate(g_angles.angleX * cam_look_speed, up_vec); // Rotação no eixo up (horizontal)

	g_angles.angleX = 0.0f;
	g_angles.angleY = 0.0f;

    return view_vec;
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
    glfwSetWindowSize(window, g_windowSize.height, g_windowSize.width); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

    glfwMakeContextCurrent(window);
    gl3wInit();

    startGUI(window);

    ShaderInfo  shaders[] =
    {
        { GL_VERTEX_SHADER,   "../shaders/main.vert" },
        { GL_FRAGMENT_SHADER, "../shaders/main.frag" },
        { GL_NONE, NULL }
    };

    GLuint program = LoadShaders(shaders);
    glUseProgram(program);
    glEnable(GL_DEPTH_TEST);

    glGenVertexArrays(NumVAOs, VAOs);
    glBindVertexArray(VAOs[Triangles]);
    ObjectInfo Obj = ReadObject("../objs/cow.in");

    std::vector<GLfloat> vertices;
    for(auto &obj : Obj.position){
        vertices.emplace_back(obj.x);
        vertices.emplace_back(obj.y);
        vertices.emplace_back(obj.z);
    }
    std::vector<GLfloat> normals;
    for(auto &obj : Obj.normal){
        normals.emplace_back(obj.x);
        normals.emplace_back(obj.y);
        normals.emplace_back(obj.z);
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

    glBindBuffer(GL_ARRAY_BUFFER, Buffers[NormalBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, normals.size()*sizeof(GLfloat), normals.data(), 0);

    glVertexAttribPointer(vNormal, 3, GL_FLOAT,
        GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vNormal);

    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ColorBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, colors.size()*sizeof(GLfloat), colors.data(), 0);

    glVertexAttribPointer(vColor, 3, GL_FLOAT,
        GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vColor);

    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 model = Matrix_Scale(1, 1, 1) * Matrix_Translate(-Obj.center.x, -Obj.center.y, -Obj.center.z);

    glm::vec4 camera_position_c  = g_cameraInitialPosition;                         // Ponto "c", centro da câmera
    glm::vec4 camera_lookat_l    = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);               // Ponto "l", para onde a câmera (look-at) estará sempre olhando
    glm::vec4 camera_view_vector = normalize(camera_lookat_l - camera_position_c);  // Vetor "view", sentido para onde a câmera está virada
    glm::vec4 camera_up_vector   = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);               // Vetor "up" fixado para apontar para o "céu" (eito Y global)

    bool  useColor      = true;
    float color[3]      = { 1.0f, 1.0f, 1.0f }; // Cor sendo usada
    float nearplane     = 0.01f;                // Posição do "near plane"
    float farplane      = 5000.0f;              // Posição do "far plane"
    float field_of_view = 60.0f;                // Campo de visão

    while (!glfwWindowShouldClose(window))
    {
        static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        glClearBufferfv(GL_COLOR, 0, black);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        camera_position_c  = moveCam(camera_view_vector, camera_up_vector, camera_position_c);
        
        if(g_camStyle == camLookAt)
            camera_view_vector = normalize(camera_lookat_l - camera_position_c);
        else
            camera_view_vector = rotateCam(camera_view_vector, camera_up_vector);
        
        view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

        if (g_projectionType == perspective)
            projection = Matrix_Perspective(glm::radians(field_of_view), g_ScreenRatio, -nearplane, -farplane);
        else {
            float t = 1.5f*g_CameraDistance/2.5f;
            float b = -t;
            float r = t*g_ScreenRatio;
            float l = -r;
            projection = Matrix_Orthographic(l, r, b, t, -nearplane, -farplane);
        }

        glUniformMatrix4fv(glGetUniformLocation(program, "model")      , 1 , GL_FALSE , glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(program, "view")       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program, "projection") , 1 , GL_FALSE , glm::value_ptr(projection));
        glUniform3fv(glGetUniformLocation(program, "color")            , 1            , color);
        glUniform1i(glGetUniformLocation(program, "useColor")          ,                useColor);

        glBindVertexArray(VAOs[Triangles]);

        switch (g_renderType){
            case point:
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                break;

            case line:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;

            case triangle:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (g_windingOrder == cw) glFrontFace(GL_CW);
        else glFrontFace(GL_CCW);

        if(g_backFaceCulling) glEnable(GL_CULL_FACE); 
        else glDisable(GL_CULL_FACE);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        renderGUI(&g_renderType, &g_backFaceCulling, &g_windingOrder, &useColor,
               color, &g_camStyle, &g_projectionType, &farplane, &nearplane,
               &field_of_view, &g_CameraDistance,
               &camera_position_c, g_cameraInitialPosition,
               &camera_view_vector, camera_lookat_l);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    g_windowSize.width  = width;
    g_windowSize.height = height;
    g_ScreenRatio = (float)width / height;
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if(!(g_camStyle == camFree)){
        g_rotateCam = false;
        return;
    }
    if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        g_rotateCam = true;
    if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        g_rotateCam = false;
}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    float dx = xpos - g_LastCursorPosX;
    float dy = ypos - g_LastCursorPosY;
    g_LastCursorPosX = xpos;
    g_LastCursorPosY = ypos;

    if(!g_rotateCam) return;

    float PI = 3.1415;
    g_angles.angleX = dx/(g_windowSize.width/2)  * 2*PI; // Calcula o ângulo rotação horizontal de acordo com a porcentagem da tela movida (máximo = 2*PI)
    g_angles.angleY = dy/(g_windowSize.height/2) * 2*PI; // Calcula o ângulo rotação  vertical  de acordo com a porcentagem da tela movida (máximo = 2*PI)
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    g_CameraDistance -= yoffset;

    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (g_CameraDistance < verysmallnumber)
        g_CameraDistance = verysmallnumber;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        g_projectionType = g_projectionType == perspective ? ortographic : perspective;

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
        g_camStyle = g_camStyle == camLookAt ? camFree : camLookAt;

    if (key == GLFW_KEY_O && action == GLFW_PRESS)
        g_windingOrder = !g_windingOrder;

    if (key == GLFW_KEY_B && action == GLFW_PRESS)
        g_backFaceCulling = !g_backFaceCulling;

    if (key == GLFW_KEY_E && action == GLFW_PRESS)
        g_renderType = g_renderType == triangle ? point : g_renderType+1;
    
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
        g_reset = true;

    if (key == GLFW_KEY_R && action == GLFW_RELEASE)
        g_reset = false;

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
