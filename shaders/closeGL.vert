#version 400 core

layout( location = 0 ) in vec2 vPosition;
layout( location = 3 ) in vec2 vTexture;

void main()
{
    gl_Position = vPosition;
}