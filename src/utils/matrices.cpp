#include "matrices.hpp"

glm::mat4 Matrix(
    float m00, float m01, float m02, float m03, // LINHA 1
    float m10, float m11, float m12, float m13, // LINHA 2
    float m20, float m21, float m22, float m23, // LINHA 3
    float m30, float m31, float m32, float m33  // LINHA 4
)
{
    return glm::mat4(
        m00, m10, m20, m30, // COLUNA 1
        m01, m11, m21, m31, // COLUNA 2
        m02, m12, m22, m32, // COLUNA 3
        m03, m13, m23, m33  // COLUNA 4
    );
}

// Matriz identidade.
glm::mat4 Matrix_Identity()
{
    return Matrix(
        1.0f , 0.0f , 0.0f , 0.0f , // LINHA 1
        0.0f , 1.0f , 0.0f , 0.0f , // LINHA 2
        0.0f , 0.0f , 1.0f , 0.0f , // LINHA 3
        0.0f , 0.0f , 0.0f , 1.0f   // LINHA 4
    );
}

// Matriz de translação T.
glm::mat4 Matrix_Translate(float tx, float ty, float tz)
{
    return Matrix(
        1.0f , 0.0f , 0.0f ,  tx  ,  // LINHA 1
        0.0f , 1.0f , 0.0f ,  ty  ,  // LINHA 2
        0.0f , 0.0f , 1.0f ,  tz  ,  // LINHA 3
        0.0f , 0.0f , 0.0f , 1.0f    // LINHA 4
    );
}

// Matriz S de "escalamento de um ponto" em relação à origem do sistema de coordenadas.
glm::mat4 Matrix_Scale(float sx, float sy, float sz)
{
    return Matrix(
         sx  , 0.0f , 0.0f , 0.0f ,  // LINHA 1
        0.0f ,  sy  , 0.0f , 0.0f ,  // LINHA 2
        0.0f , 0.0f ,  sz  , 0.0f ,  // LINHA 3
        0.0f , 0.0f , 0.0f , 1.0f    // LINHA 4
    );
}

// Matriz R de "rotação de um ponto" em relação à origem do sistema de
// coordenadas e em torno do eixo X.
glm::mat4 Matrix_Rotate_X(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return Matrix(
        1.0f , 0.0f , 0.0f , 0.0f ,  // LINHA 1
        0.0f ,  c   ,  -s  , 0.0f ,  // LINHA 2
        0.0f ,  s   ,  c   , 0.0f ,  // LINHA 3
        0.0f , 0.0f , 0.0f , 1.0f    // LINHA 4
    );
}

// Matriz R de "rotação de um ponto" em relação à origem do sistema de
// coordenadas e em torno do eixo Y.
glm::mat4 Matrix_Rotate_Y(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return Matrix(
         c   , 0.0f ,  s   , 0.0f ,  // LINHA 1
        0.0f , 1.0f , 0.0f , 0.0f ,  // LINHA 2
         -s  , 0.0f ,  c   , 0.0f ,  // LINHA 3
        0.0f , 0.0f , 0.0f , 1.0f    // LINHA 4
    );
}

// Matriz R de "rotação de um ponto" em relação à origem do sistema de
// coordenadas e em torno do eixo Z.
glm::mat4 Matrix_Rotate_Z(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return Matrix(
         c   ,  -s  , 0.0f , 0.0f ,  // LINHA 1
         s   ,  c   , 0.0f , 0.0f ,  // LINHA 2
        0.0f , 0.0f , 1.0f , 0.0f ,  // LINHA 3
        0.0f , 0.0f , 0.0f , 1.0f    // LINHA 4
    );
}

// Norma Euclidiana de um vetor cujos coeficientes são
// definidos em uma base ortonormal qualquer.
float norm(glm::vec4 v)
{
    float vx = v.x;
    float vy = v.y;
    float vz = v.z;

    return sqrt( vx*vx + vy*vy + vz*vz );
}

// Normaliza um vetor
glm::vec4 normalize(glm::vec4 v)
{
    return v / norm(v);
}

// Matriz R de "rotação de um ponto" em relação à origem do sistema de
// coordenadas e em torno do eixo definido pelo vetor 'axis'.
glm::mat4 Matrix_Rotate(float angle, glm::vec4 axis)
{
    float c = cos(angle);
    float s = sin(angle);

    glm::vec4 v = normalize(axis);

    float vx = v.x;
    float vy = v.y;
    float vz = v.z;

    return Matrix(
        vx*vx*(1-c) + c    , vx*vy*(1-c) - vz*s , vx*vz*(1-c) + vy*s , 0.0f ,  // LINHA 1
        vx*vy*(1-c) + vz*s , vy*vy*(1-c) + c    , vy*vz*(1-c) - vx*s , 0.0f ,  // LINHA 2
        vx*vz*(1-c) - vy*s , vy*vz*(1-c) + vx*s , vz*vz*(1-c) + c    , 0.0f ,  // LINHA 3
              0.0f         ,       0.0f         ,       0.0f         , 1.0f    // LINHA 4
    );
}

// Produto vetorial entre dois vetores u e v definidos em um sistema de
// coordenadas ortonormal.
glm::vec4 crossproduct(glm::vec4 u, glm::vec4 v)
{
    float u1 = u.x;
    float u2 = u.y;
    float u3 = u.z;
    float v1 = v.x;
    float v2 = v.y;
    float v3 = v.z;

    return glm::vec4(
        u2*v3 - u3*v2, // Primeiro coeficiente
        u3*v1 - u1*v3, // Segundo coeficiente
        u1*v2 - u2*v1, // Terceiro coeficiente
        0.0f // w = 0 para vetores.
    );
}

// Produto escalar entre dois vetores u e v definidos em um sistema de
// coordenadas ortonormal.
float dotproduct(glm::vec4 u, glm::vec4 v)
{
    float u1 = u.x;
    float u2 = u.y;
    float u3 = u.z;
    float u4 = u.w;
    float v1 = v.x;
    float v2 = v.y;
    float v3 = v.z;
    float v4 = v.w;

    if ( u4 != 0.0f || v4 != 0.0f )
    {
        fprintf(stderr, "ERROR: Produto escalar não definido para pontos.\n");
        std::exit(EXIT_FAILURE);
    }

    return u1*v1 + u2*v2 + u3*v3 + u4*v4;
}

// Produto vetorial entre dois vetores u e v definidos em um sistema de
// coordenadas ortonormal.
glm::vec3 crossproduct(glm::vec3 u, glm::vec3 v)
{
    float u1 = u.x;
    float u2 = u.y;
    float u3 = u.z;
    float v1 = v.x;
    float v2 = v.y;
    float v3 = v.z;

    return glm::vec3(
        u2*v3 - u3*v2, // Primeiro coeficiente
        u3*v1 - u1*v3, // Segundo coeficiente
        u1*v2 - u2*v1  // Terceiro coeficiente
    );
}

// Produto escalar entre dois vetores u e v definidos em um sistema de
// coordenadas ortonormal.
float dotproduct(glm::vec3 u, glm::vec3 v)
{
    float u1 = u.x;
    float u2 = u.y;
    float u3 = u.z;
    float v1 = v.x;
    float v2 = v.y;
    float v3 = v.z;

    return u1*v1 + u2*v2 + u3*v3;
}

// Matriz de mudança de coordenadas para o sistema de coordenadas da Câmera.
glm::mat4 Matrix_Camera_View(glm::vec4 position_c, glm::vec4 view_vector, glm::vec4 up_vector)
{
    glm::vec4 w = -view_vector;
    glm::vec4 u = crossproduct(up_vector,w);

    w = normalize(w);
    u = normalize(u);

    glm::vec4 v = crossproduct(w,u);

    glm::vec4 origin_o = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    float ux = u.x;
    float uy = u.y;
    float uz = u.z;
    float vx = v.x;
    float vy = v.y;
    float vz = v.z;
    float wx = w.x;
    float wy = w.y;
    float wz = w.z;

    return Matrix(
         ux  ,  uy  ,  uz  , -dotproduct(u, position_c - origin_o) ,
         vx  ,  vy  ,  vz  , -dotproduct(v, position_c - origin_o) ,
         wx  ,  wy  ,  wz  , -dotproduct(w, position_c - origin_o) ,
        0.0f , 0.0f , 0.0f ,                 1.0f                   
    );
}

// Matriz de projeção paralela ortográfica
glm::mat4 Matrix_Orthographic(float l, float r, float b, float t, float n, float f)
{
    glm::mat4 M = Matrix(
        2.0f/(r-l) ,    0.0f    ,    0.0f    , -(r+l)/(r-l) ,
           0.0f    , 2.0f/(t-b) ,    0.0f    , -(t+b)/(t-b) ,
           0.0f    ,    0.0f    , 2.0f/(f-n) , -(f+n)/(f-n) ,
           0.0f    ,    0.0f    ,    0.0f    ,      1.0f     
    );

    return M;
}

// Matriz de projeção perspectiva
glm::mat4 Matrix_Perspective(float field_of_view, float aspect, float n, float f)
{
    float t = fabs(n) * tanf(field_of_view / 2.0f);
    float b = -t;
    float r = t * aspect;
    float l = -r;

    glm::mat4 P = Matrix(
         n   , 0.0f , 0.0f , 0.0f ,  // LINHA 1
        0.0f ,  n   , 0.0f , 0.0f ,  // LINHA 2
        0.0f , 0.0f ,  n+f , -f*n ,  // LINHA 3
        0.0f , 0.0f , 1.0f , 0.0f    // LINHA 4
    );

    glm::mat4 M = Matrix_Orthographic(l, r, b, t, n, f);
    
    return -M*P;
}

// Matriz de projeção perspectiva
glm::mat4 Matrix_Frustum(float FOVv, float FOVh, float n, float f)
{
    float t = fabs(n) * tanf(FOVv / 2.0f);
    float b = -t;
    float r = fabs(n) * tanf(FOVh / 2.0f);
    float l = -r;

    glm::mat4 P = Matrix(
         n   , 0.0f , 0.0f , 0.0f ,  // LINHA 1
        0.0f ,  n   , 0.0f , 0.0f ,  // LINHA 2
        0.0f , 0.0f ,  n+f , -f*n ,  // LINHA 3
        0.0f , 0.0f , 1.0f , 0.0f    // LINHA 4
    );

    glm::mat4 M = Matrix_Orthographic(l, r, b, t, n, f);
    
    return -M*P;
}

glm::mat4 Matrix_View_Port(float width, float height){
    return Matrix(
        width/2,    0    , 0, width/2  , // LINHA 1
           0   , height/2, 0, height/2 , // LINHA 2
           0   ,    0    , 1,    0     , // LINHA 3
           0   ,    0    , 0,    1       // LINHA 4
    );
}
