#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <vector>
#include <array>

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../utils/enums.hpp"
#define BUFFER_OFFSET(a) ((void*)(a))

int buildCloseGL(GLuint *VAOs, GLuint *Buffers, std::vector<GLfloat> vertices, std::vector<GLfloat> normals, std::vector<GLfloat> colors,
                  glm::mat4 model, glm::mat4 view, glm::mat4 projection);
void renderCloseGL(GLuint program, glm::mat4 model, glm::mat4 view, glm::mat4 projection,
                   float *color, bool useColor, GLuint *VAOs,
                   int g_renderType, int g_windingOrder, int g_backFaceCulling,
                   GLuint *Buffers, std::vector<GLfloat> vertices, std::vector<GLfloat> normals, std::vector<GLfloat> colors);