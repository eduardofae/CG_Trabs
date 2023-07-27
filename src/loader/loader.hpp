#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <stb_image.h>

#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../utils/printMatrices.hpp"
#include "../utils/enums.hpp"

#define BUFFER_OFFSET(a) ((void*)(a))

typedef struct {
    GLenum       type;
    const char* filename;
    GLuint       shader;
} ShaderLocation;

static const GLchar* ReadShader(const char* filename);
GLuint LoadShaders(ShaderLocation* shaders);
ObjectInfo ReadObject(char *FileName);
TextureInfo LoadTextureImage(const char* filename);