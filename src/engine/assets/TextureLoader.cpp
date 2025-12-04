#include "TextureLoader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cstdio>

std::unordered_map<std::string, Texture*> TextureLoader::texture_cache;


Texture* TextureLoader::load(const std::string& path, GLenum min_filter, GLenum mag_filter, GLenum wrap_s, GLenum wrap_t)
{
    // Check cache first
    auto it = texture_cache.find(path);
    if (it != texture_cache.end())
        return it->second;


    // Load image data
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data;
    data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (!data)
    {
        printf("Failed to load texture: %s\n", path.c_str());
        return nullptr;
    }

    // Determine format
    GLenum format = GL_RGB;
    if (channels == 1) format = GL_RED;
    else if (channels == 3) format = GL_RGB;
    else if (channels == 4) format = GL_RGBA;

    // Create texture
    Texture* texture = new Texture();
    texture->create(width, height, data, format);
    texture->set_filters(min_filter, mag_filter);
    texture->set_wrap(wrap_s, wrap_t);

    // Free image data
    stbi_image_free(data);

    // Cache texture
    texture_cache[path] = texture;

    return texture;
}

void TextureLoader::clear_cache()
{
    for (auto& pair : texture_cache)
        delete pair.second;

    texture_cache.clear();
}

size_t TextureLoader::get_loaded_count()
{
    return texture_cache.size();
}

bool TextureLoader::is_loaded(const std::string& filepath)
{
    auto it = texture_cache.find(filepath);
    return it != texture_cache.end();
}

void TextureLoader::unload(const std::string& path)
{
    auto it = texture_cache.find(path);

    if (it != texture_cache.end())
    {
        delete it->second;
        texture_cache.erase(it);
    }
}

void TextureLoader::unload_all()
{
    for (auto& pair : texture_cache)
        delete pair.second;

    texture_cache.clear();
}
