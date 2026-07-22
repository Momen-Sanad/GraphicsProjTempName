// lighting.glsl
#ifndef LIGHTING_GLSL
#define LIGHTING_GLSL

#define TYPE_DIRECTIONAL 0
#define TYPE_POINT       1
#define TYPE_SPOT        2
#define MAX_LIGHT_COUNT  8

struct Light {
    int type;
    vec3 color;
    vec3 position;
    vec3 direction;
    float cos_inner_angle;
    float cos_outer_angle;
};

float calculate_lambert(vec3 normal, vec3 lightDir) {
    return max(dot(normal, lightDir), 0.0);
}

float calculate_blinn_phong(vec3 normal, vec3 lightDir, vec3 viewDir, float shininess) {
    vec3 halfVec = normalize(lightDir + viewDir);
    return pow(max(dot(normal, halfVec), 0.0), shininess);
}

vec3 apply_lighting(
    vec3 albedo,
    vec3 specular,
    vec3 normal,
    vec3 worldPos,
    vec3 viewPos,
    vec3 emissive,
    float shininess,
    Light lights[MAX_LIGHT_COUNT],
    int light_count,
    vec3 ambient
) {
    vec3 N = normalize(normal);
    vec3 V = normalize(viewPos - worldPos);

    vec3 result = emissive + albedo * ambient;

    int count = min(light_count, MAX_LIGHT_COUNT);
    for (int i = 0; i < count; i++) {
        Light light = lights[i];

        vec3 L;
        float attenuation = 1.0;

        if (light.type == TYPE_DIRECTIONAL) {
            L = normalize(-light.direction);
        } else {
            vec3 toLight = light.position - worldPos;
            float dist = length(toLight);
            L = toLight / dist;
            attenuation = 1.0 / (dist * dist);

            if (light.type == TYPE_SPOT) {
                float angle = dot(normalize(light.direction), L);
                attenuation *= smoothstep(light.cos_outer_angle, light.cos_inner_angle, angle);
            }
        }

        vec3 diffuse = albedo * light.color * calculate_lambert(N, L);
        vec3 spec = specular * light.color *
                    calculate_blinn_phong(N, L, V, shininess);

        result += (diffuse + spec) * attenuation;
    }

    return result;
}

#endif
