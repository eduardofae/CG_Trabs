#version 400 core

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec3 vColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 inColor;

void main()
{
    vec4 pos = vec4(vPosition, 1.0f);

    inColor.rgb = vColor;
    inColor.w = 1.0f;

    gl_Position = projection * view * model * pos;
}