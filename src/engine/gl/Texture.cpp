#include "Texture.hpp"
#include <cstdio>


Texture::Texture() : id(0), width(0), height(0), format(GL_RGBA) {}

Texture::~Texture()
{
    if (id != 0)
        glDeleteTextures(1, &id);
}

void Texture::create(int width, int height, const unsigned char* data, GLenum format)
{
    this->width = width;
    this->height = height;
    this->format = format;

    // Generate texture
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Default filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Default wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::set_filters(GLenum min_filter, GLenum mag_filter)
{
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::set_wrap(GLenum wrap_s, GLenum wrap_t)
{
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::get_id() const { return id; }
int Texture::get_width() const { return width; }
int Texture::get_height() const { return height; }
GLenum Texture::get_format() const { return format; }