#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <GLFW/glfw3.h>

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../utils/enums.hpp"

void renderOpenGL(GLuint program, glm::mat4 model, glm::mat4 view, glm::mat4 projection,
                  float *color, bool useColor, GLuint *VAOs, int VAOid,
                  int g_renderType, int g_windingOrder, int g_backFaceCulling, int size);