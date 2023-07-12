#include "closeGL.hpp"

void CloseToGL::buildCloseGL(GLuint *VAOs, GLuint *Buffers)
{
    glBindVertexArray(VAOs[CloseGL]);

    GLfloat pos[] = { 0.0f, 0.0f,
                      0.0f, 1.0f,
                      1.0f, 0.0f,
                      1.0f, 1.0f,
                      0.0f, 1.0f,
                      1.0f, 0.0f };

    glCreateBuffers(NumBuffers, Buffers);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, sizeof(pos), pos, 0);

    glVertexAttribPointer(vPosition, 2, GL_FLOAT,
        GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vPosition);

    glBindBuffer(GL_ARRAY_BUFFER, Buffers[TextureBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, sizeof(pos), pos, 0);

    glVertexAttribPointer(vTexture, 2, GL_FLOAT,
        GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vTexture);
}

void CloseToGL::renderCloseGL(GLuint program, glm::mat4 model, glm::mat4 view, glm::mat4 projection,
                   float *color, bool useColor, GLuint *VAOs,
                   int g_mashType, int g_windingOrder, int g_backFaceCulling, GLuint *Buffers,
                   std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<int> material_id,
                   int shadingType, glm::vec4 camera_position, std::vector <MaterialInfo> materials,
                   WindowSize windowSize)
{
    glUseProgram(program);

    glBindVertexArray(VAOs[CloseGL]);

    cleanBuffers(windowSize);

    glm::mat4 view_port = Matrix_View_Port(windowSize.width, windowSize.height);

    for(int i = 0; i < vertices.size(); i += 3){
        std::array<glm::vec4, 3> pixel_pos, pos, norm, color;
        std::array<int, 3> mat_id;
        bool valid = true;

        for(int j = 0; j < 3; j++)
            pos[j] = projection * view * model * glm::vec4(vertices.at(i+j), 1.0f);

        if(pos.at(0).w > 0 && pos.at(1).w > 0 && pos.at(2).w > 0)
            for(int j = 0; j < 3 || !valid; j++){
                glm::vec4 p = pos.at(j) /= pos.at(j).w;
                pos[j] = p;
                if(p.x > 1 || p.x < -1 || p.y > 1 || p.y < -1 || p.z > 1 || p.z < -1)
                    valid = false;
            }

        if(valid){
            for(int j = 0; j < 3; j++){
                pixel_pos[j] = view_port * pos.at(j);
                mat_id[j]    = material_id.at(i+j);
                norm[j]      = glm::vec4(normals.at(i+j), 0.0f);
                color[j]     = vertex(pos.at(j), mat_id.at(j), norm.at(j), shadingType, camera_position, materials);
            }            

            rasterize(pos, mat_id, norm, color, pixel_pos, shadingType, camera_position, materials, view_port);
        }
    }

    glDrawArrays(GL_TRIANGLES, 0, 2);
}

void CloseToGL::rasterize(std::array<glm::vec4, 3> pos, std::array<int, 3> mat_id,
                          std::array<glm::vec4, 3> norm, std::array<glm::vec4, 3> color,
                          std::array<glm::vec4, 3> pixel_pos,
                          int shadingType, glm::vec4 camera_position, WindowSize windowSize,
                          std::vector <MaterialInfo> materials, glm::mat4 view_port)
{
    std::array <glm::vec4, 3> edges;
    std::array <int, 3> incX, incZ;

    int minY = std::numeric_limits<int>::max(), maxY = std::numeric_limits<int>::min(),
        minX = std::numeric_limits<int>::max(), maxX = std::numeric_limits<int>::min();

    for(int i; i < 3; i++){
        glm::vec4 edge = pixel_pos[i] - pixel_pos[(i+1)%3];
        edges[i] = edge;
        incX[i]  = glm::round(edge.x / edge.y);
        incZ[i]  = glm::round(edge.z / edge.y);
        
        int y = glm::round(pixel_pos[i].y);
        int x = glm::round(pixel_pos[i].x);

        minY = std::min(minY, y);
        maxY = std::max(maxY, y);
        minX = std::min(minX, x);
        maxX = std::max(maxX, x);
    }
    
    for(int y = minY; y < maxY; y++){
        for(int x = minX; x < maxX; x++){

            //glm::vec4 cor = fragment(pos, mat_id, norm, color, shadingType, camera_position, materials);
            ColorBuffer.at((y * windowSize.width + x) * 4) = cor;
            ZBuffer.at(y * windowSize.width + x) = z;
        }
    }
}

glm::vec4 CloseToGL::vertex(glm::vec4 pos, int mat_id, glm::vec4 norm,
                              int shadingType, glm::vec4 camera_position,
                              std::vector <MaterialInfo> materials)
{
    MaterialInfo mat = materials.at(mat_id);

    switch(shadingType){
        case GouAD:
            break;
        case GouADS:
            break;
    }

    return glm::vec4(mat.diffuse, 1.0f);
}

glm::vec4 CloseToGL::fragment(glm::vec4 pos, int mat_id, glm::vec4 norm, glm::vec4 color,
                              int shadingType, glm::vec4 camera_position,
                              std::vector <MaterialInfo> materials)
{
    switch(shadingType){
        case Phong:
            MaterialInfo mat = materials.at(mat_id);
            break;
    }

    return color;
}

void CloseToGL::cleanBuffers(WindowSize windowSize){
    ColorBuffer.resize(windowSize.width * windowSize.height * 4);
    ZBuffer.resize(windowSize.width * windowSize.height);
    std::fill(ColorBuffer.begin(), ColorBuffer.end(), 0.0f);
    std::fill(ZBuffer.begin(), ZBuffer.end(), std::numeric_limits<float>::max());
}