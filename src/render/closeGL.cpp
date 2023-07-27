#include "closeGL.hpp"

void CloseToGL::buildCloseGL(GLuint *VAOs, GLuint *Buffers)
{
    glBindVertexArray(VAOs[CloseGL]);

    GLfloat pos[] = {  1.0f,   -1.0f,
                      -1.0f,    1.0f,
                      -1.0f,   -1.0f, 
                       1.0f,    1.0f,
                      -1.0f,    1.0f,
                       1.0f,   -1.0f };


    GLfloat text[] = { 1.0f, 0.0f,
                       0.0f, 1.0f,
                       0.0f, 0.0f,                       
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
}

void CloseToGL::renderCloseGL(GLuint program, Matrices matrices,
                              float *color, bool useColor, GLuint *VAOs,
                              int g_mashType, int g_windingOrder, int g_backFaceCulling,
                              ObjectInfo Obj, int shadingType, glm::vec4 camera_position,
                              TextureInfo &texture)
{
    glUseProgram(program);

    glBindVertexArray(VAOs[CloseGL]);

    cleanBuffers();

    setShaderInfo(Obj, color, useColor, shadingType, camera_position, g_mashType);

    CullingInfo cullingInfo = { g_windingOrder, g_backFaceCulling };

    drawImage(Obj, matrices, cullingInfo, texture);

    linkTexture(program);

    glFrontFace(GL_CCW);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void CloseToGL::setShaderInfo(ObjectInfo &Obj, float *color, bool useColor, int shadingType, glm::vec4 camera_position, int mashType)
{
    glm::vec3 cor = glm::vec3(color[0], color[1], color[2]);
    for(auto &mat : Obj.materialInfos)
        mat.diffuse = useColor ? cor : mat.diffuse;
    this->shader = { shadingType, camera_position, Obj.materialInfos, mashType, Obj.texture };
}

void CloseToGL::linkTexture(GLuint program)
{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, windowSize.width,     
          windowSize.height, GL_RGBA, GL_UNSIGNED_BYTE,  
          ColorBuffer.data());
}

void CloseToGL::backFaceCulling(std::array<PointInfo, 3> vertices, CullingInfo cullingInfo, TextureInfo &texture)
{
    bool valid = true;
    glm::vec4 edge1, edge2, norm,
        view = { 0.0f, 0.0f, -1.0f, 0.0f };

    if(cullingInfo.backFaceCulling){
        edge1 = vertices.at(1).pos - vertices.at(0).pos;
        edge2 = vertices.at(2).pos - vertices.at(0).pos;
        switch(cullingInfo.windingOrder){
            case cw : 
                norm = crossproduct(edge1, edge2);
                if(dotproduct(norm, view) < 0)
                    valid = false;
                break;
            case ccw :
                norm = crossproduct(edge2, edge1);
                if(dotproduct(norm, view) < 0)
                    valid = false;
                break;
        }
    }

    if(valid)
        rasterize(vertices, texture);
}

void CloseToGL::drawImage(ObjectInfo &Obj, Matrices matrices, CullingInfo cullingInfo, TextureInfo &texture)
{
    const int size = Obj.position.size();
    std::array<PointInfo, 3> vertices;
    for(int i = 0; i < size; i += 3){
        for(int j = 0; j < 3; j++){
            vertices[j].pos = matrices.view * matrices.model * glm::vec4(Obj.position.at(i+j), 1.0f);
            vertices[j].pixel_pos = matrices.proj * vertices.at(j).pos;
        }
    
        bool valid = false;
        if(vertices.at(0).pixel_pos.w != 0 && vertices.at(1).pixel_pos.w != 0 && vertices.at(2).pixel_pos.w != 0){
            valid = true;
            glm::vec4 p;
            for(int j = 0; j < 3 && valid; j++){
                p = vertices.at(j).pixel_pos / vertices.at(j).pixel_pos.w;
                if(p.x > 1 || p.x < -1 || p.y > 1 || p.y < -1 || p.z > 1 || p.z < -1)
                    valid = false;
                else
                    vertices[j].pixel_pos = p;
            }
        }

        if(valid){
            for(int j = 0; j < 3; j++){
                vertices[j].pixel_pos = view_port * vertices.at(j).pixel_pos;
                vertices[j].mat_id    = Obj.material_id.at(i+j);
                vertices[j].norm      = matrices.invModelView * glm::vec4(Obj.normal.at(i+j), 0.0f);
                vertices[j].color     = vertex(vertices.at(j), texture);
            }   
            backFaceCulling(vertices, cullingInfo, texture);
        }
    }
}

void CloseToGL::rasterize(std::array<PointInfo, 3> vertices, TextureInfo &texture)
{
    for(auto &vert : vertices){
        vert.w            = 1/vert.pixel_pos.w;
        vert.color       *= vert.w;
        vert.pixel_pos   *= vert.w;
        vert.norm        *= vert.w;
        vert.pos         *= vert.w;
        vert.mat_id      *= vert.w;
        vert.text_coords *= vert.w;
    }

    for(int i = 1; i < 3; i++)
        if(vertices[i].pixel_pos.y > vertices[0].pixel_pos.y)
            std::swap(vertices[0], vertices[i]);

    if(vertices[2].pixel_pos.y > vertices[1].pixel_pos.y)
        std::swap(vertices[2], vertices[1]);

    assert(vertices[0].pixel_pos.y >= vertices[1].pixel_pos.y);
    assert(vertices[0].pixel_pos.y >  vertices[2].pixel_pos.y);
    assert(vertices[1].pixel_pos.y >= vertices[2].pixel_pos.y);

    if(shader.mashType == line){
        drawLine({ vertices.at(1), vertices.at(0) }, texture);
        drawLine({ vertices.at(2), vertices.at(0) }, texture);
        drawLine({ vertices.at(2), vertices.at(1) }, texture);
        return;
    }
    
    if(vertices[1].pixel_pos.y == vertices[2].pixel_pos.y){
        drawTopTriangle(vertices, texture);
        return;
    }

    if(vertices[0].pixel_pos.y == vertices[1].pixel_pos.y){
        std::swap(vertices[0], vertices[2]); 
        drawBotTriangle(vertices, texture);
        return;
    }

    PointInfo top, bot;
    top = vertices.at(0);
    bot = vertices.at(2);

    float edgeM, edgeB;
    edgeM = vertices.at(1).pixel_pos.y - top.pixel_pos.y;
    edgeB = bot.pixel_pos.y            - top.pixel_pos.y;

    PointInfo interpolatedVert = interpolate(bot, top, edgeM / edgeB);

    vertices[2] = interpolatedVert;
    drawTopTriangle(vertices, texture);
    
    vertices[0] = bot;
    drawBotTriangle(vertices, texture);
}

void CloseToGL::drawLine(std::array<PointInfo, 2> vertices, TextureInfo &texture){
    int  y0   = std::ceil(vertices.at(0).pixel_pos.y);
    int  y1   = std::ceil(vertices.at(1).pixel_pos.y);
    int  x0   = std::ceil(vertices.at(0).pixel_pos.x);
    int  x1   = std::ceil(vertices.at(1).pixel_pos.x);
    bool useY = std::fabs(y0 - y1) > std::fabs(x0 - x1);
    if(!useY && vertices.at(0).pixel_pos.x > vertices.at(1).pixel_pos.x){
        std::swap(vertices.at(0), vertices.at(1));
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int  dx   = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int  dy   = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
    int  err  = (dx>dy ? dx : -dy)/2, e2; 
    float inc = useY ? 1/(vertices.at(1).pixel_pos.y - vertices.at(0).pixel_pos.y) : 
                       1/(vertices.at(1).pixel_pos.x - vertices.at(0).pixel_pos.x);
    float acc = 0;
    Pixel p;
    PointInfo frag;
    for(;;){
        frag = interpolate(vertices.at(0), vertices.at(1), acc);

        frag.color     /= frag.w;
        frag.pixel_pos /= frag.w;
        frag.norm      /= frag.w;
        frag.pos       /= frag.w;
        frag.mat_id    /= frag.w;

        p = fragment(frag, texture);
        fillBuffers(p, x0, y0, frag.pixel_pos.z);
        acc += inc;

        if (x0==x1 && y0==y1) break;
        e2 = err;
        if (e2 >-dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

void CloseToGL::drawTopTriangle(std::array<PointInfo, 3> vertices, TextureInfo &texture)
{
    int start = std::ceil(vertices[2].pixel_pos.y),
        end   = std::ceil(vertices[0].pixel_pos.y);

    assert(vertices[0].pixel_pos.y > vertices[1].pixel_pos.y);
    assert(vertices[0].pixel_pos.y > vertices[2].pixel_pos.y);

    if(vertices[1].pixel_pos.x > vertices[2].pixel_pos.x)
        std::swap(vertices[1], vertices[2]);

    PointInfo left, right;
    for(int y = start; y < end; y++){
        float t = (y-vertices[2].pixel_pos.y)/(vertices[0].pixel_pos.y - vertices[2].pixel_pos.y);

        left  = interpolate(vertices[0], vertices[1], t);
        right = interpolate(vertices[0], vertices[2], t);
        
        scanline(left, right, y, texture);
    }
}

void CloseToGL::drawBotTriangle(std::array<PointInfo, 3> vertices, TextureInfo &texture)
{
    int start = std::ceil(vertices[0].pixel_pos.y),
        end   = std::ceil(vertices[2].pixel_pos.y);

    assert(vertices[0].pixel_pos.y > vertices[1].pixel_pos.y);
    assert(vertices[0].pixel_pos.y > vertices[2].pixel_pos.y);

    if(vertices[1].pixel_pos.x > vertices[2].pixel_pos.x)
        std::swap(vertices[1], vertices[2]);

    PointInfo left, right;
    for(int y = start; y < end; y++){
        float t = (y-vertices[0].pixel_pos.y) / (vertices[2].pixel_pos.y - vertices[0].pixel_pos.y);

        left  = interpolate(vertices[1], vertices[0], t);
        right = interpolate(vertices[2], vertices[0], t);

        scanline(left, right, y, texture);
    }
}

void CloseToGL::scanline(PointInfo left, PointInfo right, int y, TextureInfo &texture)
{
    int start = std::ceil(left.pixel_pos.x),
        end   = std::ceil(right.pixel_pos.x);

    PointInfo frag;
    Pixel p;
    for(int x = start; x < end; x++){
        float t = (x-left.pixel_pos.x) / (right.pixel_pos.x - left.pixel_pos.x);
        frag = interpolate(left, right, t);
        frag.color       /= frag.w;
        frag.pixel_pos   /= frag.w;
        frag.norm        /= frag.w;
        frag.pos         /= frag.w;
        frag.mat_id      /= frag.w;
        frag.text_coords /= frag.w;

        p = fragment(frag, texture);
        fillBuffers(p, x, y, frag.pixel_pos.z);
    }
}

PointInfo CloseToGL::interpolate(PointInfo top, PointInfo bot, float t)
{
    PointInfo point;
    point.mat_id      = top.mat_id      * t + bot.mat_id      * (1 - t);
    point.norm        = top.norm        * t + bot.norm        * (1 - t);
    point.pos         = top.pos         * t + bot.pos         * (1 - t);
    point.pixel_pos   = top.pixel_pos   * t + bot.pixel_pos   * (1 - t);
    point.text_coords = top.text_coords * t + bot.text_coords * (1 - t);
    point.w           = top.w           * t + bot.w           * (1 - t);
    return point;
}

void CloseToGL::fillBuffers(Pixel p, int x, int y, float z)
{
    assert(x < windowSize.width);
    assert(y < windowSize.height);
    assert(x >= 0);
    assert(y >= 0);

    const int pos = y * windowSize.width + x;
    assert(pos < ZBuffer.size());

    if(z < ZBuffer.at(pos)){
        ZBuffer[pos] = z;
        ColorBuffer[pos] = p;
    }
}

void CloseToGL::cleanBuffers()
{
    float val = std::numeric_limits<float>::max();
    int   qtd = windowSize.width*windowSize.height;
    ColorBuffer.assign(qtd, Pixel());
    ZBuffer.assign(qtd, val);
}

void CloseToGL::updateWindowSize(WindowSize windowSize)
{
    this->windowSize.width  = windowSize.width;
    this->windowSize.height = windowSize.height;

    ColorBuffer.resize(windowSize.width * windowSize.height * 4);
    ZBuffer.resize(windowSize.width * windowSize.height, std::numeric_limits<float>::max());

    view_port = Matrix_View_Port((float) windowSize.width, (float) windowSize.height);

    if(textureID)
        glDeleteTextures(1, &textureID);
    
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, windowSize.width, windowSize.height);
}

glm::vec4 CloseToGL::vertex(PointInfo vertex, TextureInfo &texture)
{
    MaterialInfo mat = shader.materials.at(vertex.mat_id);

    glm::vec3 I  = glm::vec3(1.0, 1.0, 1.0);
    glm::vec3 Ia = glm::vec3(0.2, 0.2, 0.2);

    glm::vec4 n  = normalize(vertex.norm);
    glm::vec4 l  = normalize(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) - vertex.pos);
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

Pixel CloseToGL::fragment(PointInfo fragment, TextureInfo &texture)
{
    switch(shader.shadingType){
        case Phong:
            MaterialInfo mat = shader.materials.at(fragment.mat_id);

            glm::vec3 I  = glm::vec3(1.0, 1.0, 1.0);
            glm::vec3 Ia = glm::vec3(0.2, 0.2, 0.2);

            glm::vec4 n  = normalize(fragment.norm);
            glm::vec4 l  = normalize(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) - fragment.pos);
            n.w = 0.0f;
            l.w = 0.0f;

            glm::vec4 v = l;
            glm::vec4 r = -l + 2.0f*n*dotproduct(n,l);

            glm::vec3 lambert_diffuse_term = mat.diffuse*I*std::max(0.0f,dotproduct(n,l));
            if(shader.hasTexture)
                lambert_diffuse_term = glm::vec3(texture.data[((int) std::round(fragment.text_coords.y) * texture.width + (int) std::round(fragment.text_coords.x)) * texture.channels], texture.data[((int) std::round(fragment.text_coords.y) * texture.width + (int) std::round(fragment.text_coords.x)) * texture.channels + 1], texture.data[((int) std::round(fragment.text_coords.y) * texture.width + (int) std::round(fragment.text_coords.x)) * texture.channels + 2])*I*std::max(0.0f,dotproduct(n,l));
            glm::vec3 ambient_term         = mat.ambient*Ia;
            glm::vec3 phong_specular_term  = mat.specular*I*std::pow(std::max(0.0f,dotproduct(r,v)),mat.shine);

            return Pixel(glm::vec4(lambert_diffuse_term + ambient_term + phong_specular_term, 1.0f));
    }
    return Pixel(fragment.color);
}