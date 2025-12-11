#version 330 core

in vec2 v_texCoord;
out vec4 FragColor;

#define MAX_TEXTURES 8

// Blend modes
#define BLEND_LERP 0
#define BLEND_MULTIPLY 1
#define BLEND_ADD 2
#define BLEND_OVERLAY 3
#define BLEND_SCREEN 4

uniform sampler2D u_textures[MAX_TEXTURES];
uniform float u_blendWeights[MAX_TEXTURES];
uniform int u_blendModes[MAX_TEXTURES];
uniform int u_textureCount;

vec4 blendLerp(vec4 base, vec4 blend, float weight)
{
    return mix(base, blend, weight);
}

vec4 blendMultiply(vec4 base, vec4 blend, float weight)
{
    return mix(base, base * blend, weight);
}

vec4 blendAdd(vec4 base, vec4 blend, float weight)
{
    return mix(base, min(base + blend, vec4(1.0)), weight);
}

vec4 blendOverlay(vec4 base, vec4 blend, float weight)
{
    vec4 result;
    for(int i = 0; i < 3; i++)
    {
        if(base[i] < 0.5)
            result[i] = 2.0 * base[i] * blend[i];
        else
            result[i] = 1.0 - 2.0 * (1.0 - base[i]) * (1.0 - blend[i]);
    }
    result.a = base.a;
    return mix(base, result, weight);
}

vec4 blendScreen(vec4 base, vec4 blend, float weight)
{
    return mix(base, vec4(1.0) - (vec4(1.0) - base) * (vec4(1.0) - blend), weight);
}

void main()
{
    vec4 result = texture(u_textures[0], v_texCoord);
    
    for(int i = 1; i < u_textureCount && i < MAX_TEXTURES; i++)
    {
        vec4 blendTex = texture(u_textures[i], v_texCoord);
        int mode = u_blendModes[i];
        float weight = u_blendWeights[i];
        
        if(mode == BLEND_LERP)
            result = blendLerp(result, blendTex, weight);
        else if(mode == BLEND_MULTIPLY)
            result = blendMultiply(result, blendTex, weight);
        else if(mode == BLEND_ADD)
            result = blendAdd(result, blendTex, weight);
        else if(mode == BLEND_OVERLAY)
            result = blendOverlay(result, blendTex, weight);
        else if(mode == BLEND_SCREEN)
            result = blendScreen(result, blendTex, weight);
    }
    
    FragColor = result;
}