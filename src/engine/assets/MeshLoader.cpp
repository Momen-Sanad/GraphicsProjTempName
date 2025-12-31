#include "MeshLoader.hpp"
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tinygltf/tiny_gltf.h"
#include <glm/gtc/type_ptr.hpp>

Mesh* MeshLoader::get_mesh(const std::string& filepath)
{
    auto it = mesh_cache.find(filepath);

    if (it != mesh_cache.end())
        return it->second;
    else
    {
        std::cout << "No registered mesh with filepath: '" << filepath << "'" << std::endl;
        return nullptr;
    }
}

size_t hash_combine(size_t seed, size_t h) {
    return seed ^ (h + 0x9e3779b9) + (seed << 6) + (seed >> 2);
}

struct VertexHash
{
    size_t operator()(const Vertex& v) const
    {
        size_t seed = 0;
        seed = hash_combine(seed, std::hash<float>()(v.position.x));
        seed = hash_combine(seed, std::hash<float>()(v.position.y));
        seed = hash_combine(seed, std::hash<float>()(v.position.z));
        seed = hash_combine(seed, std::hash<uint8_t>()(v.color.r));
        seed = hash_combine(seed, std::hash<uint8_t>()(v.color.g));
        seed = hash_combine(seed, std::hash<uint8_t>()(v.color.b));
        seed = hash_combine(seed, std::hash<uint8_t>()(v.color.a));
        seed = hash_combine(seed, std::hash<float>()(v.tex_coord.s));
        seed = hash_combine(seed, std::hash<float>()(v.tex_coord.t));
        return seed;
    }
};


// suppoets loading a mesh from an .obj file
Mesh* MeshLoader::load_obj(const char* path)
{
    // Check file extension
    std::string filepath = path;
    size_t dotPos = filepath.find_last_of('.');
    std::string ext;
    if (dotPos != std::string::npos)
        ext = filepath.substr(dotPos);
    if (ext != ".obj")
    {
        printf("Unsupported mesh format: %s\n", ext.c_str());
        return {};
    }

    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    // Map to remove duplicate vertices
    std::unordered_map<Vertex, uint16_t, VertexHash> vertex_map;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    // Load OBJ file
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path))
    {
        printf("Failed to load obj file \"%s\" due to error: %s\n", path, err.c_str());
        return {};
    }
    if (!warn.empty())
        printf("Warning while loading obj file \"%s\" due to error: %s\n", path, warn.c_str());

    // Process all shapes
    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex =
            {
                .position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                },
                .color = {
                    uint8_t(attrib.colors[3 * index.vertex_index + 0] * 255),
                    uint8_t(attrib.colors[3 * index.vertex_index + 1] * 255),
                    uint8_t(attrib.colors[3 * index.vertex_index + 2] * 255),
                    255
                },
                .tex_coord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                }
            };

            // Check if vertex already exists
            auto it = vertex_map.find(vertex);
            if (it == vertex_map.end())
            {
                // Add new unique vertex
                auto new_vertex_index = static_cast<uint16_t>(vertices.size());
                vertex_map[vertex] = new_vertex_index;
                indices.push_back(new_vertex_index);
                vertices.push_back(vertex);
            }
            else
                // Reuse existing vertex
                indices.push_back(it->second);
        }
    }

    Mesh* mesh = new Mesh();
    mesh->create(vertices, indices);
    return mesh;
}


ModelData* MeshLoader::load_gltf(const char* path)
{
    std::string filepath = path;

    tinygltf::Model gltf_model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool ret = false;

    // Check file extension
    if (filepath.find(".glb") != std::string::npos) {
        ret = loader.LoadBinaryFromFile(&gltf_model, &err, &warn, filepath);
    }
    else {
        ret = loader.LoadASCIIFromFile(&gltf_model, &err, &warn, filepath);
    }

    if (!warn.empty()) {
        std::cout << "Warning: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << "Error: " << err << std::endl;
    }

    if (!ret) {
        std::cerr << "Failed to load GLTF: " << filepath << std::endl;
        return nullptr;
    }

    ModelData* model_data = new ModelData();

    // Load skeleton if present
    if (!gltf_model.skins.empty()) {
        model_data->skeleton = std::make_shared<Skeleton>();
        const tinygltf::Skin& skin = gltf_model.skins[0];

        // Load inverse bind matrices
        std::vector<glm::mat4> inverse_bind_matrices;
        if (skin.inverseBindMatrices >= 0) {
            const tinygltf::Accessor& accessor = gltf_model.accessors[skin.inverseBindMatrices];
            const tinygltf::BufferView& buffer_view = gltf_model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = gltf_model.buffers[buffer_view.buffer];

            const float* data = reinterpret_cast<const float*>(
                &buffer.data[buffer_view.byteOffset + accessor.byteOffset]);

            for (size_t i = 0; i < accessor.count; ++i) {
                glm::mat4 mat;
                memcpy(glm::value_ptr(mat), &data[i * 16], 16 * sizeof(float));
                inverse_bind_matrices.push_back(mat);
            }
        }

        // Build bone hierarchy
        for (size_t i = 0; i < skin.joints.size(); ++i) {
            int joint_index = skin.joints[i];
            const tinygltf::Node& node = gltf_model.nodes[joint_index];

            // Find parent
            int parent_id = -1;
            for (size_t j = 0; j < gltf_model.nodes.size(); ++j) {
                for (int child : gltf_model.nodes[j].children) {
                    if (child == joint_index) {
                        // Check if parent is a joint
                        for (size_t k = 0; k < skin.joints.size(); ++k) {
                            if (skin.joints[k] == static_cast<int>(j)) {
                                parent_id = static_cast<int>(k);
                                break;
                            }
                        }
                        break;
                    }
                }
                if (parent_id >= 0) break;
            }

            glm::mat4 inverse_bind = (i < inverse_bind_matrices.size())
                ? inverse_bind_matrices[i]
                : glm::mat4(1.0f);

            model_data->skeleton->add_bone(node.name, parent_id, inverse_bind);
        }

        // Set global inverse transform (root node transform)
        if (gltf_model.scenes.size() > 0) {
            model_data->skeleton->set_global_inverse_transform(glm::mat4(1.0f));
        }
    }

    // Load meshes
    for (const auto& gltf_mesh : gltf_model.meshes) {
        for (const auto& primitive : gltf_mesh.primitives) {
            std::vector<Vertex> vertices;
            std::vector<SkeletonVertex> skel_vertices;
            std::vector<uint16_t> indices;

            size_t vertex_count = 0;

            // Load positions (required)
            if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                const tinygltf::Accessor& accessor = gltf_model.accessors[primitive.attributes.at("POSITION")];
                const tinygltf::BufferView& buffer_view = gltf_model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = gltf_model.buffers[buffer_view.buffer];

                const float* positions = reinterpret_cast<const float*>(
                    &buffer.data[buffer_view.byteOffset + accessor.byteOffset]);

                vertex_count = accessor.count;
                vertices.resize(vertex_count);

                for (size_t i = 0; i < vertex_count; ++i) {
                    vertices[i].position = glm::vec3(
                        positions[i * 3 + 0],
                        positions[i * 3 + 1],
                        positions[i * 3 + 2]
                    );
                    vertices[i].color = WHITE;  // Default color
                    vertices[i].tex_coord = glm::vec2(0.0f);  // Default UV
                }
            }

            // Load texture coordinates
            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                const tinygltf::Accessor& accessor = gltf_model.accessors[primitive.attributes.at("TEXCOORD_0")];
                const tinygltf::BufferView& buffer_view = gltf_model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = gltf_model.buffers[buffer_view.buffer];

                const float* tex_coords = reinterpret_cast<const float*>(
                    &buffer.data[buffer_view.byteOffset + accessor.byteOffset]);

                for (size_t i = 0; i < vertex_count && i < accessor.count; ++i) {
                    vertices[i].tex_coord = glm::vec2(
                        tex_coords[i * 2 + 0],
                        tex_coords[i * 2 + 1]
                    );
                }
            }

            // Load skeleton data if present
            bool has_skeleton_data =
                primitive.attributes.find("NORMAL") != primitive.attributes.end() ||
                primitive.attributes.find("JOINTS_0") != primitive.attributes.end() ||
                primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end();

            if (has_skeleton_data) {
                skel_vertices.resize(vertex_count);

                // Initialize with defaults
                for (size_t i = 0; i < vertex_count; ++i) {
                    skel_vertices[i].normal = glm::vec3(0.0f, 1.0f, 0.0f);
                    skel_vertices[i].bone_ids = glm::ivec4(-1);
                    skel_vertices[i].bone_weights = glm::vec4(0.0f);
                }

                // Load normals
                if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                    const tinygltf::Accessor& accessor = gltf_model.accessors[primitive.attributes.at("NORMAL")];
                    const tinygltf::BufferView& buffer_view = gltf_model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer = gltf_model.buffers[buffer_view.buffer];

                    const float* normals = reinterpret_cast<const float*>(
                        &buffer.data[buffer_view.byteOffset + accessor.byteOffset]);

                    for (size_t i = 0; i < vertex_count && i < accessor.count; ++i) {
                        skel_vertices[i].normal = glm::vec3(
                            normals[i * 3 + 0],
                            normals[i * 3 + 1],
                            normals[i * 3 + 2]
                        );
                    }
                }

                // Load bone weights and indices
                if (model_data->skeleton &&
                    primitive.attributes.find("JOINTS_0") != primitive.attributes.end() &&
                    primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end()) {

                    // Load joint indices
                    const tinygltf::Accessor& joint_accessor = gltf_model.accessors[primitive.attributes.at("JOINTS_0")];
                    const tinygltf::BufferView& joint_buffer_view = gltf_model.bufferViews[joint_accessor.bufferView];
                    const tinygltf::Buffer& joint_buffer = gltf_model.buffers[joint_buffer_view.buffer];

                    // Load weights
                    const tinygltf::Accessor& weight_accessor = gltf_model.accessors[primitive.attributes.at("WEIGHTS_0")];
                    const tinygltf::BufferView& weight_buffer_view = gltf_model.bufferViews[weight_accessor.bufferView];
                    const tinygltf::Buffer& weight_buffer = gltf_model.buffers[weight_buffer_view.buffer];

                    for (size_t i = 0; i < vertex_count; ++i) {
                        // Joint indices (can be UNSIGNED_BYTE or UNSIGNED_SHORT)
                        if (joint_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                            const uint16_t* joints = reinterpret_cast<const uint16_t*>(
                                &joint_buffer.data[joint_buffer_view.byteOffset + joint_accessor.byteOffset]);
                            skel_vertices[i].bone_ids = glm::ivec4(
                                joints[i * 4 + 0],
                                joints[i * 4 + 1],
                                joints[i * 4 + 2],
                                joints[i * 4 + 3]
                            );
                        }
                        else {
                            const uint8_t* joints = reinterpret_cast<const uint8_t*>(
                                &joint_buffer.data[joint_buffer_view.byteOffset + joint_accessor.byteOffset]);
                            skel_vertices[i].bone_ids = glm::ivec4(
                                joints[i * 4 + 0],
                                joints[i * 4 + 1],
                                joints[i * 4 + 2],
                                joints[i * 4 + 3]
                            );
                        }

                        // Weights
                        const float* weights = reinterpret_cast<const float*>(
                            &weight_buffer.data[weight_buffer_view.byteOffset + weight_accessor.byteOffset]);
                        skel_vertices[i].bone_weights = glm::vec4(
                            weights[i * 4 + 0],
                            weights[i * 4 + 1],
                            weights[i * 4 + 2],
                            weights[i * 4 + 3]
                        );
                    }
                }
            }

            // Load indices
            if (primitive.indices >= 0) {
                const tinygltf::Accessor& accessor = gltf_model.accessors[primitive.indices];
                const tinygltf::BufferView& buffer_view = gltf_model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = gltf_model.buffers[buffer_view.buffer];

                indices.resize(accessor.count);

                if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                    const uint16_t* data = reinterpret_cast<const uint16_t*>(
                        &buffer.data[buffer_view.byteOffset + accessor.byteOffset]);
                    for (size_t i = 0; i < accessor.count; ++i) {
                        indices[i] = data[i];
                    }
                }
                else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                    const uint32_t* data = reinterpret_cast<const uint32_t*>(
                        &buffer.data[buffer_view.byteOffset + accessor.byteOffset]);
                    for (size_t i = 0; i < accessor.count; ++i) {
                        indices[i] = static_cast<uint16_t>(data[i]);
                    }
                }
                else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
                    const uint8_t* data = reinterpret_cast<const uint8_t*>(
                        &buffer.data[buffer_view.byteOffset + accessor.byteOffset]);
                    for (size_t i = 0; i < accessor.count; ++i) {
                        indices[i] = static_cast<uint16_t>(data[i]);
                    }
                }
            }

            // Create mesh using the Mesh constructor
            SkinnedMesh mesh(vertices, indices, skel_vertices);
            model_data->meshes.push_back(std::move(mesh));
        }
    }

    // Load animations
    for (const auto& gltf_anim : gltf_model.animations) {
        auto anim_clip = std::make_shared<AnimationClip>();
        anim_clip->set_name(gltf_anim.name);

        float max_time = 0.0f;

        for (const auto& channel : gltf_anim.channels) {
            const tinygltf::AnimationSampler& sampler = gltf_anim.samplers[channel.sampler];

            // Get time values
            const tinygltf::Accessor& time_accessor = gltf_model.accessors[sampler.input];
            const tinygltf::BufferView& time_buffer_view = gltf_model.bufferViews[time_accessor.bufferView];
            const tinygltf::Buffer& time_buffer = gltf_model.buffers[time_buffer_view.buffer];
            const float* times = reinterpret_cast<const float*>(
                &time_buffer.data[time_buffer_view.byteOffset + time_accessor.byteOffset]);

            // Get output values
            const tinygltf::Accessor& output_accessor = gltf_model.accessors[sampler.output];
            const tinygltf::BufferView& output_buffer_view = gltf_model.bufferViews[output_accessor.bufferView];
            const tinygltf::Buffer& output_buffer = gltf_model.buffers[output_buffer_view.buffer];
            const float* outputs = reinterpret_cast<const float*>(
                &output_buffer.data[output_buffer_view.byteOffset + output_accessor.byteOffset]);

            // Find bone index
            int bone_id = -1;
            if (model_data->skeleton) {
                for (int i = 0; i < model_data->skeleton->get_bone_count(); ++i) {
                    if (gltf_model.nodes[channel.target_node].name == model_data->skeleton->get_bone(i).name) {
                        bone_id = i;
                        break;
                    }
                }
            }

            if (bone_id < 0) continue;

            // Find or create bone animation
            BoneAnimation* bone_anim = nullptr;
            for (auto& ba : anim_clip->get_bone_animations()) {
                if (ba.bone_id == bone_id) {
                    bone_anim = const_cast<BoneAnimation*>(&ba);
                    break;
                }
            }

            if (!bone_anim) {
                BoneAnimation new_bone_anim;
                new_bone_anim.bone_id = bone_id;
                anim_clip->add_bone_animation(new_bone_anim);
                bone_anim = const_cast<BoneAnimation*>(&anim_clip->get_bone_animations().back());
            }

            // Add keyframes based on path
            if (channel.target_path == "translation") {
                for (size_t i = 0; i < time_accessor.count; ++i) {
                    PositionKeyframe key;
                    key.time = times[i];
                    key.position = glm::vec3(outputs[i * 3 + 0], outputs[i * 3 + 1], outputs[i * 3 + 2]);
                    bone_anim->position_keys.push_back(key);
                    max_time = std::max(max_time, key.time);
                }
            }
            else if (channel.target_path == "rotation") {
                for (size_t i = 0; i < time_accessor.count; ++i) {
                    RotationKeyframe key;
                    key.time = times[i];
                    key.rotation = glm::quat(outputs[i * 4 + 3], outputs[i * 4 + 0],
                        outputs[i * 4 + 1], outputs[i * 4 + 2]);
                    bone_anim->rotation_keys.push_back(key);
                    max_time = std::max(max_time, key.time);
                }
            }
            else if (channel.target_path == "scale") {
                for (size_t i = 0; i < time_accessor.count; ++i) {
                    ScaleKeyframe key;
                    key.time = times[i];
                    key.scale = glm::vec3(outputs[i * 3 + 0], outputs[i * 3 + 1], outputs[i * 3 + 2]);
                    bone_anim->scale_keys.push_back(key);
                    max_time = std::max(max_time, key.time);
                }
            }
        }

        anim_clip->set_duration(max_time);
        model_data->animations.push_back(std::move(anim_clip));
    }

    // Load textures
    for (const auto& gltf_texture : gltf_model.textures) {
        if (gltf_texture.source >= 0 && gltf_texture.source < static_cast<int>(gltf_model.images.size())) {
            const tinygltf::Image& image = gltf_model.images[gltf_texture.source];
            
            if (!image.image.empty()) {
                auto texture = std::make_shared<Texture>();
                
                GLenum format = GL_RGBA;
                if (image.component == 3) {
                    format = GL_RGB;
                } else if (image.component == 1) {
                    format = GL_RED;
                }
                
                texture->create(image.width, image.height, image.image.data(), format);
                texture->set_filters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
                texture->set_wrap(GL_REPEAT, GL_REPEAT);
                
                model_data->textures.push_back(texture);
                std::cout << "Loaded texture: " << image.uri << " (" << image.width << "x" << image.height << ")" << std::endl;
            }
        }
    }

    return model_data;
}

// Initialize static members
std::unordered_map<std::string, Mesh*> MeshLoader::mesh_cache;

// Memory management
void MeshLoader::unload(const std::string& filepath)
{
    auto cache_it = mesh_cache.find(filepath);
    if (cache_it != mesh_cache.end())
    {
        // Simply delete the Mesh object
        delete cache_it->second;
        mesh_cache.erase(cache_it);

        std::cout << "Unloaded mesh from cache: " << filepath << std::endl;
    }
    else
    {
        std::cout << "Mesh not found in cache: " << filepath << std::endl;
    }
}

void MeshLoader::unload_all()
{
    std::cout << "Unloading all cached meshes..." << std::endl;

    // Delete all Mesh objects
    for (auto& pair : mesh_cache)
    {
        delete pair.second;
    }

    // Clear the cache
    mesh_cache.clear();

    std::cout << "All cached meshes unloaded. Total: " << mesh_cache.size() << " meshes remaining." << std::endl;
}


bool MeshLoader::is_loaded(const std::string& filepath)
{
    return mesh_cache.find(filepath) != mesh_cache.end();
}

size_t MeshLoader::get_loaded_count()
{
    return mesh_cache.size();
}