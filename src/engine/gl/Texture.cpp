#include "Texture.hpp"
#include <cstdio>


Texture::Texture() : id(0), sampler(0), width(0), height(0), format(GL_RGBA) {}

Texture::~Texture()
{
    if (id != 0) glDeleteTextures(1, &id);
    if (sampler != 0) glDeleteSamplers(1, &sampler);
}


void Texture::create(int width, int height, const unsigned char* data, GLenum format)
{
    this->width = width;
    this->height = height;
    this->format = format;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    // Choose internal format based on input format
    GLenum internal_format;
    if (format == GL_RED)
        internal_format = GL_R8;
    else if (format == GL_RGB)
        internal_format = GL_RGB8;
    else if (format == GL_RGBA)
        internal_format = GL_RGBA8;
    else
        internal_format = format;

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    glGenSamplers(1, &sampler);

    // Default sampler settings
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Anisotropy
    float max_aniso;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &max_aniso);
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY, max_aniso);
    glGenerateMipmap(GL_TEXTURE_2D);

}

void Texture::set_filters(GLenum min_filter, GLenum mag_filter)
{
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, min_filter);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, mag_filter);
}

void Texture::set_wrap(GLenum wrap_s, GLenum wrap_t)
{
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, wrap_s);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, wrap_t);
}

 
void Texture::bind(int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);

    glBindTexture(GL_TEXTURE_2D, id);

    glBindSampler(unit, sampler);
}


void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindSampler(0, 0);
}


GLuint Texture::get_id() const { return id; }
int Texture::get_width() const { return width; }
int Texture::get_height() const { return height; }
GLenum Texture::get_format() const { return format; }