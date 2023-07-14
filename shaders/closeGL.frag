#version 400 core

in vec2 text_pos;

uniform sampler2D text;

out vec4 color;

void main()
{
    color = texture(text, text_pos);
}