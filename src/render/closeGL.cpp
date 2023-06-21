#include "closeGL.hpp"

int buildCloseGL(GLuint *VAOs, GLuint *Buffers, std::vector<GLfloat> vertices, std::vector<GLfloat> normals, std::vector<GLfloat> colors,
                  glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    glBindVertexArray(VAOs[CloseGL]);

    std::vector<GLfloat> position, finalNormals, finalColors;

    for(int i = 0; i < vertices.size(); i += 9){
        std::array<glm::vec4, 3> points;
        for(int j = 0; j < 3; j++)
            points[j] = projection * view * model * glm::vec4(vertices.at(i+3*j), vertices.at(i+3*j+1), vertices.at(i+3*j+2), 1.0f);
        if(points.at(0).w > 0 && points.at(1).w > 0 && points.at(2).w > 0){
            for(int j = 0; j < 3; j++){
                points[j] /= points.at(j).w;
                position.emplace_back(points.at(j).x);
                position.emplace_back(points.at(j).y);
                position.emplace_back(points.at(j).z);

                finalNormals.emplace_back(normals.at(i+3*j));
                finalNormals.emplace_back(normals.at(i+3*j+1));
                finalNormals.emplace_back(normals.at(i+3*j+2));

                finalColors.emplace_back(colors.at(i+3*j));
                finalColors.emplace_back(colors.at(i+3*j+1));
                finalColors.emplace_back(colors.at(i+3*j+2));
            }
        }
    }

    glCreateBuffers(NumBuffers, Buffers);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, position.size()*sizeof(GLfloat), position.data(), 0);

    glVertexAttribPointer(vPosition, 3, GL_FLOAT,
        GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vPosition);

    glBindBuffer(GL_ARRAY_BUFFER, Buffers[NormalBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, finalNormals.size()*sizeof(GLfloat), finalNormals.data(), 0);

    glVertexAttribPointer(vNormal, 3, GL_FLOAT,
        GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vNormal);

    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ColorBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, finalColors.size()*sizeof(GLfloat), finalColors.data(), 0);

    glVertexAttribPointer(vColor, 3, GL_FLOAT,
        GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vColor);

    return position.size();
}

void renderCloseGL(GLuint program, glm::mat4 model, glm::mat4 view, glm::mat4 projection,
                   float *color, bool useColor, GLuint *VAOs,
                   int g_mashType, int g_windingOrder, int g_backFaceCulling,
                   GLuint *Buffers, std::vector<GLfloat> vertices, std::vector<GLfloat> normals, std::vector<GLfloat> colors)
{
    glUseProgram(program);

    int size = buildCloseGL(VAOs, Buffers, vertices, normals, colors, model, view, projection);

    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glClearBufferfv(GL_COLOR, 0, black);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUniform3fv(glGetUniformLocation(program, "color")  , 1, color);
    glUniform1i(glGetUniformLocation(program, "useColor"),    useColor);

    glBindVertexArray(VAOs[CloseGL]);

    switch (g_mashType){
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