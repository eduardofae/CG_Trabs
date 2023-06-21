#pragma once

enum VAO_IDs    { OpenGL, CloseGL, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NormalBuffer, ColorBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0, vNormal = 1, vColor = 2 };

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
    openGL = 0,
    closeGL = 1
};