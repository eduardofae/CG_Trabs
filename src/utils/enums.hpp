#pragma once

enum VAO_IDs    { OpenGL, CloseGL, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NormalBuffer, materialIdBuffer, NumBuffers };
enum Attrib_IDs { vPosition, vNormal, vMaterialId };

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

typedef struct {
    glm::vec3   ambient;
    glm::vec3   diffuse;
    glm::vec3   specular;
    float       shine;
} MaterialInfo;