#include "Texture.hpp"
#include <cstdio>
#include <utility>

// ----------------------------------------------------
// Default constructor: Initializes the texture with default values
// ----------------------------------------------------
Texture::Texture() : id(0), sampler(0), width(0), height(0), format(GL_RGBA) {}

// ----------------------------------------------------
// Destructor: Cleans up the texture and its associated resources
// ----------------------------------------------------
Texture::~Texture()
{
    destroy();
}

Texture::Texture(Texture&& other) noexcept {
    *this = std::move(other);
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        destroy();

        id = other.id;
        sampler = other.sampler;
        width = other.width;
        height = other.height;
        format = other.format;

        other.id = 0;
        other.sampler = 0;
        other.width = 0;
        other.height = 0;
        other.format = GL_RGBA;
    }
    return *this;
}

void Texture::destroy()
{
    if (id != 0 && glad_glDeleteTextures) glDeleteTextures(1, &id);
    if (sampler != 0 && glad_glDeleteSamplers) glDeleteSamplers(1, &sampler);

    id = 0;
    sampler = 0;
    width = 0;
    height = 0;
    format = GL_RGBA;
}

// ----------------------------------------------------
// Creates a texture from raw data, specifying width, height, and format
// ----------------------------------------------------
void Texture::create(int textureWidth, int textureHeight, const unsigned char* data, GLenum textureFormat)
{
    destroy();

    width = textureWidth;  // Store the texture's width
    height = textureHeight;  // Store the texture's height
    format = textureFormat;  // Store the texture's format (e.g., GL_RGBA)

    if (!glad_glGenTextures || !glad_glGenSamplers) {
        return;
    }

    // Generate the OpenGL texture ID
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);  // Bind the texture to 2D targets

    // Choose internal format based on the input format
    GLenum internal_format;
    if (textureFormat == GL_RED)
        internal_format = GL_R8;  // Single channel (red) format
    else if (textureFormat == GL_RGB)
        internal_format = GL_RGB8;  // RGB format with 8 bits per channel
    else if (textureFormat == GL_RGBA)
        internal_format = GL_RGBA8;  // RGBA format with 8 bits per channel
    else
        internal_format = textureFormat;  // Use the provided format for internal format

    // Specify the texture image data
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, textureWidth, textureHeight, 0, textureFormat, GL_UNSIGNED_BYTE, data);

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
    if (glad_glGetFloatv) {
        float max_aniso = 1.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &max_aniso);  // Get the maximum anisotropy value supported by the hardware
        glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY, max_aniso);  // Set the anisotropic filtering level
    }
}

// ----------------------------------------------------
// Sets custom filters for texture minification and magnification
// ----------------------------------------------------
void Texture::set_filters(GLenum min_filter, GLenum mag_filter)
{
    if (sampler == 0 || !glad_glSamplerParameteri) return;
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, min_filter);  // Set minification filter
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, mag_filter);  // Set magnification filter
}

// ----------------------------------------------------
// Sets custom wrap modes for the S (horizontal) and T (vertical) axes
// ----------------------------------------------------
void Texture::set_wrap(GLenum wrap_s, GLenum wrap_t)
{
    if (sampler == 0 || !glad_glSamplerParameteri) return;
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, wrap_s);  // Set the wrap mode for the S axis (horizontal)
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, wrap_t);  // Set the wrap mode for the T axis (vertical)
}

// ----------------------------------------------------
// Binds the texture to the specified texture unit
// ----------------------------------------------------
void Texture::bind(int unit) const
{
    if (!glad_glActiveTexture || !glad_glBindTexture) return;
    glActiveTexture(GL_TEXTURE0 + unit);  // Activate the specified texture unit (GL_TEXTURE0 + unit)

    glBindTexture(GL_TEXTURE_2D, id);  // Bind the texture to the GL_TEXTURE_2D target

    if (sampler != 0 && glad_glBindSampler) {
        glBindSampler(unit, sampler);  // Bind the sampler to the texture unit
    }
}

void Texture::bind(int unit, const char* samp) const
{
    bind(unit);
    if (samp && glad_glGetIntegerv && glad_glGetUniformLocation && glad_glUniform1i) {
        GLint current_program = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
        if (current_program != 0) {
            GLint location = glGetUniformLocation(static_cast<GLuint>(current_program), samp);
            if (location != -1) {
                glUniform1i(location, unit);
            }
        }
    }
}

// ----------------------------------------------------
// Unbinds the texture and sampler
// ----------------------------------------------------
void Texture::unbind() const
{
    if (!glad_glBindTexture) return;
    glBindTexture(GL_TEXTURE_2D, 0);  // Unbind the texture from the GL_TEXTURE_2D target
    if (glad_glBindSampler) {
        glBindSampler(0, 0);  // Unbind the sampler
    }
}

GLuint Texture::get_id() const { return id; }

int Texture::get_width() const { return width; }

int Texture::get_height() const { return height; }

GLenum Texture::get_format() const { return format; }
