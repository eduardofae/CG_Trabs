#include "openGL.hpp"

void renderOpenGL(GLuint program, glm::mat4 model, glm::mat4 view, glm::mat4 projection,
                  float *color, bool useColor, GLuint *VAOs, int VAOid,
                  int g_renderType, int g_windingOrder, int g_backFaceCulling, int size)
{
    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glClearBufferfv(GL_COLOR, 0, black);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUniformMatrix4fv(glGetUniformLocation(program, "model")      , 1 , GL_FALSE , glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(program, "view")       , 1 , GL_FALSE , glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection") , 1 , GL_FALSE , glm::value_ptr(projection));

    glUniform3fv(glGetUniformLocation(program, "color")            , 1            , color);
    glUniform1i(glGetUniformLocation(program, "useColor")          ,                useColor);

    glBindVertexArray(VAOs[VAOid]);

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