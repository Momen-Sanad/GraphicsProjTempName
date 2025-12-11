#include "TexturedMaterial.hpp"
#include <algorithm>


TexturedMaterial::TexturedMaterial()
    : Material(), maxTextures(8)
{
}

TexturedMaterial::TexturedMaterial(std::shared_ptr<Shader> shader, Texture* tex)
    : Material(), maxTextures(8)
{
    setShader(std::move(shader));
    if (tex)
        addTextureLayer(tex);
}

// single texture method
void TexturedMaterial::setTexture(Texture* tex)
{
    clearTextureLayers();
    if (tex)
        addTextureLayer(tex, 1.0f);
}

Texture* TexturedMaterial::getTexture() const
{
    return textureLayers.empty() ? nullptr : textureLayers[0].texture;
}

void TexturedMaterial::setTextureUnit(int unit)
{
    if (!textureLayers.empty())
        textureLayers[0].unit = unit;
}

int TexturedMaterial::getTextureUnit() const
{
    return textureLayers.empty() ? 0 : textureLayers[0].unit;
}

// Multi-texture methods
void TexturedMaterial::addTextureLayer(Texture* tex, BlendMode blmod, float blendWeight)
{
    if (textureLayers.size() >= maxTextures)
        return;

    int unit = static_cast<int>(textureLayers.size());
    textureLayers.emplace_back(tex, unit, blmod, blendWeight);
}

void TexturedMaterial::addTextureLayer(Texture* tex, int unit, BlendMode blmod, float blendWeight)
{
    if (textureLayers.size() >= maxTextures)
        return;

    textureLayers.emplace_back(tex, unit, blmod, blendWeight);
}

void TexturedMaterial::setTextureLayer(int index, Texture* tex, BlendMode blmod, float blendWeight)
{
    if (index >= 0 && index < textureLayers.size())
    {
        textureLayers[index].texture = tex;
        textureLayers[index].blendWeight = blendWeight;
		textureLayers[index].blendMode = blmod;
    }
}

void TexturedMaterial::removeTextureLayer(int index)
{
    if (index >= 0 && index < textureLayers.size())
        textureLayers.erase(textureLayers.begin() + index);
}

void TexturedMaterial::clearTextureLayers()
{
    textureLayers.clear();
}

int TexturedMaterial::getTextureLayerCount() const
{
    return static_cast<int>(textureLayers.size());
}

TextureLayer& TexturedMaterial::getTextureLayer(int index)
{
    return textureLayers[index];
}

const TextureLayer& TexturedMaterial::getTextureLayer(int index) const
{
    return textureLayers[index];
}

void TexturedMaterial::setBlendWeight(int index, float weight)
{
    if (index >= 0 && index < textureLayers.size())
        textureLayers[index].blendWeight = std::clamp(weight, 0.0f, 1.0f);
}

float TexturedMaterial::getBlendWeight(int index) const
{
    return (index >= 0 && index < textureLayers.size())
        ? textureLayers[index].blendWeight
        : 0.0f;
}

void TexturedMaterial::setup()
{
    if (!shader)
        return;

    shader->use();

    // Handle single texture case for backward compatibility
    if (textureLayers.size() == 1)
    {
        textureLayers[0].texture->bind(textureLayers[0].unit);

        GLint texLocation = getUniformLocation("u_texture");
        if (texLocation != -1)
            glUniform1i(texLocation, textureLayers[0].unit);
    }
    // Handle multiple textures with blending
    else if (textureLayers.size() > 1)
    {
        // Bind all textures
        for (size_t i = 0; i < textureLayers.size(); ++i)
        {
            if (textureLayers[i].texture)
            {
                textureLayers[i].texture->bind(textureLayers[i].unit);
            }
        }

        // Set texture sampler uniforms
        for (size_t i = 0; i < textureLayers.size(); ++i)
        {
            std::string uniformName = "u_textures[" + std::to_string(i) + "]";
            GLint texLocation = getUniformLocation(uniformName.c_str());
            if (texLocation != -1)
                glUniform1i(texLocation, textureLayers[i].unit);
        }

        // Set blend weights
        std::vector<float> weights;
        for (const auto& layer : textureLayers)
            weights.push_back(layer.blendWeight);

        GLint weightsLocation = getUniformLocation("u_blendWeights");
        if (weightsLocation != -1)
            glUniform1fv(weightsLocation, static_cast<GLsizei>(weights.size()), weights.data());

        // Set blend modes
        std::vector<GLint> blendModes;
        for (const auto& layer : textureLayers)
            blendModes.push_back(static_cast<GLint>(layer.blendMode));

        GLint modesLocation = getUniformLocation("u_blendModes");
        if (modesLocation != -1)
            glUniform1iv(modesLocation, static_cast<GLsizei>(blendModes.size()), blendModes.data());

        // Set texture count
        GLint countLocation = getUniformLocation("u_textureCount");
        if (countLocation != -1)
            glUniform1i(countLocation, static_cast<int>(textureLayers.size()));
    }
}