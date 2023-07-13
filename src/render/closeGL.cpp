#include "closeGL.hpp"

void CloseToGL::buildCloseGL(GLuint *VAOs, GLuint *Buffers)
{
    glBindVertexArray(VAOs[CloseGL]);

    float width  = (float) windowSize.width,
          height = (float) windowSize.height;
    GLfloat pos[] = { 0.0f ,   0.0f ,
                      0.0f , -height,
                      width,   0.0f ,
                      width, -height,
                      0.0f , -height,
                      width,   0.0f };


    GLfloat text[] = { 0.0f, 0.0f,
                       0.0f, 1.0f,
                       1.0f, 0.0f,
                       1.0f, 1.0f,
                       0.0f, 1.0f,
                       1.0f, 0.0f };

    glCreateBuffers(NumBuffers, Buffers);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), pos, 0);

    glVertexAttribPointer(vPosition, 2, GL_FLOAT,
        GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vPosition);

    glBindBuffer(GL_ARRAY_BUFFER, Buffers[TextureBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), text, 0);

    glVertexAttribPointer(vTexture, 2, GL_FLOAT,
        GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vTexture);

    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA, width, height);
}

void CloseToGL::renderCloseGL(GLuint program, Matrices matrices,
                              float *color, bool useColor, GLuint *VAOs,
                              int g_mashType, int g_windingOrder, int g_backFaceCulling,
                              ObjectInfo Obj, int shadingType, glm::vec4 camera_position)
{
    glUseProgram(program);

    glBindVertexArray(VAOs[CloseGL]);

    cleanBuffers();

    setShaderInfo(Obj, color, useColor, shadingType, camera_position);

    CullingInfo cullingInfo = { g_windingOrder, g_backFaceCulling };

    backFaceCulling(Obj, matrices, cullingInfo);

    linkTexture(program);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void CloseToGL::setShaderInfo(ObjectInfo Obj, float *color, bool useColor, int shadingType, glm::vec4 camera_position)
{
    glm::vec3 cor = glm::vec3(color[0], color[1], color[2]);
    for(auto &mat : Obj.materialInfos)
        mat.diffuse = useColor ? cor : mat.diffuse;
    this->shader = { shadingType, camera_position, Obj.materialInfos };
}

void CloseToGL::linkTexture(GLuint program)
{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, windowSize.width,     
          windowSize.height, GL_RGBA, GL_FLOAT,  
          ColorBuffer.data());

    glUniform1i(glGetUniformLocation(program, "text"), 0);
}

void CloseToGL::backFaceCulling(ObjectInfo Obj, Matrices matrices, CullingInfo cullingInfo)
{
    for(int i = 0; i < Obj.position.size(); i += 3){
        bool valid = true;

        if(cullingInfo.backFaceCulling){
            switch(cullingInfo.windingOrder){
                case cw : 
                    if(false)
                        valid = false;
                    break;
                case ccw :
                    if(false)
                        valid = false;
            }
        }

        if(valid){
            frustrumCulling(Obj, matrices, i);
        }

    }
}

void CloseToGL::frustrumCulling(ObjectInfo Obj, Matrices matrices, int i)
{
    std::array<PointInfo, 3> vertices;
    std::array<glm::vec4, 3> pos;
    for(int j = 0; j < 3; j++){
        pos[j] = matrices.view * matrices.model * glm::vec4(Obj.position.at(i+j), 1.0f);
        vertices[j].pos = pos.at(j);
        pos[j] = matrices.proj * pos.at(j);
    }   
    
    bool valid = true;
    if(pos.at(0).w != 0 && pos.at(1).w != 0 && pos.at(2).w != 0){
        for(int j = 0; j < 3 && valid; j++){
            glm::vec4 p = pos.at(j) /= pos.at(j).w;
            if(p.x > 1 || p.x < -1 || p.y > 1 || p.y < -1 || p.z > 1 || p.z < -1)
                valid = false;
            else
                pos[j] = p;
        }
    }

    if(valid){
        for(int j = 0; j < 3; j++){
            vertices[j].pixel_pos = view_port * pos.at(j);
            vertices[j].mat_id    = Obj.material_id.at(i+j);
            vertices[j].norm      = glm::transpose(glm::inverse(matrices.model)) * glm::vec4(Obj.normal.at(i+j), 0.0f);
            vertices[j].color     = vertex(vertices.at(j));
        }        
        rasterize(vertices);
    }
}

void CloseToGL::rasterize(std::array<PointInfo, 3> vertices)
{
    for(int i = 1; i < 3; i++)
        if(vertices[i].pixel_pos.y > vertices[0].pixel_pos.y)
            std::swap(vertices[2], vertices[1]);

    if(vertices[2].pixel_pos.y > vertices[1].pixel_pos.y)
        std::swap(vertices[2], vertices[1]);
    
    if(vertices[1].pixel_pos.y == vertices[2].pixel_pos.y){
        drawTriangle(vertices, Top);
        return;
    }

    if(vertices[0].pixel_pos.y == vertices[1].pixel_pos.y){
        drawTriangle(vertices, Bot);
        return;
    }
    
    std::array<PointInfo, 2> verts;
    verts[0] = vertices.at(0);
    verts[1] = vertices.at(2);

    glm::vec4 edgeM, edgeB;
    edgeM = vertices.at(1).pixel_pos - verts.at(0).pixel_pos;
    edgeB = verts.at(1).pixel_pos - verts.at(0).pixel_pos;
    
    PointInfo interpolatedVert = interpolate(verts, edgeM.y / edgeB.y);

    std::array<PointInfo, 3> topVert = { vertices[0], vertices[1], interpolatedVert },
                             botVert = { vertices[2], vertices[1], interpolatedVert };

    drawTriangle(topVert, Top);
    drawTriangle(botVert, Bot);
}

void CloseToGL::drawTriangle(std::array<PointInfo, 3> vertices, int type)
{
    float deltaY = vertices[1].pixel_pos.y - vertices[0].pixel_pos.y;

    if(vertices[1].pixel_pos.x > vertices[2].pixel_pos.x)
        std::swap(vertices[1], vertices[2]);

    for(int y = std::ceil(vertices[0].pixel_pos.y); y > std::ceil(vertices[1].pixel_pos.y); y += type == Top ? -1 : 1){
        std::array<PointInfo, 2> points, aux;

        aux = { vertices[0], vertices[1] };
        points[0] = interpolate(aux, y/deltaY);
        aux = { vertices[0], vertices[2] };
        points[1] = interpolate(aux, y/deltaY);
        
        scanline(points, y);
    }
}

void CloseToGL::scanline(std::array<PointInfo, 2> points, int y)
{
    glm::vec4 &left  = points.at(0).pixel_pos, 
              &right = points.at(1).pixel_pos;
    float deltaX = (right - left).x;

    for(int x = left.x; x < right.x; x++){
        PointInfo frag = interpolate(points, x / deltaX);
        frag.color = fragment(frag);        
        fillBuffers(frag, x, y);
    }
}

PointInfo CloseToGL::interpolate(std::array<PointInfo, 2> points, float t)
{
    PointInfo &pointT = points.at(0), &pointB = points.at(1), point;

    point.color     = pointT.color     * t + pointB.color     * (1 - t);
    point.mat_id    = pointT.mat_id    * t + pointB.mat_id    * (1 - t);
    point.norm      = pointT.norm      * t + pointB.norm      * (1 - t);
    point.pos       = pointT.pos       * t + pointB.pos       * (1 - t);
    point.pixel_pos = pointT.pixel_pos * t + pointB.pixel_pos * (1 - t);

    return point;
}

void CloseToGL::fillBuffers(PointInfo frag, int x, int y)
{
    int pos = y * windowSize.width + x;
    if(frag.pixel_pos.z < ZBuffer.at(pos)){
        ZBuffer[pos] = frag.pixel_pos.z;
        for(int i = 0; i < 4; i++)
            ColorBuffer[4*pos+i] = frag.color[i];
    }   
}

void CloseToGL::cleanBuffers()
{
    std::fill(ColorBuffer.begin(), ColorBuffer.end(), 0.0f);
    std::fill(ZBuffer.begin(), ZBuffer.end(), std::numeric_limits<float>::max());
}

void CloseToGL::updateWindowSize(WindowSize windowSize)
{
    this->windowSize.width  = windowSize.width;
    this->windowSize.height = windowSize.height;

    ColorBuffer.resize(windowSize.width * windowSize.height * 4);
    ZBuffer.resize(windowSize.width * windowSize.height);

    view_port = Matrix_View_Port((float) windowSize.width, (float) windowSize.height);
}

glm::vec4 CloseToGL::vertex(PointInfo vertex)
{
    MaterialInfo mat = shader.materials.at(vertex.mat_id);

    glm::vec3 I  = glm::vec3(1.0, 1.0, 1.0);
    glm::vec3 Ia = glm::vec3(0.2, 0.2, 0.2);

    glm::vec4 n  = normalize(vertex.norm);
    glm::vec4 l  = normalize(shader.camera_position - vertex.pos);
    n.w = 0.0f;
    l.w = 0.0f;

    glm::vec3 lambert_diffuse_term, ambient_term, phong_specular_term;

    switch(shader.shadingType){
        case GouAD:
            lambert_diffuse_term = mat.diffuse*I*std::max(0.0f,dotproduct(n,l));
            ambient_term         = mat.ambient*Ia;

            return glm::vec4(lambert_diffuse_term + ambient_term, 1.0f);
        case GouADS:
            glm::vec4 v = l;
            glm::vec4 r = -l + 2.0f*n*dotproduct(n,l);

            lambert_diffuse_term = mat.diffuse*I*std::max(0.0f,dotproduct(n,l));
            ambient_term         = mat.ambient*Ia;
            phong_specular_term  = mat.specular*I*std::pow(std::max(0.0f,dotproduct(r,v)),mat.shine);

            return glm::vec4(lambert_diffuse_term + ambient_term + phong_specular_term, 1.0f);
    }
    return glm::vec4(mat.diffuse, 1.0f);
}

glm::vec4 CloseToGL::fragment(PointInfo fragment)
{
    switch(shader.shadingType){
        case Phong:
            MaterialInfo mat = shader.materials.at(fragment.mat_id);

            glm::vec3 I  = glm::vec3(1.0, 1.0, 1.0);
            glm::vec3 Ia = glm::vec3(0.2, 0.2, 0.2);

            glm::vec4 n  = normalize(fragment.norm);
            glm::vec4 l  = normalize(shader.camera_position - fragment.pos);
            n.w = 0.0f;
            l.w = 0.0f;

            glm::vec4 v = l;
            glm::vec4 r = -l + 2.0f*n*dotproduct(n,l);

            glm::vec3 lambert_diffuse_term = mat.diffuse*I*std::max(0.0f,dotproduct(n,l));
            glm::vec3 ambient_term         = mat.ambient*Ia;
            glm::vec3 phong_specular_term  = mat.specular*I*std::pow(std::max(0.0f,dotproduct(r,v)),mat.shine);

            return glm::vec4(lambert_diffuse_term + ambient_term + phong_specular_term, 1.0f);
    }
    return fragment.color;
}