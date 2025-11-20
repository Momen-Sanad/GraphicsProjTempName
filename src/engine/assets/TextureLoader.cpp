#include "TextureLoader.hpp"
#include <iostream>

Texture::Texture() : texture_id(0) {}

Texture::~Texture() {
    if (texture_id != 0) {
        glDeleteTextures(1, &texture_id);
    }
}

bool Texture::load(const std::string& file_path) {
    // Load image using stb_image
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true); // Flip image vertically (OpenGL expects this)
    unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &channels, 0);
    
    if (!data) {
        std::cerr << "Failed to load texture: " << file_path << std::endl;
        return false;
    }

    // Generate OpenGL texture
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // Set texture parameters (wrapping and filtering)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Determine the format of the image (RGB or RGBA)
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    // Upload texture data to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Free the image data after uploading to GPU
    stbi_image_free(data);

    return true;
}
