#ifndef TEXTURE_LOADER_HPP
#define TEXTURE_LOADER_HPP
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <string>
#include <glad/gl.h>

class Texture {
public:
    Texture();
    ~Texture();

    // Loads the texture from a file and returns the texture ID
    bool load(const std::string& file_path);

    // Returns the OpenGL texture ID
    GLuint get_texture_id() const { return texture_id; }

private:
    GLuint texture_id;  // OpenGL texture ID
};

#endif // TEXTURE_LOADER_HPP
