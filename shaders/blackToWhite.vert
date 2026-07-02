#version 330 core

// Attributes
layout(location = 0) in vec3 position;
layout(location = 3) in vec4 color;

// Varyings
out Varyings {
    vec4 color;
    float height;
} vs_out;

// Uniforms
uniform mat4 MVP;

void main() {
    // MVP transform
    gl_Position = MVP * vec4(position, 1.0);

    // pass through actual vertex color (still allowed)
    vs_out.color = color;

    // Convert cube Y from [-0.5, 0.5] to [0.0, 1.0]
    // this assumes cube is centered at origin and is 1 unit tall
    vs_out.height = position.y + 0.5;
}
