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

const GLuint  NumVertices  = 8;
const GLuint  NumTriangles = 12;

std::vector<GLfloat> get_vertices();
std::vector<GLfloat> get_colors();
