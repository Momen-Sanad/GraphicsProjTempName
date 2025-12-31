#version 330 core

// Varyings
in Varyings {
    vec4 color;
    float height;
} fs_in;

// Output
out vec4 fragColor;

// Tint uniform (from your material)
uniform vec4 tint;

void main() {
    // Compute a vertical gradient: black to white
    vec4 gradientColor = mix(vec4(0.0, 0.0, 0.0, 1.0),
                             vec4(1.0, 1.0, 1.0, 1.0),
                             fs_in.height);

    // Final color: gradient * tint
    fragColor = gradientColor * tint;
}
