#include "TexturedMaterial.hpp"
#include <algorithm>

// ------------------------------------------------------
// Default Constructor
// ------------------------------------------------------
TexturedMaterial::TexturedMaterial()
    : Material(), maxTextures(8)  // Initialize base class and set maximum texture layers to 8
{
}

// ------------------------------------------------------
// Constructor with Shader and Texture
// ------------------------------------------------------
TexturedMaterial::TexturedMaterial(std::shared_ptr<Shader> shader, Texture* tex)
    : Material(), maxTextures(8)  // Initialize base class and set maximum texture layers to 8
{
    setShader(std::move(shader));  // Set shader for the material
    if (tex)
        addTextureLayer(tex);  // If texture is provided, add it as a texture layer
}

// ------------------------------------------------------
// Single texture methods
// ------------------------------------------------------

// Set a single texture and clear any existing texture layers
void TexturedMaterial::setTexture(Texture* tex)
{
    clearTextureLayers();  // Clear previous texture layers
    if (tex)
        addTextureLayer(tex, 1.0f);  // Add the provided texture with full blend weight
}

// Get the first texture in the texture layers (if available)
Texture* TexturedMaterial::getTexture() const
{
    return textureLayers.empty() ? nullptr : textureLayers[0].texture;
}

// Set the texture unit for the first texture layer
void TexturedMaterial::setTextureUnit(int unit)
{
    if (!textureLayers.empty())
        textureLayers[0].unit = unit;  // Set the unit for the first texture
}

// Get the texture unit for the first texture layer
int TexturedMaterial::getTextureUnit() const
{
    return textureLayers.empty() ? 0 : textureLayers[0].unit;  // Return the unit of the first texture
}

// ------------------------------------------------------
// Multi-texture methods
// ------------------------------------------------------

// Add a texture layer with specified blend mode and weight
void TexturedMaterial::addTextureLayer(Texture* tex, BlendMode blmod, float blendWeight)
{
    // If the maximum number of texture layers is reached, do not add another one
    if (textureLayers.size() >= maxTextures)
        return;

    int unit = static_cast<int>(textureLayers.size());  // Use the current size of texture layers as the texture unit
    textureLayers.emplace_back(tex, unit, blmod, blendWeight);  // Add a new texture layer
}

// Add a texture layer with a specified texture unit, blend mode, and weight
void TexturedMaterial::addTextureLayer(Texture* tex, int unit, BlendMode blmod, float blendWeight)
{
    // If the maximum number of texture layers is reached, do not add another one
    if (textureLayers.size() >= maxTextures)
        return;

    textureLayers.emplace_back(tex, unit, blmod, blendWeight);  // Add a new texture layer with specified parameters
}

// Set a texture layer at a specific index with new texture, blend mode, and weight
void TexturedMaterial::setTextureLayer(int index, Texture* tex, BlendMode blmod, float blendWeight)
{
    if (index >= 0 && index < textureLayers.size())
    {
        textureLayers[index].texture = tex;
        textureLayers[index].blendWeight = blendWeight;
        textureLayers[index].blendMode = blmod;
    }
}

// Remove a texture layer at a specific index
void TexturedMaterial::removeTextureLayer(int index)
{
    if (index >= 0 && index < textureLayers.size())
        textureLayers.erase(textureLayers.begin() + index);  // Remove the texture layer at the specified index
}

// Clear all texture layers
void TexturedMaterial::clearTextureLayers()
{
    textureLayers.clear();  // Remove all texture layers
}

// Get the total number of texture layers
int TexturedMaterial::getTextureLayerCount() const
{
    return static_cast<int>(textureLayers.size());  // Return the size of the texture layers array
}

// Get a texture layer by index (non-const version)
TextureLayer& TexturedMaterial::getTextureLayer(int index)
{
    return textureLayers[index];  // Return the texture layer at the specified index
}

// Get a texture layer by index (const version)
const TextureLayer& TexturedMaterial::getTextureLayer(int index) const
{
    return textureLayers[index];  // Return the texture layer at the specified index
}

// Set the blend weight for a texture layer at a specified index
void TexturedMaterial::setBlendWeight(int index, float weight)
{
    if (index >= 0 && index < textureLayers.size())
        textureLayers[index].blendWeight = std::clamp(weight, 0.0f, 1.0f);  // Ensure the weight is between 0 and 1
}

// Get the blend weight for a texture layer at a specified index
float TexturedMaterial::getBlendWeight(int index) const
{
    return (index >= 0 && index < textureLayers.size())
        ? textureLayers[index].blendWeight  // Return the blend weight for the texture layer at the specified index
        : 0.0f;  // Return 0 if the index is out of range
}

// ------------------------------------------------------
// Setup method for applying textures and blending in shaders
// ------------------------------------------------------
void TexturedMaterial::setup()
{
    if (!shader)
        return;  // If no shader is set, return immediately

    shader->use();  // Use the shader for this material

    int count = textureLayers.size();  // Get the number of texture layers
    if (count == 0)
        return;  // If no textures are added, do nothing

    // Bind all textures to fixed texture units (0, 1, ..., n)
    for (int i = 0; i < count; i++)
    {
        if (textureLayers[i].texture)
        {
            glActiveTexture(GL_TEXTURE0 + i);  // Activate the corresponding texture unit
            glBindTexture(GL_TEXTURE_2D, textureLayers[i].texture->get_id());  // Bind the texture
        }
    }

    // Set fixed sampler uniform names: u_tex0, u_tex1, ..., u_tex7
    static const char* samplerNames[8] = {
        "u_tex0", "u_tex1", "u_tex2", "u_tex3",
        "u_tex4", "u_tex5", "u_tex6", "u_tex7"
    };

    // Assign each texture unit to its respective sampler uniform in the shader
    for (int i = 0; i < count; i++)
    {
        GLint loc = getUniformLocation(samplerNames[i]);  // Get the location of the uniform
        if (loc != -1)
            glUniform1i(loc, i);  // Set the texture unit for the sampler
    }

    // Upload blend weights to the shader
    std::vector<float> weights(count);
    for (int i = 0; i < count; i++)
        weights[i] = textureLayers[i].blendWeight;  // Store the blend weights

    GLint weightsLoc = getUniformLocation("u_blendWeights");
    if (weightsLoc != -1)
        glUniform1fv(weightsLoc, count, weights.data());  // Upload blend weights to the shader

    // Upload blend modes to the shader
    std::vector<GLint> modes(count);
    for (int i = 0; i < count; i++)
        modes[i] = static_cast<GLint>(textureLayers[i].blendMode);  // Store the blend modes

    GLint modesLoc = getUniformLocation("u_blendModes");
    if (modesLoc != -1)
        glUniform1iv(modesLoc, count, modes.data());  // Upload blend modes to the shader

    // Upload the number of textures to the shader
    GLint countLoc = getUniformLocation("u_textureCount");
    if (countLoc != -1)
        glUniform1i(countLoc, count);  // Upload the texture count
}

void TexturedMaterial::setAlbedoTexture(Texture* tex) {
    setTexture(tex);
}

void TexturedMaterial::setSpecularTexture(Texture* tex) {
    addTextureLayer(tex, 1);
}

void TexturedMaterial::setRoughnessTexture(Texture* tex) {
    addTextureLayer(tex, 2);
}

void TexturedMaterial::setEmissiveTexture(Texture* tex) {
    addTextureLayer(tex, 3);
}

void TexturedMaterial::setAmbientOcclusionTexture(Texture* tex) {
    addTextureLayer(tex, 4);
}
