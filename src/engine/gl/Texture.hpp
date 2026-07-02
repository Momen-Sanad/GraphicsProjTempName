#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>


class Texture
{
private:
    GLuint id;
    GLuint sampler;
    int width;
    int height;
    GLenum format;

public:
    Texture();
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    // Create texture from image data
    void create(int textureWidth, int textureHeight, const unsigned char* data, GLenum textureFormat = GL_RGBA);
    void destroy();

    // Set filtering options
    void set_filters(GLenum min_filter, GLenum mag_filter);

    // Set wrapping options
    void set_wrap(GLenum wrap_s, GLenum wrap_t);

    // Bind texture to a texture unit
    void bind(int unit = 0) const;
    void bind(int unit, const char* samp) const;

    // Unbind texture
    void unbind() const;

    // Getters
    GLuint get_id() const;
    int get_width() const;
    int get_height() const;
    GLenum get_format() const;
};
