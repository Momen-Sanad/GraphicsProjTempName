#include "Texture.hpp"
#include <cstdio>

// ----------------------------------------------------
// Default constructor: Initializes the texture with default values
// ----------------------------------------------------
Texture::Texture() : id(0), sampler(0), width(0), height(0), format(GL_RGBA) {}

// ----------------------------------------------------
// Destructor: Cleans up the texture and its associated resources
// ----------------------------------------------------
Texture::~Texture()
{
    if (id != 0) glDeleteTextures(1, &id);  // Deletes the texture if it's valid
    if (sampler != 0) glDeleteSamplers(1, &sampler);  // Deletes the sampler if it's valid
}

// ----------------------------------------------------
// Creates a texture from raw data, specifying width, height, and format
// ----------------------------------------------------
void Texture::create(int width, int height, const unsigned char* data, GLenum format)
{
    this->width = width;  // Store the texture's width
    this->height = height;  // Store the texture's height
    this->format = format;  // Store the texture's format (e.g., GL_RGBA)

    // Generate the OpenGL texture ID
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);  // Bind the texture to 2D targets

    // Choose internal format based on the input format
    GLenum internal_format;
    if (format == GL_RED)
        internal_format = GL_R8;  // Single channel (red) format
    else if (format == GL_RGB)
        internal_format = GL_RGB8;  // RGB format with 8 bits per channel
    else if (format == GL_RGBA)
        internal_format = GL_RGBA8;  // RGBA format with 8 bits per channel
    else
        internal_format = format;  // Use the provided format for internal format

    // Specify the texture image data
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    // Generate mipmaps for the texture
    glGenerateMipmap(GL_TEXTURE_2D);

    // Generate a texture sampler
    glGenSamplers(1, &sampler);

    // Set default sampler parameters
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  // Min filter: linear mipmaps
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // Mag filter: linear
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);  // Wrap mode for the S axis (horizontal)
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);  // Wrap mode for the T axis (vertical)

    // Set anisotropic filtering if supported
    float max_aniso;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &max_aniso);  // Get the maximum anisotropy value supported by the hardware
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY, max_aniso);  // Set the anisotropic filtering level
}

// ----------------------------------------------------
// Sets custom filters for texture minification and magnification
// ----------------------------------------------------
void Texture::set_filters(GLenum min_filter, GLenum mag_filter)
{
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, min_filter);  // Set minification filter
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, mag_filter);  // Set magnification filter
}

// ----------------------------------------------------
// Sets custom wrap modes for the S (horizontal) and T (vertical) axes
// ----------------------------------------------------
void Texture::set_wrap(GLenum wrap_s, GLenum wrap_t)
{
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, wrap_s);  // Set the wrap mode for the S axis (horizontal)
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, wrap_t);  // Set the wrap mode for the T axis (vertical)
}

// ----------------------------------------------------
// Binds the texture to the specified texture unit
// ----------------------------------------------------
void Texture::bind(int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);  // Activate the specified texture unit (GL_TEXTURE0 + unit)

    glBindTexture(GL_TEXTURE_2D, id);  // Bind the texture to the GL_TEXTURE_2D target

    glBindSampler(unit, sampler);  // Bind the sampler to the texture unit

    glUniform1i(sampler, unit);
}


// ----------------------------------------------------
// Unbinds the texture and sampler
// ----------------------------------------------------
void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);  // Unbind the texture from the GL_TEXTURE_2D target
    glBindSampler(0, 0);  // Unbind the sampler
}

GLuint Texture::get_id() const { return id; }

int Texture::get_width() const { return width; }

int Texture::get_height() const { return height; }

GLenum Texture::get_format() const { return format; }
