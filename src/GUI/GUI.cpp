#include "GUI.hpp"

void startGUI(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void renderGUI(int *g_renderType, bool *g_backFaceCulling, int *g_windingOrder, bool *useColor,
               float *color, int *g_camStyle, int *g_projectionType, float *farplane, float *nearplane,
               float *field_of_view, float *g_CameraDistance,
               glm::vec4 *camera_position_c, glm::vec4 g_cameraInitialPosition,
               glm::vec4 *camera_view_vector, glm::vec4 camera_lookat_l)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Scene Properties");
    {
        if (ImGui::BeginTabBar("##tab", 0))
        {
            if (ImGui::BeginTabItem("Object"))
            {
                ImGui::SeparatorText("Render Type");
                ImGui::RadioButton("Triangles", g_renderType, triangle); ImGui::SameLine();
                ImGui::RadioButton("Lines", g_renderType, line); ImGui::SameLine();
                ImGui::RadioButton("Points", g_renderType, point);

                ImGui::SeparatorText("Back Face Culling");
                ImGui::Checkbox("##bfc" , g_backFaceCulling); ImGui::SameLine();
                ImGui::RadioButton("CW" , g_windingOrder, cw); ImGui::SameLine();
                ImGui::RadioButton("CCW", g_windingOrder, ccw);

                ImGui::SeparatorText("Color");
                ImGui::Checkbox("##ccb" , useColor); ImGui::SameLine();
                ImGui::ColorEdit3("##cw", color);

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Camera"))
            {
                ImGui::SeparatorText("Camera Style");
                ImGui::RadioButton("LookAt", g_camStyle, camLookAt); ImGui::SameLine();
                ImGui::RadioButton("Free"  , g_camStyle, camFree);

                ImGui::SeparatorText("Projection Style");
                ImGui::RadioButton("Perspective", g_projectionType, perspective); ImGui::SameLine();
                ImGui::RadioButton("Ortographic", g_projectionType, ortographic);

                ImGui::SeparatorText("Projection Parameters");
                ImGui::DragFloat("Far"         , farplane        , 1.0f , std::fmax(*nearplane, 1.0f), std::numeric_limits<float>::max());
                ImGui::DragFloat("Near"        , nearplane       , 0.01f,            0.01f          , *farplane);
                if(*g_projectionType == perspective)
                    ImGui::DragFloat("FOV"     , field_of_view   , 1.0f ,            5.0f           , 120.0f);
                else
                    ImGui::DragFloat("Distance", g_CameraDistance, 1.0f ,            1.0f           , std::numeric_limits<float>::max());

                ImGui::SeparatorText("Camera Position");
                ImGui::DragFloat("X", &camera_position_c->x, 1.0f , -std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
                ImGui::DragFloat("Y", &camera_position_c->y, 1.0f,  -std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
                ImGui::DragFloat("Z", &camera_position_c->z, 1.0f , -std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
                if (ImGui::Button("Reset")){
                    *camera_position_c = g_cameraInitialPosition;
                    *camera_view_vector = normalize(camera_lookat_l - *camera_position_c);
                } 

                ImGui::EndTabItem();
            }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}