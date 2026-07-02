// potential file to load for animations + kf + bone locations: file.dae
// consider open source lib for fpx
#include "MeshLoader.hpp"
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tinygltf/tiny_gltf.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <algorithm>
#include <cstring>


std::shared_ptr<Mesh> MeshLoader::get_mesh(const std::string& filepath)
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

namespace {
    template <typename T>
    T read_pod(const unsigned char* ptr) {
        T value{};
        std::memcpy(&value, ptr, sizeof(T));
        return value;
    }

    size_t component_size(int component_type) {
        switch (component_type) {
        case TINYGLTF_COMPONENT_TYPE_BYTE:
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            return 1;
        case TINYGLTF_COMPONENT_TYPE_SHORT:
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            return 2;
        case TINYGLTF_COMPONENT_TYPE_INT:
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            return 4;
        case TINYGLTF_COMPONENT_TYPE_DOUBLE:
            return 8;
        default:
            return 0;
        }
    }

    size_t component_count(int accessor_type) {
        switch (accessor_type) {
        case TINYGLTF_TYPE_SCALAR:
            return 1;
        case TINYGLTF_TYPE_VEC2:
            return 2;
        case TINYGLTF_TYPE_VEC3:
            return 3;
        case TINYGLTF_TYPE_VEC4:
            return 4;
        case TINYGLTF_TYPE_MAT4:
            return 16;
        default:
            return 0;
        }
    }

    size_t accessor_stride(const tinygltf::Model& model, const tinygltf::Accessor& accessor) {
        if (accessor.bufferView < 0 ||
            accessor.bufferView >= static_cast<int>(model.bufferViews.size())) {
            return 0;
        }

        const tinygltf::BufferView& buffer_view = model.bufferViews[accessor.bufferView];
        const size_t packed_size = component_size(accessor.componentType) * component_count(accessor.type);
        return buffer_view.byteStride > 0 ? buffer_view.byteStride : packed_size;
    }

    const unsigned char* accessor_element_ptr(
        const tinygltf::Model& model,
        const tinygltf::Accessor& accessor,
        size_t index)
    {
        if (accessor.bufferView < 0 ||
            accessor.bufferView >= static_cast<int>(model.bufferViews.size())) {
            return nullptr;
        }

        const tinygltf::BufferView& buffer_view = model.bufferViews[accessor.bufferView];
        if (buffer_view.buffer < 0 ||
            buffer_view.buffer >= static_cast<int>(model.buffers.size())) {
            return nullptr;
        }

        const tinygltf::Buffer& buffer = model.buffers[buffer_view.buffer];
        const size_t stride = accessor_stride(model, accessor);
        const size_t elem_size = component_size(accessor.componentType) * component_count(accessor.type);
        if (stride == 0 || elem_size == 0) {
            return nullptr;
        }

        const size_t offset = buffer_view.byteOffset + accessor.byteOffset + index * stride;
        if (offset > buffer.data.size() || elem_size > buffer.data.size() - offset) {
            return nullptr;
        }

        return buffer.data.data() + offset;
    }

    float read_component_float(const unsigned char* ptr, int component_type, bool normalized) {
        switch (component_type) {
        case TINYGLTF_COMPONENT_TYPE_BYTE: {
            const int8_t value = read_pod<int8_t>(ptr);
            return normalized ? std::max(value / 127.0f, -1.0f) : static_cast<float>(value);
        }
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
            const uint8_t value = read_pod<uint8_t>(ptr);
            return normalized ? value / 255.0f : static_cast<float>(value);
        }
        case TINYGLTF_COMPONENT_TYPE_SHORT: {
            const int16_t value = read_pod<int16_t>(ptr);
            return normalized ? std::max(value / 32767.0f, -1.0f) : static_cast<float>(value);
        }
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
            const uint16_t value = read_pod<uint16_t>(ptr);
            return normalized ? value / 65535.0f : static_cast<float>(value);
        }
        case TINYGLTF_COMPONENT_TYPE_INT:
            return static_cast<float>(read_pod<int32_t>(ptr));
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            return static_cast<float>(read_pod<uint32_t>(ptr));
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            return read_pod<float>(ptr);
        case TINYGLTF_COMPONENT_TYPE_DOUBLE:
            return static_cast<float>(read_pod<double>(ptr));
        default:
            return 0.0f;
        }
    }

    uint32_t read_component_uint(const unsigned char* ptr, int component_type) {
        switch (component_type) {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            return read_pod<uint8_t>(ptr);
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            return read_pod<uint16_t>(ptr);
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            return read_pod<uint32_t>(ptr);
        case TINYGLTF_COMPONENT_TYPE_BYTE:
            return static_cast<uint32_t>(std::max<int8_t>(read_pod<int8_t>(ptr), 0));
        case TINYGLTF_COMPONENT_TYPE_SHORT:
            return static_cast<uint32_t>(std::max<int16_t>(read_pod<int16_t>(ptr), 0));
        case TINYGLTF_COMPONENT_TYPE_INT:
            return static_cast<uint32_t>(std::max<int32_t>(read_pod<int32_t>(ptr), 0));
        default:
            return 0;
        }
    }

    glm::vec4 read_accessor_vec4(
        const tinygltf::Model& model,
        const tinygltf::Accessor& accessor,
        size_t index,
        const glm::vec4& fallback = glm::vec4(0.0f))
    {
        const unsigned char* ptr = accessor_element_ptr(model, accessor, index);
        if (!ptr) {
            return fallback;
        }

        glm::vec4 result = fallback;
        const size_t count = std::min<size_t>(4, component_count(accessor.type));
        const size_t size = component_size(accessor.componentType);
        for (size_t component = 0; component < count; ++component) {
            result[static_cast<int>(component)] =
                read_component_float(ptr + component * size, accessor.componentType, accessor.normalized);
        }
        return result;
    }

    glm::ivec4 read_accessor_ivec4(
        const tinygltf::Model& model,
        const tinygltf::Accessor& accessor,
        size_t index,
        const glm::ivec4& fallback = glm::ivec4(0))
    {
        const unsigned char* ptr = accessor_element_ptr(model, accessor, index);
        if (!ptr) {
            return fallback;
        }

        glm::ivec4 result = fallback;
        const size_t count = std::min<size_t>(4, component_count(accessor.type));
        const size_t size = component_size(accessor.componentType);
        for (size_t component = 0; component < count; ++component) {
            result[static_cast<int>(component)] =
                static_cast<int>(read_component_uint(ptr + component * size, accessor.componentType));
        }
        return result;
    }

    MeshIndex read_accessor_index(
        const tinygltf::Model& model,
        const tinygltf::Accessor& accessor,
        size_t index)
    {
        const unsigned char* ptr = accessor_element_ptr(model, accessor, index);
        if (!ptr) {
            return 0;
        }
        return static_cast<MeshIndex>(read_component_uint(ptr, accessor.componentType));
    }

    glm::mat4 read_accessor_mat4(
        const tinygltf::Model& model,
        const tinygltf::Accessor& accessor,
        size_t index)
    {
        const unsigned char* ptr = accessor_element_ptr(model, accessor, index);
        if (!ptr) {
            return glm::mat4(1.0f);
        }

        glm::mat4 matrix(1.0f);
        const size_t size = component_size(accessor.componentType);
        for (size_t component = 0; component < 16; ++component) {
            matrix[static_cast<int>(component / 4)][static_cast<int>(component % 4)] =
                read_component_float(ptr + component * size, accessor.componentType, accessor.normalized);
        }
        return matrix;
    }

    glm::mat4 read_node_transform(const tinygltf::Node& node) {
        if (node.matrix.size() == 16) {
            glm::mat4 matrix(1.0f);
            for (size_t component = 0; component < 16; ++component) {
                matrix[static_cast<int>(component / 4)][static_cast<int>(component % 4)] =
                    static_cast<float>(node.matrix[component]);
            }
            return matrix;
        }

        glm::vec3 translation(0.0f);
        if (node.translation.size() == 3) {
            translation = glm::vec3(
                static_cast<float>(node.translation[0]),
                static_cast<float>(node.translation[1]),
                static_cast<float>(node.translation[2]));
        }

        glm::quat rotation(1.0f, 0.0f, 0.0f, 0.0f);
        if (node.rotation.size() == 4) {
            rotation = glm::quat(
                static_cast<float>(node.rotation[3]),
                static_cast<float>(node.rotation[0]),
                static_cast<float>(node.rotation[1]),
                static_cast<float>(node.rotation[2]));
        }

        glm::vec3 scale(1.0f);
        if (node.scale.size() == 3) {
            scale = glm::vec3(
                static_cast<float>(node.scale[0]),
                static_cast<float>(node.scale[1]),
                static_cast<float>(node.scale[2]));
        }

        return glm::translate(glm::mat4(1.0f), translation) *
               glm::toMat4(rotation) *
               glm::scale(glm::mat4(1.0f), scale);
    }

    bool has_required_unsupported_extensions(const tinygltf::Model& model) {
        if (!model.extensionsRequired.empty()) {
            std::cerr << "Unsupported required glTF extension: "
                      << model.extensionsRequired.front() << std::endl;
            return true;
        }

        return false;
    }
}


// supports loading a mesh from an .obj file
std::shared_ptr<Mesh> MeshLoader::readObjMesh(const char* path)
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
        return nullptr;
    }

    std::vector<Vertex> vertices;
    std::vector<MeshIndex> indices;

    // Map to remove duplicate vertices
    std::unordered_map<Vertex, MeshIndex, VertexHash> vertex_map;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    // Load OBJ file
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path))
    {
        printf("Failed to load obj file \"%s\" due to error: %s\n", path, err.c_str());
        return nullptr;
    }
    if (!warn.empty())
        printf("Warning while loading obj file \"%s\" due to error: %s\n", path, warn.c_str());

    // Process all shapes
    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};
            vertex.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };
            vertex.color = WHITE;
            vertex.tex_coord = glm::vec2(0.0f);

            if (index.vertex_index >= 0 &&
                (3 * index.vertex_index + 2) < static_cast<int>(attrib.colors.size())) {
                vertex.color = {
                    static_cast<uint8_t>(glm::clamp(attrib.colors[3 * index.vertex_index + 0], 0.0f, 1.0f) * 255.0f),
                    static_cast<uint8_t>(glm::clamp(attrib.colors[3 * index.vertex_index + 1], 0.0f, 1.0f) * 255.0f),
                    static_cast<uint8_t>(glm::clamp(attrib.colors[3 * index.vertex_index + 2], 0.0f, 1.0f) * 255.0f),
                    255
                };
            }

            if (index.texcoord_index >= 0 &&
                (2 * index.texcoord_index + 1) < static_cast<int>(attrib.texcoords.size())) {
                vertex.tex_coord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                };
            }

            // Check if vertex already exists
            auto it = vertex_map.find(vertex);
            if (it == vertex_map.end())
            {
                // Add new unique vertex
                auto new_vertex_index = static_cast<MeshIndex>(vertices.size());
                vertex_map[vertex] = new_vertex_index;
                indices.push_back(new_vertex_index);
                vertices.push_back(vertex);
            }
            else
                // Reuse existing vertex
                indices.push_back(it->second);
        }
    }

    auto mesh = std::make_shared<Mesh>();
    mesh->create(vertices, indices);
    return mesh;
}

// Initialize static members
std::unordered_map<std::string, std::shared_ptr<Mesh>> MeshLoader::mesh_cache;

// Memory management
void MeshLoader::unload(const std::string& filepath)
{
    auto cache_it = mesh_cache.find(filepath);
    if (cache_it != mesh_cache.end())
    {
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

std::shared_ptr<ModelData> MeshLoader::readGltfModel(const char* path)
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

    if (has_required_unsupported_extensions(gltf_model)) {
        std::cerr << "Failed to load GLTF because it requires unsupported extensions: "
                  << filepath << std::endl;
        return nullptr;
    }

    auto model_data = std::make_shared<ModelData>();
    model_data->defaultScene = gltf_model.defaultScene;

    model_data->materialNames.reserve(gltf_model.materials.size());
    for (size_t materialIndex = 0; materialIndex < gltf_model.materials.size(); ++materialIndex) {
        const std::string& name = gltf_model.materials[materialIndex].name;
        model_data->materialNames.push_back(
            name.empty() ? "material_" + std::to_string(materialIndex) : name);
    }

    model_data->nodes.reserve(gltf_model.nodes.size());
    for (const tinygltf::Node& gltf_node : gltf_model.nodes) {
        ImportedModelNode node;
        node.name = gltf_node.name;
        node.localTransform = read_node_transform(gltf_node);
        node.meshIndex = gltf_node.mesh;
        node.skinIndex = gltf_node.skin;
        node.children = gltf_node.children;
        model_data->nodes.push_back(std::move(node));
    }

    for (size_t parentIndex = 0; parentIndex < gltf_model.nodes.size(); ++parentIndex) {
        for (int childIndex : gltf_model.nodes[parentIndex].children) {
            if (childIndex >= 0 && static_cast<size_t>(childIndex) < model_data->nodes.size()) {
                model_data->nodes[static_cast<size_t>(childIndex)].parentIndex =
                    static_cast<int>(parentIndex);
            }
        }
    }

    model_data->scenes.reserve(gltf_model.scenes.size());
    for (const tinygltf::Scene& gltf_scene : gltf_model.scenes) {
        ImportedModelScene scene;
        scene.name = gltf_scene.name;
        scene.rootNodes = gltf_scene.nodes;
        model_data->scenes.push_back(std::move(scene));
    }

    // Load skeleton if present
    if (!gltf_model.skins.empty()) {
        model_data->skeleton = std::make_shared<Skeleton>();
        const tinygltf::Skin& skin = gltf_model.skins[0];

        // Load inverse bind matrices
        std::vector<glm::mat4> inverse_bind_matrices;
        if (skin.inverseBindMatrices >= 0) {
            const tinygltf::Accessor& accessor = gltf_model.accessors[skin.inverseBindMatrices];

            for (size_t i = 0; i < accessor.count; ++i) {
                inverse_bind_matrices.push_back(read_accessor_mat4(gltf_model, accessor, i));
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
    model_data->meshPrimitiveIndices.resize(gltf_model.meshes.size());
    for (size_t gltfMeshIndex = 0; gltfMeshIndex < gltf_model.meshes.size(); ++gltfMeshIndex) {
        const auto& gltf_mesh = gltf_model.meshes[gltfMeshIndex];
        for (const auto& primitive : gltf_mesh.primitives) {
            std::vector<Vertex> vertices;
            std::vector<SkeletonVertex> skel_vertices;
            std::vector<MeshIndex> indices;

            size_t vertex_count = 0;

            // Load positions (required)
            if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                const tinygltf::Accessor& accessor = gltf_model.accessors[primitive.attributes.at("POSITION")];

                vertex_count = accessor.count;
                vertices.resize(vertex_count);

                for (size_t i = 0; i < vertex_count; ++i) {
                    vertices[i].position = glm::vec3(read_accessor_vec4(gltf_model, accessor, i));
                    vertices[i].color = WHITE;  // Default color
                    vertices[i].tex_coord = glm::vec2(0.0f);  // Default UV
                }
            }

            // Load texture coordinates
            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                const tinygltf::Accessor& accessor = gltf_model.accessors[primitive.attributes.at("TEXCOORD_0")];

                for (size_t i = 0; i < vertex_count && i < accessor.count; ++i) {
                    vertices[i].tex_coord = glm::vec2(read_accessor_vec4(gltf_model, accessor, i));
                }
            }

            if (primitive.attributes.find("COLOR_0") != primitive.attributes.end()) {
                const tinygltf::Accessor& accessor = gltf_model.accessors[primitive.attributes.at("COLOR_0")];

                for (size_t i = 0; i < vertex_count && i < accessor.count; ++i) {
                    glm::vec4 color = read_accessor_vec4(
                        gltf_model,
                        accessor,
                        i,
                        glm::vec4(1.0f));
                    vertices[i].color = {
                        static_cast<uint8_t>(glm::clamp(color.r, 0.0f, 1.0f) * 255.0f),
                        static_cast<uint8_t>(glm::clamp(color.g, 0.0f, 1.0f) * 255.0f),
                        static_cast<uint8_t>(glm::clamp(color.b, 0.0f, 1.0f) * 255.0f),
                        static_cast<uint8_t>(glm::clamp(color.a, 0.0f, 1.0f) * 255.0f)
                    };
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

                    for (size_t i = 0; i < vertex_count && i < accessor.count; ++i) {
                        skel_vertices[i].normal = glm::normalize(
                            glm::vec3(read_accessor_vec4(gltf_model, accessor, i, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f))));
                    }
                }

                // Load bone weights and indices
                if (model_data->skeleton &&
                    primitive.attributes.find("JOINTS_0") != primitive.attributes.end() &&
                    primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end()) {

                    // Load joint indices
                    const tinygltf::Accessor& joint_accessor = gltf_model.accessors[primitive.attributes.at("JOINTS_0")];

                    // Load weights
                    const tinygltf::Accessor& weight_accessor = gltf_model.accessors[primitive.attributes.at("WEIGHTS_0")];

                    for (size_t i = 0; i < vertex_count; ++i) {
                        skel_vertices[i].bone_ids = read_accessor_ivec4(
                            gltf_model,
                            joint_accessor,
                            i,
                            glm::ivec4(-1));

                        glm::vec4 weights = read_accessor_vec4(gltf_model, weight_accessor, i);
                        const float weight_sum = weights.x + weights.y + weights.z + weights.w;
                        if (weight_sum > 0.0f) {
                            weights /= weight_sum;
                        }
                        skel_vertices[i].bone_weights = weights;
                    }
                }
            }

            // Load indices
            if (primitive.indices >= 0) {
                const tinygltf::Accessor& accessor = gltf_model.accessors[primitive.indices];

                indices.resize(accessor.count);
                for (size_t i = 0; i < accessor.count; ++i) {
                    indices[i] = read_accessor_index(gltf_model, accessor, i);
                }
            } else {
                indices.resize(vertex_count);
                for (size_t i = 0; i < vertex_count; ++i) {
                    indices[i] = static_cast<MeshIndex>(i);
                }
            }

            // Create mesh using the Mesh constructor
            SkinnedMesh mesh(vertices, indices, skel_vertices);
            const int primitiveIndex = static_cast<int>(model_data->meshes.size());
            model_data->meshes.push_back(std::move(mesh));
            model_data->meshMaterialIndices.push_back(primitive.material);
            model_data->meshPrimitiveIndices[gltfMeshIndex].push_back(primitiveIndex);
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

            // Get output values
            const tinygltf::Accessor& output_accessor = gltf_model.accessors[sampler.output];

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
                    const float time = read_accessor_vec4(gltf_model, time_accessor, i).x;
                    const glm::vec3 translation =
                        glm::vec3(read_accessor_vec4(gltf_model, output_accessor, i));

                    PositionKeyframe key;
                    key.time = time;
                    key.position = translation;
                    bone_anim->position_keys.push_back(key);
                    max_time = std::max(max_time, key.time);
                }
            }
            else if (channel.target_path == "rotation") {
                for (size_t i = 0; i < time_accessor.count; ++i) {
                    const float time = read_accessor_vec4(gltf_model, time_accessor, i).x;
                    const glm::vec4 rotation =
                        read_accessor_vec4(gltf_model, output_accessor, i, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

                    RotationKeyframe key;
                    key.time = time;
                    key.rotation = glm::quat(rotation.w, rotation.x, rotation.y, rotation.z);
                    bone_anim->rotation_keys.push_back(key);
                    max_time = std::max(max_time, key.time);
                }
            }
            else if (channel.target_path == "scale") {
                for (size_t i = 0; i < time_accessor.count; ++i) {
                    const float time = read_accessor_vec4(gltf_model, time_accessor, i).x;
                    const glm::vec3 scale =
                        glm::vec3(read_accessor_vec4(gltf_model, output_accessor, i, glm::vec4(1.0f)));

                    ScaleKeyframe key;
                    key.time = time;
                    key.scale = scale;
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
