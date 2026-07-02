#version 330 core
struct Light {
    int type; // 0=dir,1=point,2=spot
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float range;
    float innerCone; // cos
    float outerCone; // cos
};

#define MAX_LIGHTS 16
uniform int u_lightCount;
uniform Light u_lights[MAX_LIGHTS];
uniform vec3 u_cameraPos;
uniform int u_debugMode;

// material
struct Material {
    sampler2D albedoMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D aoMap;
    sampler2D emissionMap;
    vec3 albedoFactor;
    float metallicFactor;
    float roughnessFactor;
    float aoFactor;
    vec3 emissionFactor;
};
uniform Material u_material;

in vec3 v_worldPos;
in vec3 v_normal;
in vec2 v_uv;
out vec4 fragColor;

vec3 applyLight(Light light, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness) {
    vec3 L;
    float attenuation = 1.0;
    if (light.type == 0) {
        // directional: use light.direction (should be normalized)
        L = normalize(-light.direction);
    } else {
        L = normalize(light.position - v_worldPos);
        float dist = length(light.position - v_worldPos);
        attenuation = clamp(1.0 - (dist / light.range), 0.0, 1.0);
    }
    vec3 H = normalize(L + V);

    // Simple Blinn-Phong-ish spec:
    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float spec = pow(NdotH, max(1.0, 32.0 * (1.0 - roughness))); // roughness→lower shininess

    vec3 diffuse = albedo * NdotL;
    vec3 specular = vec3(spec) * mix(vec3(0.04), albedo, metallic);

    // Spot cone attenuation
    if (light.type == 2) {
        float cosTheta = dot(normalize(-light.direction), L);
        float spotAtt = smoothstep(light.outerCone, light.innerCone, cosTheta);
        attenuation *= spotAtt;
    }

    return (diffuse + specular) * light.color * light.intensity * attenuation;
}

void main() {
    vec3 N = normalize(v_normal);
    vec3 V = normalize(u_cameraPos - v_worldPos);
    // Fetch material maps (fall back to factors)
    vec3 albedo = (texture(u_material.albedoMap, v_uv).rgb) * u_material.albedoFactor;
    float metallic = texture(u_material.metallicMap, v_uv).r * u_material.metallicFactor;
    float roughness = texture(u_material.roughnessMap, v_uv).r * u_material.roughnessFactor;
    float ao = texture(u_material.aoMap, v_uv).r * u_material.aoFactor;
    vec3 emission = texture(u_material.emissionMap, v_uv).rgb * u_material.emissionFactor;

    if (u_debugMode == 1) {
        fragColor = vec4(albedo, 1.0);
        return;
    }
    if (u_debugMode == 2) {
        fragColor = vec4(N * 0.5 + 0.5, 1.0);
        return;
    }
    if (u_debugMode == 5) {
        fragColor = vec4(roughness, ao, metallic, 1.0);
        return;
    }

    vec3 accum = vec3(0.0);
    for (int i = 0; i < u_lightCount; ++i) {
        accum += applyLight(u_lights[i], N, V, albedo, metallic, roughness);
    }

    accum = accum * ao + emission;
    if (u_debugMode == 3) {
        fragColor = vec4(accum, 1.0);
        return;
    }
    fragColor = vec4(accum, 1.0);
}
