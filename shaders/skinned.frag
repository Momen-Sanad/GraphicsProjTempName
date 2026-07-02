#version 330 core

in vec4 vColor;
in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vWorldPos;
in vec4 vBoneWeights;

out vec4 outColor;

uniform sampler2D u_tex0;
uniform bool u_hasTexture;
uniform int u_debugMode;
uniform vec3 camera_pos;
uniform vec3 ambient;

struct Light {
    int type;
    vec3 color;
    vec3 position;
    vec3 direction;
    float cos_inner_angle;
    float cos_outer_angle;
    float intensity;
};

#define TYPE_DIRECTIONAL 0
#define TYPE_POINT 1
#define TYPE_SPOT 2
#define MAX_LIGHT_COUNT 8

uniform Light lights[MAX_LIGHT_COUNT];
uniform int light_count;
uniform bool u_useLightBlock;

layout(std140) uniform LightBlock {
    vec4 lb_header;
    vec4 lb_data[MAX_LIGHT_COUNT * 4];
};

Light light_from_block(int index) {
    int base = index * 4;
    vec4 color = lb_data[base + 0];
    vec4 posType = lb_data[base + 1];
    vec4 dir = lb_data[base + 2];
    vec4 angles = lb_data[base + 3];

    Light light;
    light.type = int(posType.w + 0.5);
    light.color = color.rgb;
    light.position = posType.xyz;
    light.direction = dir.xyz;
    light.cos_inner_angle = angles.x;
    light.cos_outer_angle = angles.y;
    light.intensity = color.w;
    return light;
}

vec3 apply_light(Light light, vec3 normal, vec3 view, vec3 baseColor) {
    vec3 Ldir;
    float attenuation = 1.0;

    if (light.type == TYPE_DIRECTIONAL) {
        Ldir = normalize(-light.direction);
    } else {
        vec3 toLight = light.position - vWorldPos;
        float distanceToLight = length(toLight);
        Ldir = distanceToLight > 1e-6 ? toLight / distanceToLight : vec3(0.0, 1.0, 0.0);
        attenuation = 1.0 / max(distanceToLight * distanceToLight, 1.0);

        if (light.type == TYPE_SPOT) {
            float cosAngle = dot(normalize(light.direction), -Ldir);
            attenuation *= smoothstep(light.cos_outer_angle, light.cos_inner_angle, cosAngle);
        }
    }

    float diffuseTerm = max(dot(normal, Ldir), 0.0);
    vec3 halfVector = normalize(view + Ldir);
    float specularTerm = pow(max(dot(normal, halfVector), 0.0), 32.0);
    return (baseColor * diffuseTerm + vec3(specularTerm) * 0.15) * light.color * light.intensity * attenuation;
}

void main() {
    vec3 normal = normalize(vNormal);

    // Get base color from texture or vertex color
    vec4 baseColor;
    if (u_hasTexture) {
        baseColor = texture(u_tex0, vTexCoord);
    } else {
        baseColor = vColor;
    }

    if (u_debugMode == 1) {
        outColor = vec4(baseColor.rgb, baseColor.a);
        return;
    }
    if (u_debugMode == 2) {
        outColor = vec4(normal * 0.5 + 0.5, baseColor.a);
        return;
    }
    if (u_debugMode == 4) {
        outColor = vec4(vBoneWeights.xyz, baseColor.a);
        return;
    }

    vec3 view = normalize(camera_pos - vWorldPos);
    vec3 lit = baseColor.rgb * ambient;
    int count = u_useLightBlock
        ? min(int(lb_header.x + 0.5), MAX_LIGHT_COUNT)
        : min(light_count, MAX_LIGHT_COUNT);
    for (int i = 0; i < count; i++) {
        Light light = u_useLightBlock ? light_from_block(i) : lights[i];
        lit += apply_light(light, normal, view, baseColor.rgb);
    }

    vec3 color = lit;
    if (u_debugMode == 3) {
        color = lit / max(baseColor.rgb, vec3(0.001));
    }

    outColor = vec4(color, baseColor.a);
}
