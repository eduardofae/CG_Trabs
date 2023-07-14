#version 400 core

layout( location = 0 ) in vec2 vPosition;
layout( location = 3 ) in vec2 vTexture;

out vec2 text_pos;

void main()
{
    text_pos = vTexture;
    gl_Position = vec4(vPosition, 0.0f, 1.0f);
}