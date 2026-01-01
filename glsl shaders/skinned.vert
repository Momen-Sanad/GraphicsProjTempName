#version 330 core

// Regular vertex attributes (from first VBO)
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;

// Skeleton vertex attributes (from second VBO)
layout (location = 3) in vec3 aNormal;
layout (location = 4) in vec4 aBoneWeights;
layout (location = 5) in ivec4 aBoneIds;

// Uniforms
uniform mat4 uBoneMatrices[100];
uniform mat4 MVP;
uniform mat4 model;
uniform bool uIsAnimated;

out vec4 vColor;
out vec2 vTexCoord;
out vec3 vNormal;

void main() {
    vec4 skinnedPosition;
    vec3 skinnedNormal;
    
    float totalWeight = aBoneWeights.x + aBoneWeights.y + aBoneWeights.z + aBoneWeights.w;
    
    if (uIsAnimated && totalWeight > 0.01) {
        // Calculate skinning transformation
        mat4 boneTransform = mat4(0.0);
        
        for (int i = 0; i < 4; i++) {
            int boneId = aBoneIds[i];
            if (boneId >= 0 && boneId < 100) {
                boneTransform += uBoneMatrices[boneId] * aBoneWeights[i];
            }
        }
        
        skinnedPosition = boneTransform * vec4(aPosition, 1.0);
        skinnedNormal = mat3(boneTransform) * aNormal;
    } else {
        // No skinning - use original position
        skinnedPosition = vec4(aPosition, 1.0);
        skinnedNormal = aNormal;
    }
    
    gl_Position = MVP * skinnedPosition;
    vColor = aColor;
    vTexCoord = aTexCoord;
    vNormal = normalize(mat3(model) * skinnedNormal);
}