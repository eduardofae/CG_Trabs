#pragma once

enum VAO_IDs    { OpenGL, CloseGL, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NormalBuffer, AmbientBuffer, DiffuseBuffer, SpecularBuffer, ShineBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0, vNormal = 1, vAmbient = 2, vDiffuse = 3, vSpecular = 4, vShine = 5 };

enum oType {
    point    = 0,
    line     = 1,
    triangle = 2
};

enum orders {
    cw  = 0,
    ccw = 1
};

enum cStyle {
    camLookAt = 0,
    camFree   = 1
};

enum pType {
    perspective = 0,
    ortographic = 1
};

enum rType {
    openGL  = 0,
    closeGL = 1
};

enum sType {
    GouAD  = 0,
    GouADS = 1,
    Phong  = 2,
    numShadingTypes
};