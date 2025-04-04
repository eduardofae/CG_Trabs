#version 450 core

struct MaterialInfo {
    vec3   ambient;
    vec3   diffuse;
    vec3   specular;
    float  shine;
};

subroutine vec3 Shading(vec4 p, vec4 n, vec4 cam, MaterialInfo mat);
subroutine uniform Shading lightModels;

layout( location = 0 ) in vec3  vPosition;
layout( location = 1 ) in vec3  vNormal;
layout( location = 2 ) in int   vMaterialId;
layout( location = 3 ) in vec2  vTexture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec4 camera_position;

uniform sampler2D text;
uniform bool useTexture;

uniform MaterialInfo materials[50];

out vec3 inColor;
out vec4 inPosition;
out vec4 inNormal;
out vec4 inCam;
out vec2 inTexture;
out vec3 lambertTerm;

out MaterialInfo inMat;

subroutine (Shading) vec3 GAD(vec4 p, vec4 n, vec4 cam, MaterialInfo mat){
    vec4 l  = vec4(normalize(cam - p).rgb, 0.00);
    vec3 I  = vec3(1.0, 1.0, 1.0);
    vec3 Ia = vec3(0.2, 0.2, 0.2);

    vec3 lambert_diffuse_term = mat.diffuse*I*max(0,dot(n,l));
    vec3 ambient_term = mat.ambient*Ia;
    if(useTexture){
        lambert_diffuse_term  = vec3(0,0,0);
        lambertTerm = I*max(0,dot(n,l));
    }

    return lambert_diffuse_term + ambient_term;
}

subroutine (Shading) vec3 GADS(vec4 p, vec4 n, vec4 cam, MaterialInfo mat){
    vec4 l  = vec4(normalize(cam - p).rgb, 0.00);
    vec3 I  = vec3(1.0, 1.0, 1.0);
    vec3 Ia = vec3(0.2, 0.2, 0.2);

    vec4 v = l;
    vec4 r = -l + 2*n*dot(n,l);

    vec3 lambert_diffuse_term = mat.diffuse*I*max(0,dot(n,l));
    vec3 ambient_term         = mat.ambient*Ia;
    vec3 phong_specular_term  = mat.specular*I*pow(max(0,dot(r,v)),mat.shine);
    if(useTexture){
        lambert_diffuse_term  = vec3(0,0,0);
        lambertTerm = I*max(0,dot(n,l));
    }

    return lambert_diffuse_term + ambient_term + phong_specular_term;
}

subroutine (Shading) vec3 None(vec4 p, vec4 n, vec4 cam, MaterialInfo mat){
    return mat.diffuse;
}

void main()
{
    inPosition = model * vec4(vPosition, 1.0f);
    inNormal = vec4(normalize((transpose(inverse(model)) * vec4(vNormal, 0.0f)).xyz), 0.0f);
    inCam = camera_position;
    inMat = materials[vMaterialId];
    
    inColor = lightModels(inPosition, inNormal, inCam, inMat);

    inTexture = vTexture;

    gl_Position = projection * view * inPosition;
}