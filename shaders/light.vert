#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in vec4 a_color; // optional vertex color; if you don't provide it, set default in CPU

out Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 world;
    vec3 normal;
} vs_out;

uniform mat4 model;
uniform mat4 viewProj;   // projection * view
uniform mat3 normalMatrix; // mat3(transpose(inverse(model)))

void main()
{
    vec4 worldPos = model * vec4(a_position, 1.0);
    vs_out.world = worldPos.xyz;
    vs_out.normal = normalize(normalMatrix * a_normal);
    vs_out.tex_coord = a_texCoord;
    vs_out.color = a_color; // if not provided from VAO, make sure the attribute is set or fallback in CPU
    // vs_out.color = vec4(1.0);

    gl_Position = viewProj * worldPos;
}
