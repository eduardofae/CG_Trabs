#version 400 core

subroutine vec3 Shading(vec4 p);
subroutine uniform Shading lightModels;

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec3 vNormal;
layout( location = 2 ) in vec3 vAmbient;
layout( location = 3 ) in vec3 vDiffuse;
layout( location = 4 ) in vec3 vSpecular;
layout( location = 5 ) in vec3 vShine;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 color;
uniform bool useColor;

out vec4 inColor;

subroutine (Shading) vec3 GAD(vec4 p){
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    vec4 n = normalize(vNormal);

    vec4 l = normalize(vec4(1.0,1.0,0.5,0.0));

    vec3 I = vec3(1.0, 1.0, 1.0);

    vec3 Ia = vec3(0.2, 0.2, 0.2);

    vec3 lambert_diffuse_term = vDiffuse*I*max(0,dot(n,l));

    vec3 ambient_term = vAmbient*Ia;

    return lambert_diffuse_term + ambient_term;
}

subroutine (Shading) vec3 GADS(vec4 p){
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    vec4 n = normalize(vNormal);

    vec4 l = normalize(vec4(1.0,1.0,0.5,0.0));

    vec3 I = vec3(1.0, 1.0, 1.0);

    vec3 Ia = vec3(0.2, 0.2, 0.2);

    vec3 lambert_diffuse_term = vDiffuse*I*max(0,dot(n,l));

    vec3 ambient_term = vAmbient*Ia;

    return lambert_diffuse_term + ambient_term;
}

subroutine (Shading) vec3 Phong(vec4 p){
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    vec4 n = normalize(vNormal);

    vec4 l = normalize(vec4(1.0,1.0,0.5,0.0));

    vec3 I = vec3(1.0, 1.0, 1.0);

    vec3 Ia = vec3(0.2, 0.2, 0.2);

    vec3 lambert_diffuse_term = vDiffuse*I*max(0,dot(n,l));

    vec3 ambient_term = vAmbient*Ia;

    return lambert_diffuse_term + ambient_term;
}

void main()
{
    vec4 pos = vec4(vPosition, 1.0f);

    inColor.rgb = lightModels(pos);
    inColor.a = 1;

    gl_Position = projection * view * model * pos;
}