#version 330 core

// These two variables are of a type called "Attributes" and they will be read from the vertex buffer
// and supplied to the vertex shader for each vertex. Read the CPP code for more details.
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

// This variable "color" is of a type called "Varyings". Varyings will be passed from the vertex shader to the fragment shader.
// We are collecting all the varyings (currently, the color only) into what is called "Interface Block" for the sake of organization.
out Varyings { // Note: the name "Varying" this line is something we decided. We can use any name as long as the fragmenr shader uses the same name for this block.
    vec4 color;
} vs_out;

// This variable is of a type called "Uniform". It is sent from the CPP code to the shader.
// Unlike attributes, the value of a uniform is constant over all the vertex shader invocations in the same draw call (call of glDrawArrays or glDrawElements)
// We will use this uniform, which is a 4x4 matrix, to transform vertices
uniform mat4 MVP;

void main() {
    // The vertex shader currently does two things only:
    // 1. pass the position attribute to the builtin output variable "gl_Position" after adding the homogenous component to it and transforming it by MVP.
    // We will learn more about the homogenous component later, but for now, we will just need to let it be 1.
    gl_Position = MVP * vec4(position, 1.0);
    // 2. pass the color attribute to the color varying.
    vs_out.color = color;
}