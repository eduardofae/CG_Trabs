#version 450 core

struct MaterialInfo {
    vec3   ambient;
    vec3   diffuse;
    vec3   specular;
    float  shine;
};

subroutine vec3 Shading(vec4 p, vec4 n, vec4 cam, MaterialInfo mat);
subroutine uniform Shading lightModels;

in vec3 inColor;
in vec4 inPosition;
in vec4 inNormal;
in vec4 inCam;
in vec2 inTexture;

in MaterialInfo inMat;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D text;
uniform bool hasTexture;

out vec4 color;

subroutine (Shading) vec3 Phong(vec4 p, vec4 n, vec4 cam, MaterialInfo mat){
    vec4 l  = normalize(cam - p);
    vec3 I  = vec3(1.0, 1.0, 1.0);
    vec3 Ia = vec3(0.2, 0.2, 0.2);

    vec4 v = normalize(cam - p);
    vec4 r = -l + 2*n*dot(n,l);

    vec3 lambert_diffuse_term = mat.diffuse*I*max(0,dot(n,l));
    if(hasTexture)
        lambert_diffuse_term = texture(text, inTexture).rgb*I*max(0,dot(n,l));
    vec3 ambient_term         = mat.ambient*Ia;
    vec3 phong_specular_term  = mat.specular*I*pow(max(0,dot(r,v)),mat.shine);

    return lambert_diffuse_term + ambient_term + phong_specular_term;
}

subroutine (Shading) vec3 None(vec4 p, vec4 n, vec4 cam, MaterialInfo mat){
    return inColor;
}

void main()
{
    color.rgb = lightModels(inPosition, inNormal, inCam, inMat);
    color.a = 1;
}