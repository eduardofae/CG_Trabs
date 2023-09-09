#pragma once
#include <vector>
#include <array>

enum VAO_IDs    { OpenGL, CloseGL, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NormalBuffer, materialIdBuffer, TextureBuffer, NumBuffers };
enum Attrib_IDs { vPosition, vNormal, vMaterialId, vTexture };

enum oType {
    point,
    line,
    triangle
};

enum orders {
    cw,
    ccw
};

enum cStyle {
    camLookAt,
    camFree
};

enum pType {
    perspective,
    ortographic
};

enum rType {
    openGL,
    closeGL
};

enum sType {
    GouAD,
    GouADS,
    Phong,
    NoneVert,
    NoneFrag,
    numShadingTypes
};

enum samplingType {
    nearNeig,
    bilinear,
    trilinear
};

typedef struct {
    glm::vec3   ambient;
    glm::vec3   diffuse;
    glm::vec3   specular;
    float       shine;
} MaterialInfo;

typedef struct {
	int width, height;
} WindowSize;

typedef struct {
    std::vector<glm::vec3> position;
    std::vector<glm::vec3> normal;
    std::vector<glm::vec3> face_normal;
    std::vector<glm::vec2> texture_coords;
    std::vector<int> material_id;
    std::vector<MaterialInfo> materialInfos;
    glm::vec3 center;
    bool texture;
} ObjectInfo;

typedef struct {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 invModelView;
} Matrices;

typedef struct {
    int width;
    int height;
    int channels;
    unsigned char *data;
} TextureInfo;

