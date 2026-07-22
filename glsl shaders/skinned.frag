#version 330 core

in vec4 vColor;
in vec2 vTexCoord;
in vec3 vNormal;

out vec4 outColor;

void main() {
    // DEBUG: Output solid red to verify mesh is rendering
    outColor = vec4(1.0, 0.0, 0.0, 1.0);
    
    // Original lighting code (commented out for testing):
    // vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    // float diff = max(dot(normalize(vNormal), lightDir), 0.3);
    // outColor = vColor * diff;
}