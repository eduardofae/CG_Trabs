#include <GL3/gl3.h>
#include <GL3/gl3w.h>

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
} ShaderInfo;

typedef struct {
    std::vector<glm::vec3> position;
    std::vector<glm::vec3> normal;
    std::vector<glm::vec3> face_normal;
    std::vector<int> material_id;
    std::vector<MaterialInfo> materialInfos;
    glm::vec3 center;
} ObjectInfo;

static const GLchar* ReadShader(const char* filename);
GLuint LoadShaders(ShaderInfo* shaders);
ObjectInfo ReadObject(char *FileName);
