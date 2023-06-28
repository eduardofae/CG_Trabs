#include "openGL.hpp"

void buildOpenGL(GLuint *VAOs, GLuint *Buffers, std::vector<GLfloat> vertices, std::vector<GLfloat> normals, std::vector<GLint> material_id,
                 std::vector<GLfloat> ambient, std::vector<GLfloat> diffuse, std::vector<GLfloat> specular, std::vector<GLfloat> shine)
{
    glBindVertexArray(VAOs[OpenGL]);

    glCreateBuffers(NumBuffers, Buffers);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), vertices.data(), 0);

    glVertexAttribPointer(vPosition, 3, GL_FLOAT,
        GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vPosition);

    glBindBuffer(GL_ARRAY_BUFFER, Buffers[NormalBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, normals.size()*sizeof(GLfloat), normals.data(), 0);

    glVertexAttribPointer(vNormal, 3, GL_FLOAT,
        GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vNormal);

    glBindBuffer(GL_ARRAY_BUFFER, Buffers[materialIdBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, material_id.size()*sizeof(GLint), material_id.data(), 0);

    glVertexAttribPointer(vMaterialId, 1, GL_INT,
        GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vMaterialId);
}

void renderOpenGL(GLuint program, glm::mat4 model, glm::mat4 view, glm::mat4 projection,
                  float *color, bool useColor, GLuint *VAOs,
                  int g_renderType, int g_windingOrder, int g_backFaceCulling, int size,
                  int shadingType, GLuint *lightModels, glm::vec4 camera_position,
                  std::vector <MaterialInfo> materials)
{
    glUseProgram(program);

    switch (shadingType)
    {
        case GouAD:
            glUniformSubroutinesuiv(GL_VERTEX_SHADER  , 1, &lightModels[shadingType]);
            glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &lightModels[NoneFrag]);
            break;
        case GouADS:
            glUniformSubroutinesuiv(GL_VERTEX_SHADER  , 1, &lightModels[shadingType]);
            glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &lightModels[NoneFrag]);
            break;
        case Phong:
            glUniformSubroutinesuiv(GL_VERTEX_SHADER  , 1, &lightModels[NoneVert]);
            glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &lightModels[shadingType]);
            break;
        default:
            glUniformSubroutinesuiv(GL_VERTEX_SHADER  , 1, &lightModels[NoneVert]);
            glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &lightModels[NoneFrag]);
            break;
    }
    

    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glClearBufferfv(GL_COLOR, 0, black);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUniformMatrix4fv(glGetUniformLocation(program, "model")             , 1 , GL_FALSE , glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(program, "view")              , 1 , GL_FALSE , glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection")        , 1 , GL_FALSE , glm::value_ptr(projection));

    glUniform4fv(glGetUniformLocation(program, "camera_position")         , 1            , glm::value_ptr(camera_position));
    glUniform3fv(glGetUniformLocation(program, "materials.ambient")       , 1            , glm::value_ptr(materials[0].ambient));
    if(useColor)
        glUniform3fv(glGetUniformLocation(program, "materials.diffuse")   , 1            , color);
    else
        glUniform3fv(glGetUniformLocation(program, "materials.diffuse")   , 1            , glm::value_ptr(materials[0].diffuse));
    glUniform3fv(glGetUniformLocation(program, "materials.specular")      , 1            , glm::value_ptr(materials[0].specular));
    glUniform1f(glGetUniformLocation(program, "materials.shine")                         , materials[0].shine);

    glBindVertexArray(VAOs[OpenGL]);

    switch (g_renderType){
        case point:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;

        case line:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;

        case triangle:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (g_windingOrder == cw) glFrontFace(GL_CW);
    else glFrontFace(GL_CCW);

    if(g_backFaceCulling) glEnable(GL_CULL_FACE); 
    else glDisable(GL_CULL_FACE);

    glDrawArrays(GL_TRIANGLES, 0, size);
}