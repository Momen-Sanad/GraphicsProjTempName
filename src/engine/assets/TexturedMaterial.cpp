#include "TexturedMaterial.hpp"


TexturedMaterial::TexturedMaterial(): Material(), texture(nullptr), textureUnit(0)
{
}

TexturedMaterial::TexturedMaterial(std::shared_ptr<Shader> shader, Texture* tex)
    : Material(), texture(tex), textureUnit(0)
{
    setShader(std::move(shader));
}

void TexturedMaterial::setTexture(Texture* tex)
{
    texture = tex;
}

Texture* TexturedMaterial::getTexture() const
{
    return texture;
}

void TexturedMaterial::setTextureUnit(int unit)
{
    textureUnit = unit;
}

int TexturedMaterial::getTextureUnit() const
{
    return textureUnit;
}

void TexturedMaterial::setup()
{
    if (!shader)
        return;  // No shader, nothing to setup

    // Use the shader
    shader->use();

    // Bind texture if available
    if (texture) 
    {
        texture->bind(textureUnit);
      
        // Set the texture sampler uniform
        GLint texLocation = getUniformLocation("u_texture");
        if (texLocation != -1)
            glUniform1i(texLocation, textureUnit);
       
    }
}