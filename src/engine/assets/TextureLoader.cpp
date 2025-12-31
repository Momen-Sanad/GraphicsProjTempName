#include "TextureLoader.hpp"
#define STB_IMAGE_IMPLEMENTATION  // Defines STB_IMAGE_IMPLEMENTATION for stb_image.h to work properly
#include "stb_image.h"
#include <cstdio>

// ------------------------------------------------------
// Texture Cache
// ------------------------------------------------------
std::unordered_map<std::string, Texture*> TextureLoader::texture_cache;  // Cache to store loaded textures

// ------------------------------------------------------
// Load a texture from file (if not already cached)
// ------------------------------------------------------
Texture* TextureLoader::load(const std::string& path, GLenum min_filter, GLenum mag_filter, GLenum wrap_s, GLenum wrap_t)
{
    // Check if the texture is already in the cache
    auto it = texture_cache.find(path);
    if (it != texture_cache.end())
        return it->second;  // If found in cache, return the cached texture

    // Load image data using stb_image
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);  // Flip the image vertically (common in OpenGL)
    stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);  // Load the image into data

    // If loading fails, print an error and return nullptr
    if (!data)
    {
        printf("Failed to load texture: %s\n", path.c_str());
        return nullptr;
    }

    // Determine the image format based on the number of channels in the image
    GLenum format = GL_RGB;  // Default format
    if (channels == 1) format = GL_RED;  // Single channel image (grayscale)
    else if (channels == 3) format = GL_RGB;  // RGB image (no alpha)
    else if (channels == 4) format = GL_RGBA;  // RGBA image (with alpha)

    // Create a new texture and initialize it with the image data
    Texture* texture = new Texture();
    texture->create(width, height, data, format);  // Create the texture with the image dimensions and data
    texture->set_filters(min_filter, mag_filter);  // Set texture minification and magnification filters
    texture->set_wrap(wrap_s, wrap_t);  // Set the texture wrapping parameters for S and T coordinates

    // Free the image data as it is no longer needed after texture creation
    stbi_image_free(data);

    // Cache the texture with its file path as the key
    texture_cache[path] = texture;

    return texture;  // Return the newly loaded texture
}

// ------------------------------------------------------
// Clear the texture cache by deleting all loaded textures
// ------------------------------------------------------
void TextureLoader::clear_cache()
{
    // Iterate through all textures in the cache and delete them
    for (auto& pair : texture_cache)
        delete pair.second;

    texture_cache.clear();  // Clear the cache
}

// ------------------------------------------------------
// Get the number of textures currently loaded in the cache
// ------------------------------------------------------
size_t TextureLoader::get_loaded_count()
{
    return texture_cache.size();  // Return the number of loaded textures in the cache
}

// ------------------------------------------------------
// Check if a texture is already loaded and in the cache
// ------------------------------------------------------
bool TextureLoader::is_loaded(const std::string& filepath)
{
    auto it = texture_cache.find(filepath);  // Look for the texture in the cache
    return it != texture_cache.end();  // Return true if found, false otherwise
}

// ------------------------------------------------------
// Unload a texture from the cache by its file path
// ------------------------------------------------------
void TextureLoader::unload(const std::string& path)
{
    // Find the texture in the cache by its file path
    auto it = texture_cache.find(path);

    // If found, delete the texture and remove it from the cache
    if (it != texture_cache.end())
    {
        delete it->second;  // Delete the texture object
        texture_cache.erase(it);  // Remove it from the cache
    }
}

// ------------------------------------------------------
// Unload all textures from the cache and clear the cache
// ------------------------------------------------------
void TextureLoader::unload_all()
{
    // Iterate through all textures in the cache and delete them
    for (auto& pair : texture_cache)
        delete pair.second;

    texture_cache.clear();  // Clear the cache
}
