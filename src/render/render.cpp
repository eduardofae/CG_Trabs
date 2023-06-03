#include "render.hpp"

static const GLchar* ReadShader(const char* filename)
{
    FILE* infile = fopen(filename, "rb");

    if (!infile) {
        std::cerr << "Unable to open file '" << filename << "'" << std::endl;
        return NULL;
    }

    fseek(infile, 0, SEEK_END);
    int len = ftell(infile);
    fseek(infile, 0, SEEK_SET);

    GLchar* source = new GLchar[len + 1];

    fread(source, 1, len, infile);
    fclose(infile);

    source[len] = 0;

    return const_cast<const GLchar*>(source);
}


GLuint LoadShaders(ShaderInfo* shaders)
{
    if (shaders == NULL) { return 0; }

    GLuint program = glCreateProgram();

    ShaderInfo* entry = shaders;
    while (entry->type != GL_NONE) {
        GLuint shader = glCreateShader(entry->type);

        entry->shader = shader;

        const GLchar* source = ReadShader(entry->filename);
        if (source == NULL) {
            for (entry = shaders; entry->type != GL_NONE; ++entry) {
                glDeleteShader(entry->shader);
                entry->shader = 0;
            }

            return 0;
        }

        glShaderSource(shader, 1, &source, NULL);
        delete[] source;

        glCompileShader(shader);

        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLsizei len;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

            GLchar* log = new GLchar[len + 1];
            glGetShaderInfoLog(shader, len, &len, log);
            std::cerr << "Shader compilation failed: " << log << std::endl;
            delete[] log;

            return 0;
        }

        glAttachShader(program, shader);

        ++entry;
    }

    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLsizei len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

        GLchar* log = new GLchar[len + 1];
        glGetProgramInfoLog(program, len, &len, log);
        std::cerr << "Shader linking failed: " << log << std::endl;
        delete[] log;

        for (entry = shaders; entry->type != GL_NONE; ++entry) {
            glDeleteShader(entry->shader);
            entry->shader = 0;
        }

        return 0;
    }

    return program;
}

ObjectInfo ReadObject(char *FileName)
{
    std::vector<MaterialInfo> material;
    int material_count, numTris;
    char ch = 'a';

    FILE* fp = fopen(FileName,"r");
    if (fp==NULL) { printf("ERROR: unable to open TriObj [%s]!\n",FileName); exit(1); }
    
    while(ch!= '\n') fscanf(fp, "%c", &ch); // skip the first line – object’s name    
    
    fscanf(fp,"# triangles = %d\n"   , &numTris);        // read # of triangles
    fscanf(fp,"Material count = %d\n", &material_count); // read material count

    for (int i=0; i<material_count; i++) {
        MaterialInfo pai;
        fscanf(fp, "ambient color %f %f %f\n" , &(pai.ambient.x) , &(pai.ambient.y) , &(pai.ambient.z));
        fscanf(fp, "diffuse color %f %f %f\n" , &(pai.diffuse.x) , &(pai.diffuse.y) , &(pai.diffuse.z));
        fscanf(fp, "specular color %f %f %f\n", &(pai.specular.x), &(pai.specular.y), &(pai.specular.z));
        fscanf(fp, "material shine %f\n", &(pai.shine));
        material.emplace_back(pai);
    }

    ch = 'b';
    while(ch!= '\n') fscanf(fp, "%c", &ch); // skip documentation line
    
    printf ("Reading in %s (%d triangles). . .\n", FileName, numTris);
    ObjectInfo Obj;
    
    for (int i=0; i < numTris * 3; i++) // read triangles
    {
        glm::vec3 pos, norm, face_norm;
        int material_id, fuck;
        fscanf(fp, "v%d %f %f %f %f %f %f %d\n",
            &(fuck),
            &(pos.x), &(pos.y), &(pos.z),
            &(norm.x), &(norm.y), &(norm.z),
            &(material_id));
        Obj.position.emplace_back(pos);
        Obj.normal.emplace_back(norm);
        Obj.material_id.emplace_back(material_id);

        

        if((i + 1) % 3 == 0){
            fscanf(fp, "face normal %f %f %f\n",
                &(face_norm.x), &(face_norm.y), &(face_norm.z));
            Obj.face_normal.emplace_back(face_norm);
        }

        Obj.color.emplace_back(1.0f, 0.0f, 0.0f);
    }

    fclose(fp);

    printf("File read with success!\n");

    return Obj;
} 


