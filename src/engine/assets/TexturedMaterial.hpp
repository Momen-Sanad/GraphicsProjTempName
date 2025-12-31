#pragma once
#include "Material.hpp"
#include "../gl/Texture.hpp"
#include <memory>
#include <vector>

enum class BlendMode 
{
    Lerp,      // Linear interpolation: base * (1 - factor) + blend * factor
    Multiply,  // base * blend
    Add,       // base + blend
    Overlay,   // Overlay blend mode
    Screen     // Screen blend mode
};

struct TextureLayer
{
    Texture* texture;
	BlendMode blendMode;
    int unit;
    float blendWeight;  // 0.0 to 1.0

    TextureLayer(Texture* tex = nullptr, int u = 0, BlendMode blmod = BlendMode::Lerp, float weight = 1.0f)
        : texture(tex), unit(u), blendWeight(weight), blendMode(blmod) {}
};

class TexturedMaterial : public Material
{
private:
    std::vector<TextureLayer> textureLayers;
    int maxTextures;  // Maximum number of textures supported

public:
    TexturedMaterial();
    TexturedMaterial(std::shared_ptr<Shader> shader, Texture* tex);

    // Single texture methods (backward compatibility)
    void setTexture(Texture* tex);
    Texture* getTexture() const;
    void setTextureUnit(int unit);
    int getTextureUnit() const;

    // Multi-texture methods
    void addTextureLayer(Texture* tex, BlendMode blmod = BlendMode::Lerp, float blendWeight = 1.0f);
    void addTextureLayer(Texture* tex, int unit, BlendMode blmod = BlendMode::Lerp, float blendWeight = 1.0f);
    void setTextureLayer(int index, Texture* tex, BlendMode blmod, float blendWeight);
    void removeTextureLayer(int index);
    void clearTextureLayers();

    int getTextureLayerCount() const;
    TextureLayer& getTextureLayer(int index);
    const TextureLayer& getTextureLayer(int index) const;

    void setBlendWeight(int index, float weight);
    float getBlendWeight(int index) const;

    void setup() override;
};