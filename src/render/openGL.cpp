#include "openGL.hpp"

void buildOpenGL(GLuint *VAOs, GLuint *Buffers, std::vector<GLfloat> vertices, std::vector<GLfloat> normals, std::vector<GLfloat> colors) {
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

    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ColorBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, colors.size()*sizeof(GLfloat), colors.data(), 0);

    glVertexAttribPointer(vColor, 3, GL_FLOAT,
        GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vColor);
}

void renderOpenGL(GLuint program, glm::mat4 model, glm::mat4 view, glm::mat4 projection,
                  float *color, bool useColor, GLuint *VAOs,
                  int g_renderType, int g_windingOrder, int g_backFaceCulling, int size)
{
    glUseProgram(program);

    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glClearBufferfv(GL_COLOR, 0, black);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUniformMatrix4fv(glGetUniformLocation(program, "model")      , 1 , GL_FALSE , glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(program, "view")       , 1 , GL_FALSE , glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection") , 1 , GL_FALSE , glm::value_ptr(projection));

    glUniform3fv(glGetUniformLocation(program, "color")            , 1            , color);
    glUniform1i(glGetUniformLocation(program, "useColor")          ,                useColor);

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