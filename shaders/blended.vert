#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 2) in vec2 a_texCoord;

out vec2 v_texCoord;

uniform mat4 MVP;

void main()
{
    v_texCoord = a_texCoord;
    gl_Position = MVP * vec4(a_position, 1.0);
}

