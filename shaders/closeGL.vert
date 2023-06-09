#version 400 core

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec3 vNormal;

uniform vec3 vColor;
uniform vec3 color;
uniform bool useColor;

out vec4 inColor;

void main()
{
    vec4 pos = vec4(vPosition, 1.0f);

    if(useColor) inColor.rgb = color;
    else inColor.rgb = vColor;
    
    inColor.w = 1.0f;

    gl_Position = pos;
}