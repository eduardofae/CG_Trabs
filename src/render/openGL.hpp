#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <GLFW/glfw3.h>

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>

#include "../utils/enums.hpp"
#define BUFFER_OFFSET(a) ((void*)(a))

void buildOpenGL(GLuint *VAOs, GLuint *Buffers,
                 std::vector<GLfloat> vertices, std::vector<GLfloat> normals,
                 std::vector<GLint> material_id, std::vector<GLfloat> text_coords);
void renderOpenGL(GLuint program, Matrices matrices,
                  float *color, bool useColor, GLuint *VAOs,
                  int g_mashType, int g_windingOrder, int g_backFaceCulling, int size,
                  int shadingType, GLuint *lightModels, glm::vec4 camera_position,
                  std::vector <MaterialInfo> materials, GLuint texture_id, GLuint sampler_id,
                  bool hasTexture);
void updateTextureOpenGL(TextureInfo texture, GLuint * texture_id, GLuint *sampler_id);