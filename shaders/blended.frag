#version 330 core

in vec2 v_texCoord;
out vec4 FragColor;

#define MAX_TEXTURES 8

// Blend modes
#define BLEND_LERP     0
#define BLEND_MULTIPLY 1
#define BLEND_ADD      2
#define BLEND_OVERLAY  3
#define BLEND_SCREEN   4

// Separate sampler uniforms (Linux safe)
uniform sampler2D u_tex0;
uniform sampler2D u_tex1;
uniform sampler2D u_tex2;
uniform sampler2D u_tex3;
uniform sampler2D u_tex4;
uniform sampler2D u_tex5;
uniform sampler2D u_tex6;
uniform sampler2D u_tex7;

uniform float u_blendWeights[MAX_TEXTURES];
uniform int u_blendModes[MAX_TEXTURES];
uniform int u_textureCount;

// --- Texture fetch without sampler array ---
vec4 fetchTexture(int index, vec2 uv)
{
    if(index == 0) return texture(u_tex0, uv);
    if(index == 1) return texture(u_tex1, uv);
    if(index == 2) return texture(u_tex2, uv);
    if(index == 3) return texture(u_tex3, uv);
    if(index == 4) return texture(u_tex4, uv);
    if(index == 5) return texture(u_tex5, uv);
    if(index == 6) return texture(u_tex6, uv);
    if(index == 7) return texture(u_tex7, uv);

    return vec4(0.0); // fallback
}

// --- Blend functions---
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

// --- MAIN ---
void main()
{
    // Base texture is texture 0
    vec4 result = fetchTexture(0, v_texCoord);

    for(int i = 1; i < u_textureCount && i < MAX_TEXTURES; i++)
    {
        vec4 blendTex = fetchTexture(i, v_texCoord);
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
