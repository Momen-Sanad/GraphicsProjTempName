#version 330 core

// This variable "color" is of a type called "Varyings". Varyings will be passed from the vertex shader to the fragment shader.
// We are collecting all the varyings (currently, the color only) into what is called "Interface Block" for the sake of organization.
// Although the vertex shader sends only one value for each varying per vertex, the fragment shader receives one value per pixel, because the rasterizer 
// applies barycentric interpolation to compute a value for each pixel as a linear combination of the values at the three vertices that form the triangle.
in Varyings { // Note: the name "Varying" this line is something we decided. We can use any name as long as the vertex shader uses the same name for this block.
    vec4 color;
} fs_in;

// This variable "fragColor" an output that will be sent to the framebuffer (which is the window's backbuffer in our case).
out vec4 fragColor;

// This variable is of a type called "Uniform". It is sent from the CPP code to the shader.
// Unlike varyings, the value of a uniform is constant over all the fragment shader invocations in the same draw call (call of glDrawArrays or glDrawElements)
// We will use this uniform to tint the color.
uniform vec4 tint;

void main() {
    // The fragment shader currently does one thing only:
    // 1. Pass the color varying to the output variable "fragColor" after tinting it.
    fragColor = fs_in.color * tint;
}