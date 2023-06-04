//////////////////////////////////////////////////////////////////////////////
//
//  Main.cpp
//
//////////////////////////////////////////////////////////////////////////////

#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

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
enum Buffer_IDs { ArrayBuffer, NormalBuffer, ColorBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0, vNormal = 1, vColor = 2 };
enum rType      { point = 0, line = 1, triangle = 2 };
enum orders     { cw = 0, ccw = 1 };
enum cStyle     { cLookAt = 0, cFree = 1 };
enum pType      { perspective = 0, ortographic = 1 };

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

float  g_ScreenRatio = 1.0f;
float  g_CameraDistance = 1000.0f;
int    g_projectionType = perspective;
int    g_cStyle = cLookAt;
bool   g_reset = false;
int    g_windingOrder = cw;
bool   g_backFaceCulling = true;
int    g_chosenType = triangle;
double g_LastCursorPosX, g_LastCursorPosY;

glm::vec4 g_cameraInitialPosition = glm::vec4(100.0f, 200.0f, 1000.0f, 1.0f);

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

glm::vec4 moveCam(glm::vec4 view_vec, glm::vec4 up, glm::vec4 camera_pos){
    float delta = 50.0f;
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

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

    bool  g_useColor = true;
    float color[3]      = { 1.0f, 1.0f, 1.0f }; // Cor sendo usada
    float nearplane     = 0.01f;                // Posição do "near plane"
    float farplane      = 5000.0f;              // Posição do "far plane"
    float field_of_view = 60.0f;                // Campo de visão

    while (!glfwWindowShouldClose(window))
    {
        static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        glClearBufferfv(GL_COLOR, 0, black);

        camera_position_c  = moveCam(camera_view_vector, camera_up_vector, camera_position_c);
        
        if(g_cStyle == cLookAt)
            camera_view_vector = normalize(camera_lookat_l - camera_position_c);
        
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
        glUniform1i(glGetUniformLocation(program, "useColor")          ,                g_useColor);

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

        if (g_windingOrder == cw) glFrontFace(GL_CW);
        else glFrontFace(GL_CCW);

        if(g_backFaceCulling) glEnable(GL_CULL_FACE); 
        else glDisable(GL_CULL_FACE);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Object Properties");
        {
            if (ImGui::BeginTabBar("tabs", 0))
            {
                if (ImGui::BeginTabItem("Render"))
                {
                    ImGui::SeparatorText("Render Type");
                    ImGui::RadioButton("Triangles", &g_chosenType, triangle); ImGui::SameLine();
                    ImGui::RadioButton("Lines", &g_chosenType, line); ImGui::SameLine();
                    ImGui::RadioButton("Points", &g_chosenType, point);

                    ImGui::SeparatorText("Back Face Culling");
                    ImGui::Checkbox("##", &g_backFaceCulling); ImGui::SameLine();
                    ImGui::RadioButton("CW", &g_windingOrder, cw); ImGui::SameLine();
                    ImGui::RadioButton("CCW", &g_windingOrder, ccw);

                    ImGui::SeparatorText("Color");
                    ImGui::Checkbox("##", &g_useColor); ImGui::SameLine();
                    ImGui::ColorEdit3("##", color);

                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Camera"))
                {
                    ImGui::SeparatorText("Camera Style");
                    ImGui::RadioButton("LookAt", &g_cStyle, cLookAt); ImGui::SameLine();
                    ImGui::RadioButton("Free", &g_cStyle, cFree);

                    ImGui::SeparatorText("Projection Style");
                    ImGui::RadioButton("Perspective", &g_projectionType, perspective); ImGui::SameLine();
                    ImGui::RadioButton("Ortographic", &g_projectionType, ortographic);

                    ImGui::SeparatorText("Projection Parameters");
                    ImGui::DragFloat("Far Plane"   , &farplane        , 1.0f , std::max(nearplane, 1.0f), std::numeric_limits<float>::max());
                    ImGui::DragFloat("Near Plane"  , &nearplane       , 0.01f,           0.01f          , farplane);
                    if(g_projectionType == perspective)
                        ImGui::DragFloat("FOV"     , &field_of_view   , 1.0f ,           5.0f           , 120.0f);
                    else
                        ImGui::DragFloat("Distance", &g_CameraDistance, 1.0f ,           1.0f           , std::numeric_limits<float>::max());

                    ImGui::SeparatorText("Reset Camera Position");
                    if (ImGui::Button("Reset")){
                        camera_position_c = g_cameraInitialPosition;
                        camera_view_vector = normalize(camera_lookat_l - camera_position_c);
                    } 

                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Camera"))
                {
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{

}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    float dx = xpos - g_LastCursorPosX;
    float dy = ypos - g_LastCursorPosY;

    g_LastCursorPosX = xpos;
    g_LastCursorPosY = ypos;
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
        g_cStyle = g_cStyle == cLookAt ? cFree : cLookAt;

    if (key == GLFW_KEY_O && action == GLFW_PRESS)
        g_windingOrder = !g_windingOrder;

    if (key == GLFW_KEY_B && action == GLFW_PRESS)
        g_backFaceCulling = !g_backFaceCulling;

    if (key == GLFW_KEY_E && action == GLFW_PRESS)
        g_chosenType = g_chosenType == triangle ? point : g_chosenType+1;
    
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
