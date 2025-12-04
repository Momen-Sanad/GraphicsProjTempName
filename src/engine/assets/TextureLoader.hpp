#pragma once
#include "../gl/Texture.hpp"
#include <unordered_map>


class TextureLoader
{
private:
    static std::unordered_map<std::string, Texture*> texture_cache;     // Cache loaded textures to avoid reloading

public:

    // Load with wrapping options
    static Texture* load(const std::string& path, GLenum min_filter = GL_LINEAR_MIPMAP_LINEAR, GLenum mag_filter = GL_LINEAR, GLenum wrap_s = GL_REPEAT, GLenum wrap_t = GL_REPEAT);

    // Clear all cached textures
    static void clear_cache();

    static bool is_loaded(const std::string& filepath);
    static size_t get_loaded_count();

    // Remove texture from cache
    static void unload(const std::string& path);
    static void unload_all();
};
