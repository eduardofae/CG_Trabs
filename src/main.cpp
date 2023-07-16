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
#include "./loader/loader.hpp"
#include "./GUI/GUI.hpp"
#include "./camera/camera.hpp"
#include "./render/openGL.hpp"
#include "./render/closeGL.hpp"

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

float  g_ScreenRatio = 1.0f;
float  g_CameraDistance = 1000.0f;
int    g_projectionType = perspective;
int    g_camStyle = camLookAt;
bool   g_reset = false;
int    g_windingOrder = ccw;
bool   g_backFaceCulling = true;
int    g_mashType = triangle;
double g_LastCursorPosX, g_LastCursorPosY;
bool   g_rotateCam = false;
int    g_renderType = openGL;

glm::vec4 g_cameraInitialPosition = glm::vec4(0.0f, 1000.0f, 1500.0f, 1.0f);
PressedKeys g_keys{false, false, false, false, false, false};
WindowSize g_windowSize{800, 600};
Angles g_angles{0.0f, 0.0f};

CloseToGL cgl;

// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

int main( int argc, char** argv )
{
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(800, 600, "CMP143", NULL, NULL);

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetWindowSize(window, g_windowSize.width, g_windowSize.height); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

    glfwSwapInterval(0);

    glfwMakeContextCurrent(window);
    gl3wInit();

    startGUI(window);

    ShaderLocation  openShaders[] =
    {
        { GL_VERTEX_SHADER,   "../shaders/openGL.vert" },
        { GL_FRAGMENT_SHADER, "../shaders/openGL.frag" },
        { GL_NONE, NULL }
    };

    ShaderLocation  closeToShaders[] =
    {
        { GL_VERTEX_SHADER,   "../shaders/closeGL.vert" },
        { GL_FRAGMENT_SHADER, "../shaders/closeGL.frag" },
        { GL_NONE, NULL }
    };  

    GLuint openProgram    = LoadShaders(openShaders);
    GLuint closeToProgram = LoadShaders(closeToShaders);
    GLuint lightModels[numShadingTypes];
    lightModels[GouAD]    = glGetSubroutineIndex(openProgram, GL_VERTEX_SHADER  , "GAD");
    lightModels[GouADS]   = glGetSubroutineIndex(openProgram, GL_VERTEX_SHADER  , "GADS");
    lightModels[NoneVert] = glGetSubroutineIndex(openProgram, GL_VERTEX_SHADER  , "None");
    lightModels[Phong]    = glGetSubroutineIndex(openProgram, GL_FRAGMENT_SHADER, "Phong");
    lightModels[NoneFrag] = glGetSubroutineIndex(openProgram, GL_FRAGMENT_SHADER, "None");

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
    std::vector<GLint> material_id;
    for(auto &mat : Obj.material_id){
        material_id.emplace_back(mat);
    }

    glEnable(GL_DEPTH_TEST);
    glGenVertexArrays(NumVAOs, VAOs);
    buildOpenGL(VAOs, Buffers, vertices, normals, material_id);
    cgl.buildCloseGL(VAOs, Buffers);
    cgl.updateWindowSize(g_windowSize);

    Matrices matrices;
    matrices.model = Matrix_Scale(1, 1, 1) * Matrix_Translate(-Obj.center.x, -Obj.center.y, -Obj.center.z);

    glm::vec4 camera_position_c  = g_cameraInitialPosition;                         // Ponto "c", centro da câmera
    glm::vec4 camera_lookat_l    = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);               // Ponto "l", para onde a câmera (look-at) estará sempre olhando
    glm::vec4 camera_view_vector = normalize(camera_lookat_l - camera_position_c);  // Vetor "view", sentido para onde a câmera está virada
    glm::vec4 camera_up_vector   = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);               // Vetor "up" fixado para apontar para o "céu" (eito Y global)

    bool  useColor       = true;
    float color[3]       = { 1.0f, 1.0f, 1.0f }; // Cor sendo usada
    float nearplane      = 0.1f;                 // Posição do "near plane"
    float farplane       = 5000.0f;              // Posição do "far plane"
    float field_of_viewV = 60.0f;                // Campo de visão Vertical
    float field_of_viewH = 60.0f;                // Campo de visão horizontal
    float last_time      = 0.0f;
    bool  symmetric      = true;
    int   shadingType    = NoneVert;

    while (!glfwWindowShouldClose(window))
    {
        static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        glClearBufferfv(GL_COLOR, 0, black);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        float time       = (float) glfwGetTime();
        float delta_time = time - last_time;
        last_time        = time;

        camera_position_c  = moveCam(camera_view_vector, camera_up_vector, camera_position_c, g_keys, g_reset, g_cameraInitialPosition, delta_time);
        
        if(g_camStyle == camLookAt)
            camera_view_vector = normalize(camera_lookat_l - camera_position_c);
        else
            camera_view_vector = rotateCam(camera_view_vector, camera_up_vector, &g_angles);
        
        matrices.view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

        if (g_projectionType == perspective)
        {
            if(symmetric)
                matrices.proj = Matrix_Perspective(glm::radians(field_of_viewV), g_ScreenRatio, -nearplane, -farplane);
            else
                matrices.proj = Matrix_Frustum(glm::radians(field_of_viewV), glm::radians(field_of_viewH), -nearplane, -farplane);
        }
        else {
            float t = 1.5f*g_CameraDistance/2.5f;
            float b = -t;
            float r = t*g_ScreenRatio;
            float l = -r;
            matrices.proj = Matrix_Orthographic(l, r, b, t, -nearplane, -farplane);
        }

        matrices.invModelView = glm::transpose(glm::inverse(matrices.model) * glm::inverse(matrices.view));
        
        if(g_renderType == openGL)
            renderOpenGL(openProgram, matrices, color, useColor,
            VAOs, g_mashType, g_windingOrder, g_backFaceCulling, vertices.size(),
            shadingType, lightModels, camera_position_c, Obj.materialInfos);
        else
            cgl.renderCloseGL(closeToProgram, matrices, color, useColor,
            VAOs, g_mashType, g_windingOrder, g_backFaceCulling, Obj,
            shadingType, camera_position_c);

        renderGUI(&g_mashType, &g_backFaceCulling, &g_windingOrder, &useColor,
                  color, &g_camStyle, &g_projectionType, &farplane, &nearplane,
                  &field_of_viewV, &g_CameraDistance,
                  &camera_position_c, g_cameraInitialPosition,
                  &camera_view_vector, camera_lookat_l,
                  &g_renderType, delta_time, &field_of_viewH, &symmetric, &shadingType);

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
    
    cgl.updateWindowSize(g_windowSize);
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
        g_mashType = g_mashType == triangle ? point : g_mashType+1;
    
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
