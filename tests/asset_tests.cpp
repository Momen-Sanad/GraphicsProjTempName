#include "engine/assets/AssetManager.hpp"
#include "engine/gl/Mesh.hpp"

#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
int tests_passed = 0;
int tests_failed = 0;

void assert_true(bool condition, const char* message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void assert_close(float actual, float expected, const char* message)
{
    if (std::abs(actual - expected) > 0.0001f) {
        throw std::runtime_error(message);
    }
}

void run_test(const char* name, void (*test)())
{
    try {
        test();
        std::cout << "[PASS] " << name << std::endl;
        tests_passed++;
    } catch (const std::exception& e) {
        std::cerr << "[FAIL] " << name << " - " << e.what() << std::endl;
        tests_failed++;
    }
}

std::filesystem::path test_output_dir()
{
    std::filesystem::path dir = std::filesystem::current_path() / "asset_test_files";
    std::filesystem::create_directories(dir);
    return dir;
}

void write_text(const std::filesystem::path& path, const std::string& text)
{
    std::ofstream file(path);
    if (!file) {
        throw std::runtime_error("Failed to write " + path.string());
    }
    file << text;
}

void test_obj_without_colors_or_uvs()
{
    const std::filesystem::path obj_path = test_output_dir() / "no_colors_no_uvs.obj";
    write_text(
        obj_path,
        "v 0 0 0\n"
        "v 1 0 0\n"
        "v 0 1 0\n"
        "f 1 2 3\n");

    AssetManager assets;
    auto mesh = assets.loadMesh(obj_path.string());
    assert_true(mesh != nullptr, "OBJ should load without color or UV attributes");
    assert_true(mesh->get_vertex_count() == 3, "OBJ should have 3 unique vertices");
    assert_true(mesh->get_index_count() == 3, "OBJ should have 3 indices");

    for (const Vertex& vertex : mesh->get_vertices()) {
        assert_true(
            vertex.color.r == 255 && vertex.color.g == 255 && vertex.color.b == 255 && vertex.color.a == 255,
            "Missing OBJ colors should default to white");
    }
}

void test_mesh_keeps_32_bit_indices()
{
    std::vector<Vertex> vertices(70000);
    for (size_t i = 0; i < vertices.size(); ++i) {
        vertices[i].position = glm::vec3(static_cast<float>(i), 0.0f, 0.0f);
        vertices[i].color = WHITE;
    }

    std::vector<MeshIndex> indices = {0, 65535, 69999};
    Mesh mesh(vertices, indices);

    assert_true(mesh.get_indices()[2] == 69999, "Mesh index should not be truncated to 16 bits");
}

void write_binary_gltf_payload(const std::filesystem::path& path)
{
    std::ofstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to write " + path.string());
    }

    struct InterleavedVertex {
        float x;
        float y;
        float z;
        float pad;
    };

    const InterleavedVertex vertices[] = {
        {0.0f, 0.0f, 0.0f, 99.0f},
        {1.0f, 0.0f, 0.0f, 99.0f},
        {0.0f, 1.0f, 0.0f, 99.0f},
    };
    const uint32_t indices[] = {0, 1, 2};

    file.write(reinterpret_cast<const char*>(vertices), sizeof(vertices));
    file.write(reinterpret_cast<const char*>(indices), sizeof(indices));
}

void write_multi_primitive_payload(const std::filesystem::path& path)
{
    std::ofstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to write " + path.string());
    }

    const float vertices[] = {
        -0.5f, 0.0f, 0.0f,
         0.5f, 0.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
        -0.5f, 0.0f, 1.0f,
         0.5f, 0.0f, 1.0f,
         0.0f, 1.0f, 1.0f,
    };
    file.write(reinterpret_cast<const char*>(vertices), sizeof(vertices));
}

void test_gltf_strided_uint32_indices()
{
    const std::filesystem::path dir = test_output_dir();
    const std::filesystem::path bin_path = dir / "strided.bin";
    const std::filesystem::path gltf_path = dir / "strided.gltf";

    write_binary_gltf_payload(bin_path);
    write_text(
        gltf_path,
        "{\n"
        "  \"asset\": { \"version\": \"2.0\" },\n"
        "  \"scene\": 0,\n"
        "  \"scenes\": [{ \"nodes\": [0] }],\n"
        "  \"nodes\": [{ \"mesh\": 0 }],\n"
        "  \"meshes\": [{ \"primitives\": [{ \"attributes\": { \"POSITION\": 0 }, \"indices\": 1 }] }],\n"
        "  \"buffers\": [{ \"uri\": \"strided.bin\", \"byteLength\": 60 }],\n"
        "  \"bufferViews\": [\n"
        "    { \"buffer\": 0, \"byteOffset\": 0, \"byteLength\": 48, \"byteStride\": 16 },\n"
        "    { \"buffer\": 0, \"byteOffset\": 48, \"byteLength\": 12 }\n"
        "  ],\n"
        "  \"accessors\": [\n"
        "    { \"bufferView\": 0, \"componentType\": 5126, \"count\": 3, \"type\": \"VEC3\" },\n"
        "    { \"bufferView\": 1, \"componentType\": 5125, \"count\": 3, \"type\": \"SCALAR\" }\n"
        "  ]\n"
        "}\n");

    AssetManager assets;
    auto model = assets.loadModel(gltf_path.string());
    assert_true(model != nullptr, "Strided glTF should load");
    assert_true(model->primitives.size() == 1, "glTF should contain one mesh primitive");
    assert_true(model->primitives[0].mesh != nullptr, "glTF primitive should expose mesh data");
    assert_true(model->primitives[0].mesh->get_vertex_count() == 3, "glTF should contain 3 vertices");
    assert_true(model->primitives[0].mesh->get_indices()[2] == 2, "glTF uint32 indices should load correctly");
    assert_true(
        model->primitives[0].mesh->get_vertices()[1].position.x == 1.0f,
        "glTF byteStride should skip interleaved padding");
    assert_true(model->defaultScene == 0, "AssetManager should expose a default scene");
    assert_true(model->bounds.valid, "AssetManager should expose imported bounds");
}

void test_gltf_nodes_multiple_primitives_and_materials()
{
    const std::filesystem::path dir = test_output_dir();
    const std::filesystem::path bin_path = dir / "multi_primitive.bin";
    const std::filesystem::path gltf_path = dir / "multi_primitive.gltf";

    write_multi_primitive_payload(bin_path);
    write_text(
        gltf_path,
        "{\n"
        "  \"asset\": { \"version\": \"2.0\" },\n"
        "  \"scene\": 0,\n"
        "  \"scenes\": [{ \"name\": \"Scene\", \"nodes\": [0] }],\n"
        "  \"nodes\": [\n"
        "    { \"name\": \"Root\", \"translation\": [1, 2, 3], \"children\": [1] },\n"
        "    { \"name\": \"MeshNode\", \"mesh\": 0, \"scale\": [2, 2, 2] }\n"
        "  ],\n"
        "  \"materials\": [{ \"name\": \"Red\" }, { \"name\": \"Blue\" }],\n"
        "  \"meshes\": [{ \"primitives\": [\n"
        "    { \"attributes\": { \"POSITION\": 0 }, \"material\": 0 },\n"
        "    { \"attributes\": { \"POSITION\": 1 }, \"material\": 1 }\n"
        "  ] }],\n"
        "  \"buffers\": [{ \"uri\": \"multi_primitive.bin\", \"byteLength\": 72 }],\n"
        "  \"bufferViews\": [{ \"buffer\": 0, \"byteOffset\": 0, \"byteLength\": 72 }],\n"
        "  \"accessors\": [\n"
        "    { \"bufferView\": 0, \"byteOffset\": 0, \"componentType\": 5126, \"count\": 3, \"type\": \"VEC3\" },\n"
        "    { \"bufferView\": 0, \"byteOffset\": 36, \"componentType\": 5126, \"count\": 3, \"type\": \"VEC3\" }\n"
        "  ]\n"
        "}\n");

    AssetManager assets;
    auto model = assets.loadModel(gltf_path.string());
    assert_true(model != nullptr, "Multi-primitive glTF should load");
    assert_true(model->primitives.size() == 2, "glTF mesh should expose both primitives");
    assert_true(model->materials.size() == 2, "glTF material slots should be preserved");
    assert_true(model->materials[0].name == "Red", "First material name should be preserved");
    assert_true(model->primitives[0].materialIndex == 0, "First primitive should keep material index");
    assert_true(model->primitives[1].materialIndex == 1, "Second primitive should keep material index");
    assert_true(model->nodes.size() == 2, "glTF nodes should be preserved");
    assert_true(model->nodes[0].children.size() == 1 && model->nodes[0].children[0] == 1, "Node hierarchy should be preserved");
    assert_true(model->nodes[1].parentIndex == 0, "Child node should record parent index");
    assert_true(model->nodes[1].primitiveIndices.size() == 2, "Mesh node should reference flattened primitives");
    assert_close(model->nodes[0].localTransform[3].x, 1.0f, "Node translation X should be imported");
    assert_close(model->nodes[0].localTransform[3].y, 2.0f, "Node translation Y should be imported");
    assert_close(model->nodes[1].localTransform[0].x, 2.0f, "Node scale should be imported");
    assert_true(model->defaultScene == 0, "Default scene should be preserved");
    assert_true(model->scenes.size() == 1 && model->scenes[0].rootNodes[0] == 0, "Scene roots should be preserved");
}

void test_gltf_required_extension_rejected()
{
    const std::filesystem::path gltf_path = test_output_dir() / "required_extension.gltf";
    write_text(
        gltf_path,
        "{\n"
        "  \"asset\": { \"version\": \"2.0\" },\n"
        "  \"extensionsRequired\": [\"KHR_draco_mesh_compression\"],\n"
        "  \"scenes\": [{ \"nodes\": [] }],\n"
        "  \"scene\": 0\n"
        "}\n");

    AssetManager assets;
    auto model = assets.loadModel(gltf_path.string());
    assert_true(model == nullptr, "Unsupported required glTF extension should be rejected");
}
} // namespace

int main()
{
    run_test("obj_without_colors_or_uvs", test_obj_without_colors_or_uvs);
    run_test("mesh_keeps_32_bit_indices", test_mesh_keeps_32_bit_indices);
    run_test("gltf_strided_uint32_indices", test_gltf_strided_uint32_indices);
    run_test("gltf_nodes_multiple_primitives_and_materials", test_gltf_nodes_multiple_primitives_and_materials);
    run_test("gltf_required_extension_rejected", test_gltf_required_extension_rejected);

    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    return tests_failed == 0 ? 0 : 1;
}
