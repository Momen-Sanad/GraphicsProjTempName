#pragma once
#include "Material.hpp"
#include "../gl/Texture.hpp"
#include <memory>
#include <utility>
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
    std::shared_ptr<Texture> texture;
    BlendMode blendMode;
    int unit;
    float blendWeight;  // 0.0 to 1.0

    TextureLayer(std::shared_ptr<Texture> tex = nullptr, int u = 0, BlendMode blmod = BlendMode::Lerp, float weight = 1.0f)
        : texture(std::move(tex)), blendMode(blmod), unit(u), blendWeight(weight) {}
};

class TexturedMaterial : public Material
{
private:
    std::vector<TextureLayer> textureLayers;
    int maxTextures;  // Maximum number of textures supported

public:
    TexturedMaterial();
    TexturedMaterial(std::shared_ptr<Shader> shader, std::shared_ptr<Texture> tex);

    void setTexture(std::shared_ptr<Texture> tex);
    Texture* getTexture() const;
    std::shared_ptr<Texture> getTextureHandle() const;
    void setTextureUnit(int unit);
    int getTextureUnit() const;

    // Multi-texture methods
    void addTextureLayer(std::shared_ptr<Texture> tex, BlendMode blmod = BlendMode::Lerp, float blendWeight = 1.0f);
    void addTextureLayer(std::shared_ptr<Texture> tex, int unit, BlendMode blmod = BlendMode::Lerp, float blendWeight = 1.0f);
    void setTextureLayer(int index, std::shared_ptr<Texture> tex, BlendMode blmod, float blendWeight);
    void removeTextureLayer(int index);
    void clearTextureLayers();

    int getTextureLayerCount() const;
    TextureLayer& getTextureLayer(int index);
    const TextureLayer& getTextureLayer(int index) const;

    void setBlendWeight(int index, float weight);
    float getBlendWeight(int index) const;

    void setup() override;

    // New methods for maps
    void setAlbedoTexture          (std::shared_ptr<Texture> tex);
    void setSpecularTexture        (std::shared_ptr<Texture> tex);
    void setRoughnessTexture       (std::shared_ptr<Texture> tex);
    void setEmissiveTexture        (std::shared_ptr<Texture> tex);
    void setAmbientOcclusionTexture(std::shared_ptr<Texture> tex);
};
