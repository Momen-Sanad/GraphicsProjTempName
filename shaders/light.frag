#version 330 core

// This will be used to compute the diffuse factor.
float calculate_lambert(vec3 normal, vec3 light){
    return max(0.0f, dot(normal, light));
}

// This will be used to compute the blinn phong specular.
float calculate_blinn_phong(vec3 normal, vec3 light, vec3 view, float shininess){
    vec3 half_blinn_phong = normalize(view + light);
    return pow(max(0.0f, dot(normal, half_blinn_phong)), shininess);
}

// This contains all the material properties for a single pixel.
// We have an extra property "emissive" which is used when the pixel itself emits light.
struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    vec3 emissive;
    float shininess;
    float alpha;
};

// This contains all the material properties and texture maps for the object.
struct TexturedMaterial {
    sampler2D albedo_map;
    sampler2D specular_map;
    sampler2D ambient_occlusion_map;
    sampler2D roughness_map;
    sampler2D emissive_map;
};

// This function samples the texture maps from the textured material and calculates the equivalent material at the given texture coordinates.
Material sample_material(TexturedMaterial tex_mat, vec2 tex_coord){
    Material mat;
    vec4 albedo = texture(tex_mat.albedo_map, tex_coord);
    mat.diffuse = albedo.rgb;
    mat.alpha = albedo.a;
    mat.specular = texture(tex_mat.specular_map, tex_coord).rgb;
    mat.emissive = texture(tex_mat.emissive_map, tex_coord).rgb;
    mat.ambient = mat.diffuse * texture(tex_mat.ambient_occlusion_map, tex_coord).r;

    float roughness = texture(tex_mat.roughness_map, tex_coord).r;
    // We are using a formula designed the Blinn-Phong model which is a popular approximation of the Phong model.
    // The source of the formula is http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
    // It is noteworthy that we clamp the roughness to prevent its value from ever becoming 0 or 1 to prevent lighting artifacts.
    mat.shininess = 2.0f/pow(clamp(roughness, 0.001f, 0.999f), 4.0f) - 2.0f;

    return mat;
}

// These type constants match their peers in the C++ code.
#define TYPE_DIRECTIONAL    0
#define TYPE_POINT          1
#define TYPE_SPOT           2

// Now we will use a single struct for all light types.
struct Light {
    // This will hold the light type.
    int type;
    // This defines the color and intensity of the light.
    vec3 color;
    // Position is used for point and spot lights. Direction is used for directional and spot lights.
    vec3 position, direction;
    // Cone angles are used for spot lights.
    float cos_inner_angle, cos_outer_angle;
};

in Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 world;
    vec3 normal;
} fs_in;

out vec4 frag_color;

uniform vec3 camera_pos;
uniform vec3 ambient;

// This will define the maximum number of lights we can receive.
#define MAX_LIGHT_COUNT 8

// Now we recieve the material, light array, the actual number of lights sent from the cpu and the sky light.
uniform TexturedMaterial material;
uniform Light lights[MAX_LIGHT_COUNT];
uniform int light_count;

void main() {
    // First, we sample the material color from the material textures.
    Material sampled = sample_material(material, fs_in.tex_coord);
    
    vec3 normal = normalize(fs_in.normal); // Although the normal was already normalized, it may become shorter during interpolation.
    vec3 view = normalize(camera_pos - fs_in.world);

    // Initially the accumulated light will hold the ambient light and the emissive light (light coming out of the object).
    vec3 accumulated_light = sampled.emissive + sampled.ambient * ambient;

    // Make sure that the actual light count never exceeds the maximum light count.
    int count = min(light_count, MAX_LIGHT_COUNT);
    // Now we will loop over all the lights.
    for(int index = 0; index < count; index++){
        Light light = lights[index];
        vec3 light_direction;
        float attenuation = 1;
        if(light.type == TYPE_DIRECTIONAL)
            light_direction = -light.direction; // If light is directional, use its direction as the light direction
        else {
            // If not directional, compute the direction from the position.
            light_direction = light.position - fs_in.world;
            float distance = length(light_direction);
            light_direction /= distance;

            // And compute the attenuation.
            attenuation *= 1.0f / (distance * distance);

            if(light.type == TYPE_SPOT){
                // If it is a spot light, comput the angle attenuation.
                float cos_angle = dot(light.direction, light_direction);
                attenuation *= smoothstep(light.cos_outer_angle, light.cos_inner_angle, cos_angle);
            }
        }

        // Now we compute the 2 components of the light separately.
        vec3 diffuse = sampled.diffuse * light.color * calculate_lambert(normal, light_direction);
        vec3 specular = sampled.specular * light.color * calculate_blinn_phong(normal, light_direction, view, sampled.shininess);

        // Then we accumulate the light components additively.
        accumulated_light += (diffuse + specular) * attenuation;
    }

    // frag_color = vec4(1, 0, 1, 1);
    frag_color = fs_in.color * vec4(accumulated_light, sampled.alpha);

}