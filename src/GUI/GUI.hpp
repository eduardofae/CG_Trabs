#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>

#include "../utils/matrices.hpp"

enum rType {
    point    = 0,
    line     = 1,
    triangle = 2
};

enum orders {
    cw  = 0,
    ccw = 1
};

enum cStyle {
    camLookAt = 0,
    camFree   = 1
};

enum pType {
    perspective = 0,
    ortographic = 1
};

void startGUI(GLFWwindow* window);
void renderGUI(int *g_renderType, bool *g_backFaceCulling, int *g_windingOrder, bool *useColor,
               float *color, int *g_camStyle, int *g_projectionType, float *farplane, float *nearplane,
               float *field_of_view, float *g_CameraDistance,
               glm::vec4 *camera_position_c, glm::vec4 g_cameraInitialPosition,
               glm::vec4 *camera_view_vector, glm::vec4 camera_lookat_l);