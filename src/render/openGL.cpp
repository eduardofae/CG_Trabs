#include "openGL.hpp"

void buildOpenGL(GLuint *VAOs, GLuint *Buffers,
                 std::vector<GLfloat> vertices, std::vector<GLfloat> normals,
                 std::vector<GLint> material_id, std::vector<GLfloat> text_coords)
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

    glBindBuffer(GL_ARRAY_BUFFER, Buffers[TextureBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, text_coords.size()*sizeof(GLfloat), text_coords.data(), 0);

    glVertexAttribPointer(vTexture, 2, GL_FLOAT,
        GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vTexture);
}

void renderOpenGL(GLuint program, Matrices matrices,
                  float *color, bool useColor, GLuint *VAOs,
                  int g_mashType, int g_windingOrder, int g_backFaceCulling, int size,
                  int shadingType, GLuint *lightModels, glm::vec4 camera_position,
                  std::vector <MaterialInfo> materials, GLuint texture_id, GLuint sampler_id,
                  bool hasTexture)
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

    glUniformMatrix4fv(glGetUniformLocation(program, "model")             , 1 , GL_FALSE , glm::value_ptr(matrices.model));
    glUniformMatrix4fv(glGetUniformLocation(program, "view")              , 1 , GL_FALSE , glm::value_ptr(matrices.view));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection")        , 1 , GL_FALSE , glm::value_ptr(matrices.proj));

    glUniform4fv(glGetUniformLocation(program, "camera_position")         , 1            , glm::value_ptr(camera_position));
    glUniform1i(glGetUniformLocation(program, "hasTexture")                              , hasTexture);

    for(int i = 0; i < materials.size(); i++){
        std::string mat = "materials[" + std::to_string(i) + "].";
        glUniform3fv(glGetUniformLocation(program, (mat + "ambient").data())       , 1            , glm::value_ptr(materials[i].ambient));
        if(useColor)
            glUniform3fv(glGetUniformLocation(program, (mat + "diffuse").data())   , 1            , color);
        else
            glUniform3fv(glGetUniformLocation(program, (mat + "diffuse").data())   , 1            , glm::value_ptr(materials[i].diffuse));
        glUniform3fv(glGetUniformLocation(program, (mat + "specular").data())      , 1            , glm::value_ptr(materials[i].specular));
        glUniform1f(glGetUniformLocation(program, (mat + "shine").data())                         , materials[i].shine);
    };
    
    glBindVertexArray(VAOs[OpenGL]);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBindSampler(0, sampler_id);

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

void updateTextureOpenGL(TextureInfo &texture, GLuint *texture_id, GLuint *sampler_id)
{
    // Agora criamos objetos na GPU com OpenGL para armazenar a textura
    glGenTextures(1, texture_id);
    glGenSamplers(1, sampler_id);

    // Veja slides 95-96 do documento Aula_20_Mapeamento_de_Texturas.pdf
    glSamplerParameteri(*sampler_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(*sampler_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Parâmetros de amostragem da textura.
    glSamplerParameteri(*sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(*sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Agora enviamos a imagem lida do disco para a GPU
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindSampler(0, *sampler_id);
}