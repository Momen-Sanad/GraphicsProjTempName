#pragma once
#include "Material.hpp"
#include "../gl/Texture.hpp"
#include <memory>


class TexturedMaterial : public Material 
{
private:
    Texture* texture;
    int textureUnit;  // Which texture unit to bind to (default 0)

public:
    TexturedMaterial();
    explicit TexturedMaterial(std::shared_ptr<Shader> shader, Texture* tex = nullptr);
    ~TexturedMaterial() override = default;

    // Set the texture
    void setTexture(Texture* tex);
    Texture* getTexture() const;

    // Set which texture unit to use (default is 0)
    void setTextureUnit(int unit);
    int getTextureUnit() const;

    // Override setup to bind texture and set uniforms
    void setup() override;
};