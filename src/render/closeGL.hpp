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

#include "../utils/enums.hpp"
#include "../utils/matrices.hpp"
#define BUFFER_OFFSET(a) ((void*)(a))

class CloseToGL{
    public:
        CloseToGL() = default;
        auto buildCloseGL(GLuint *VAOs, GLuint *Buffers) -> void;
        auto renderCloseGL(GLuint program, glm::mat4 model, glm::mat4 view, glm::mat4 projection,
                           float *color, bool useColor, GLuint *VAOs,
                           int g_mashType, int g_windingOrder, int g_backFaceCulling, GLuint *Buffers,
                           std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<int> material_id,
                           int shadingType, glm::vec4 camera_position, std::vector <MaterialInfo> materials,
                           WindowSize windowSize) -> void;
    private:
        std::vector <float> ColorBuffer;
        std::vector <float> OutBuffer;
        std::vector <float> ZBuffer;

        auto rasterize(std::array<glm::vec4, 3> pos, std::array<int, 3> mat_id,
                       std::array<glm::vec4, 3> norm, std::array<glm::vec4, 3> color,
                       std::array<glm::vec4, 3> pixel_pos,
                       int shadingType, glm::vec4 camera_position, WindowSize windowSize,
                       std::vector <MaterialInfo> materials, glm::mat4 view_port) -> void;
        auto vertex(glm::vec4 pos, int mat_id, glm::vec4 norm,
                    int shadingType, glm::vec4 camera_position,
                    std::vector <MaterialInfo> materials) -> glm::vec4;
        auto fragment(glm::vec4 pos, int mat_id, glm::vec4 norm, glm::vec4 color,
                     int shadingType, glm::vec4 camera_position,
                     std::vector <MaterialInfo> materials) -> glm::vec4;
        auto cleanBuffers(WindowSize windowSize) -> void;
};

