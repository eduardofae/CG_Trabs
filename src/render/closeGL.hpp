#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>
#include <stb_image_write.h>

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
    glm::vec2 text_coords;
    float w;
    float dsx;
    float dsy;
    float dtx;
    float dty;
} PointInfo;

typedef struct {
    int shadingType;
    glm::vec4 camera_position;
    std::vector <MaterialInfo> materials;
    int mashType;
    bool useTexture;
    int samplingType;
    std::vector <TextureInfo> mipmap;

    glm::vec3 lambertTerm;
} ShaderInfo;
    
typedef struct {
    TextureInfo texture;
    std::vector <TextureInfo> mipmap;
} Texture;

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
        auto setTexture(TextureInfo &texture) -> void;
        auto renderCloseGL(GLuint program, Matrices matrices,
                           float *color, bool useColor, GLuint *VAOs,
                           int g_mashType, int g_windingOrder, int g_backFaceCulling,
                           ObjectInfo Obj, int shadingType, glm::vec4 camera_position,
                           bool useTexture, int samplingType) -> void;
        auto updateWindowSize(WindowSize windowSize) -> void;
    private:
        std::vector <Pixel> ColorBuffer;
        std::vector <float> ZBuffer;
        WindowSize  windowSize;
        ShaderInfo  shader;
        Texture     texture;
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
        auto backFaceCulling(std::array<PointInfo, 3> vertices, CullingInfo cullingInfo) -> void;
        auto drawImage(ObjectInfo &Obj, Matrices matrices,  CullingInfo cullingInfo) -> void;
        auto linkTexture(GLuint program) -> void;
        auto setShaderInfo(ObjectInfo &Obj, float *color, bool useColor, int shadingType,
                           glm::vec4 camera_position, int mashType,
                           bool useTexture, int samplingType) -> void;
        auto drawLine(std::array<PointInfo, 2> vertices) -> void;
        auto sampleTexture(PointInfo &point) -> glm::vec4;
        auto interpolateColor(glm::vec4 a, glm::vec4 b, float t) -> glm::vec4;
        auto readTexture(int x, int y, int level) -> glm::vec4;
        auto genMipMaps() -> void;
        auto bilinearInterpolation(float x, float y, int level) -> glm::vec4;
        auto trilinearInterpolation(float x, float y, float level) -> glm::vec4;
        auto getMipMapLevel(float dsx, float dsy, float dtx, float dty) -> float;
        auto nearestNeighbour(float x, float y) -> glm::vec4;
};
