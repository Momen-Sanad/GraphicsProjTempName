#version 330 core

in vec4 vColor;
in vec2 vTexCoord;
in vec3 vNormal;

out vec4 outColor;

uniform sampler2D u_tex0;
uniform bool u_hasTexture;

void main() {
    // Simple directional lighting
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    vec3 normal = normalize(vNormal);
    
    // Basic diffuse lighting with ambient
    float ambient = 0.3;
    float diffuse = max(dot(normal, lightDir), 0.0);
    float lighting = ambient + diffuse * 0.7;
    
    // Get base color from texture or vertex color
    vec4 baseColor;
    if (u_hasTexture) {
        baseColor = texture(u_tex0, vTexCoord);
    } else {
        baseColor = vColor;
    }
    
    // Apply lighting
    vec3 color = baseColor.rgb * lighting;
    
    outColor = vec4(color, baseColor.a);
}