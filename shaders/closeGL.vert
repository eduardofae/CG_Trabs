#version 400 core

layout( location = 0 ) in vec2 vPosition;
layout( location = 3 ) in vec2 vTexture;

uniform sampler2D text;

out vec4 inColor;

void main()
{
    inColor = texture(text, vTexture);
    gl_Position = vec4(vPosition, 1.0f, 1.0f);
}