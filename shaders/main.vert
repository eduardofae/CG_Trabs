#version 400 core

layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec4 vColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 inColor;

void main()
{
    inColor = vColor;

    gl_Position = projection * view * model * vPosition;
}