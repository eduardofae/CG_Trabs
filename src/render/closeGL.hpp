#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <iostream>

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../utils/enums.hpp"
#include "../utils/matrices.hpp"
#define BUFFER_OFFSET(a) ((void*)(a))

struct Pixel {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

    Pixel(glm::vec4 cor){
        r = 255 * glm::clamp(cor.x, 0.0f, 1.0f);
        g = 255 * glm::clamp(cor.y, 0.0f, 1.0f);
        b = 255 * glm::clamp(cor.z, 0.0f, 1.0f);
        a = 255 * glm::clamp(cor.w, 0.0f, 1.0f);
    }

    Pixel(){
        r = 0;
        g = 0;
        b = 0;
        a = 0;
    }
};

typedef struct {
    glm::vec4 pos;
    int mat_id;
    glm::vec4 norm;
    glm::vec4 color;
    glm::vec4 pixel_pos;
    float w;
} PointInfo;

typedef struct {
    int shadingType;
    glm::vec4 camera_position;
    std::vector <MaterialInfo> materials;
} ShaderInfo;

typedef struct {
    int windingOrder;
    int backFaceCulling;
} CullingInfo;

enum tType {
    Top,
    Bot
};

class CloseToGL{
    public:
        CloseToGL() = default;
        auto buildCloseGL(GLuint *VAOs, GLuint *Buffers) -> void;
        auto renderCloseGL(GLuint program, Matrices matrices,
                           float *color, bool useColor, GLuint *VAOs,
                           int g_mashType, int g_windingOrder, int g_backFaceCulling,
                           ObjectInfo Obj, int shadingType, glm::vec4 camera_position) -> void;
        auto updateWindowSize(WindowSize windowSize) -> void;
    private:
        std::vector <Pixel> ColorBuffer;
        std::vector <float> ZBuffer;
        WindowSize  windowSize;
        ShaderInfo  shader;
        GLuint      textureID = 0;
        glm::mat4   view_port;

        auto rasterize(std::array<PointInfo, 3> vertices) -> void;
        auto vertex(PointInfo vertex)-> glm::vec4;
        auto fragment(PointInfo fragment) -> Pixel;
        auto cleanBuffers() -> void;
        auto drawTopTriangle(std::array<PointInfo, 3> vertices) -> void;
        auto drawBotTriangle(std::array<PointInfo, 3> vertices) -> void;
        auto scanline(PointInfo left, PointInfo right, int y) -> void;
        auto interpolate(PointInfo top, PointInfo bot, float t) -> PointInfo;
        auto fillBuffers(Pixel p, int x, int y, float z) -> void;
        auto backFaceCulling(ObjectInfo Obj, Matrices matrices, CullingInfo cullingInfo) -> void;
        auto frustrumCulling(ObjectInfo Obj, Matrices matrices, int i) -> void;
        auto linkTexture(GLuint program) -> void;
        auto setShaderInfo(ObjectInfo Obj, float *color, bool useColor,
                           int shadingType, glm::vec4 camera_position) -> void;
};
