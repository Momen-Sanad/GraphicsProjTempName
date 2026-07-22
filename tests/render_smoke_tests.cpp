#include "engine/assets/LitMaterial.hpp"
#include "engine/assets/SkinnedMaterial.hpp"
#include "engine/assets/TexturedMaterial.hpp"
#include "engine/assets/TintedMaterial.hpp"
#include "engine/components/Light.hpp"
#include "engine/components/SkinnedMeshRenderer.hpp"
#include "engine/ecs/World.hpp"
#include "engine/gl/GLContext.hpp"
#include "engine/gl/Mesh.hpp"
#include "engine/gl/Shader.hpp"
#include "engine/gl/SkinnedMesh.hpp"
#include "engine/gl/Texture.hpp"
#include "engine/systems/RenderSystem.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <array>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef SHADER_DIR
#define SHADER_DIR "../shaders"
#endif

namespace {
constexpr int kFramebufferSize = 64;

const char* kColorVertexShader = R"GLSL(
#version 330 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_uv;
layout(location = 3) in vec4 a_color;
uniform mat4 MVP;
out vec4 v_color;
void main() {
    v_color = a_color;
    gl_Position = MVP * vec4(a_position, 1.0);
}
)GLSL";

const char* kColorFragmentShader = R"GLSL(
#version 330 core
in vec4 v_color;
uniform vec4 tint;
out vec4 frag_color;
void main() {
    frag_color = v_color * tint;
}
)GLSL";

const char* kTextureVertexShader = R"GLSL(
#version 330 core
layout(location = 0) in vec3 a_position;
layout(location = 2) in vec2 a_uv;
uniform mat4 MVP;
out vec2 v_uv;
void main() {
    v_uv = a_uv;
    gl_Position = MVP * vec4(a_position, 1.0);
}
)GLSL";

const char* kTextureFragmentShader = R"GLSL(
#version 330 core
in vec2 v_uv;
uniform sampler2D u_texture;
out vec4 frag_color;
void main() {
    frag_color = texture(u_texture, v_uv);
}
)GLSL";

void require(bool condition, const char* message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

std::shared_ptr<Shader> makeShader(const char* vertex, const char* fragment)
{
    auto shader = std::make_shared<Shader>();
    require(shader->createFromSources(vertex, fragment), "Failed to create shader from sources");
    return shader;
}

Mesh makeTriangleMesh()
{
    std::vector<Vertex> vertices = {
        {glm::vec3(-0.8f, -0.8f, 0.0f), WHITE, glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
        {glm::vec3( 0.8f, -0.8f, 0.0f), WHITE, glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
        {glm::vec3( 0.0f,  0.8f, 0.0f), WHITE, glm::vec2(0.5f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)}
    };
    std::vector<MeshIndex> indices = {0, 1, 2};
    return Mesh(vertices, indices);
}

SkinnedMesh makeSkinnedTriangleMesh()
{
    std::vector<Vertex> vertices = {
        {glm::vec3(-0.8f, -0.8f, 0.0f), WHITE, glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
        {glm::vec3( 0.8f, -0.8f, 0.0f), WHITE, glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
        {glm::vec3( 0.0f,  0.8f, 0.0f), WHITE, glm::vec2(0.5f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)}
    };
    std::vector<MeshIndex> indices = {0, 1, 2};
    std::vector<SkeletonVertex> skeletonVertices(3);
    for (SkeletonVertex& vertex : skeletonVertices) {
        vertex.normal = glm::vec3(0.0f, 0.0f, 1.0f);
        vertex.bone_ids = glm::ivec4(0, -1, -1, -1);
        vertex.bone_weights = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    }
    return SkinnedMesh(vertices, indices, skeletonVertices);
}

RenderContext makeContext()
{
    RenderContext context;
    context.view = glm::mat4(1.0f);
    context.projection = glm::mat4(1.0f);
    context.viewProj = glm::mat4(1.0f);
    context.cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
    return context;
}

std::array<unsigned char, 4> drawAndReadCenter(World& world)
{
    while (glGetError() != GL_NO_ERROR) {
    }

    glViewport(0, 0, kFramebufferSize, kFramebufferSize);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    world.renderer().render(world, makeContext());
    glFinish();

    std::array<unsigned char, 4> pixel{};
    glReadPixels(
        kFramebufferSize / 2,
        kFramebufferSize / 2,
        1,
        1,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixel.data());

    require(glGetError() == GL_NO_ERROR, "OpenGL render smoke produced an error");
    return pixel;
}

int brightness(const std::array<unsigned char, 4>& pixel)
{
    return static_cast<int>(pixel[0]) + static_cast<int>(pixel[1]) + static_cast<int>(pixel[2]);
}

void testStaticTintedDraw()
{
    World world;
    auto shader = makeShader(kColorVertexShader, kColorFragmentShader);
    Mesh mesh = makeTriangleMesh();
    auto renderer = world.assets().createMeshRenderer("smoke-static", mesh);
    auto material = std::make_shared<TintedMaterial>();
    material->setShader(shader);
    material->tint = glm::vec4(0.25f, 0.5f, 1.0f, 1.0f);
    world.createRenderable("static", renderer, material);

    const auto pixel = drawAndReadCenter(world);
    world.shutdownGpuResources();

    require(pixel[2] > pixel[1] && pixel[1] > pixel[0], "Tinted static draw should preserve tint color ordering");
    require(brightness(pixel) > 80, "Tinted static draw should produce visible pixels");
}

void testTexturedDraw()
{
    World world;
    auto shader = makeShader(kTextureVertexShader, kTextureFragmentShader);
    auto texture = std::make_shared<Texture>();
    const unsigned char texel[4] = {240, 32, 16, 255};
    texture->create(1, 1, texel, GL_RGBA);

    Mesh mesh = makeTriangleMesh();
    auto renderer = world.assets().createMeshRenderer("smoke-textured", mesh);
    auto material = std::make_shared<TexturedMaterial>(shader, texture);
    material->addTextureLayer(nullptr);
    world.createRenderable("textured", renderer, material);

    const auto pixel = drawAndReadCenter(world);
    world.shutdownGpuResources();

    require(pixel[0] > 180 && pixel[1] < 80 && pixel[2] < 80, "Textured draw should sample the bound texture");
    require(material->getTextureLayerCount() == 1, "Null texture layers should be ignored");
}

std::array<unsigned char, 4> drawLit(float intensity)
{
    World world;
    const std::string shaderDir = SHADER_DIR;
    auto shader = world.assets().loadShader(
        "smoke-lit-" + std::to_string(static_cast<int>(intensity * 10.0f)),
        shaderDir + "/light.vert",
        shaderDir + "/light.frag");
    require(shader != nullptr, "Failed to load lit shader");

    world.lights().initUBO();
    require(world.lights().bindToShader(shader), "Lit shader should expose LightBlock");
    world.lights().addLight(Light(
        LightType::DIRECTIONAL,
        glm::vec3(1.0f),
        glm::vec3(0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::radians(15.0f),
        glm::radians(30.0f),
        intensity));

    Mesh mesh = makeTriangleMesh();
    auto renderer = world.assets().createMeshRenderer("smoke-lit", mesh);
    auto material = std::make_shared<LitMaterial>(shader);
    world.createRenderable("lit", renderer, material);

    auto pixel = drawAndReadCenter(world);
    world.shutdownGpuResources();
    return pixel;
}

void testLightBlockIntensityChangesPixels()
{
    const auto darkPixel = drawLit(0.0f);
    const auto brightPixel = drawLit(3.0f);

    require(brightness(darkPixel) > 0, "Missing-map lit fallback should still render ambient color");
    require(
        brightness(brightPixel) > brightness(darkPixel) + 120,
        "Increasing directional light intensity should visibly brighten the lit draw");
}

std::array<unsigned char, 4> drawPointLit(const glm::vec3& color)
{
    World world;
    const std::string shaderDir = SHADER_DIR;
    auto shader = world.assets().loadShader(
        "smoke-point-lit-" + std::to_string(static_cast<int>(color.r * 10.0f)) + "-" +
            std::to_string(static_cast<int>(color.b * 10.0f)),
        shaderDir + "/light.vert",
        shaderDir + "/light.frag");
    require(shader != nullptr, "Failed to load point-lit shader");

    world.lights().initUBO();
    require(world.lights().bindToShader(shader), "Point-lit shader should expose LightBlock");
    world.lights().addLight(Light(
        LightType::POINT,
        color,
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f),
        glm::radians(15.0f),
        glm::radians(30.0f),
        3.0f));

    Mesh mesh = makeTriangleMesh();
    auto renderer = world.assets().createMeshRenderer("smoke-point-lit", mesh);
    auto material = std::make_shared<LitMaterial>(shader);
    world.createRenderable("point-lit", renderer, material);

    auto pixel = drawAndReadCenter(world);
    world.shutdownGpuResources();
    return pixel;
}

void testPointLightColorAffectsPixels()
{
    const auto redPixel = drawPointLit(glm::vec3(1.0f, 0.05f, 0.04f));
    const auto bluePixel = drawPointLit(glm::vec3(0.08f, 0.25f, 1.0f));

    require(redPixel[0] > redPixel[2] + 40, "Red point light should tint the lit pixel red");
    require(bluePixel[2] > bluePixel[0] + 40, "Blue point light should tint the lit pixel blue");
}

void testSkinnedDraw()
{
    World world;
    const std::string shaderDir = SHADER_DIR;
    auto shader = world.assets().loadShader(
        "smoke-skinned",
        shaderDir + "/skinned.vert",
        shaderDir + "/skinned.frag");
    require(shader != nullptr, "Failed to load skinned shader");

    world.lights().initUBO();
    require(world.lights().bindToShader(shader), "Skinned shader should expose LightBlock");
    world.lights().addLight(Light(
        LightType::DIRECTIONAL,
        glm::vec3(1.0f),
        glm::vec3(0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::radians(15.0f),
        glm::radians(30.0f),
        2.0f));

    SkinnedMesh mesh = makeSkinnedTriangleMesh();
    auto renderer = std::make_shared<SkinnedMeshRenderer>();
    renderer->upload(mesh);

    auto material = std::make_shared<SkinnedMaterial>(shader, nullptr);
    material->set_bone_matrices({glm::mat4(1.0f)});
    material->set_animated(true);
    world.createSkinnedRenderable("skinned", {renderer}, material, nullptr);

    const auto pixel = drawAndReadCenter(world);
    world.shutdownGpuResources();

    require(brightness(pixel) > 80, "Skinned draw should produce visible lit pixels");
    require(material->get_active_bone_count() == 1, "Skinned material should keep uploaded bone matrices");
}

bool runTest(const char* name, void (*test)())
{
    try {
        test();
        std::cout << "[PASS] " << name << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[FAIL] " << name << " - " << e.what() << std::endl;
        return false;
    }
}
} // namespace

int main()
{
    GLFWwindow* window = nullptr;
    try {
        require(glfwInit() == GLFW_TRUE, "Failed to initialize GLFW");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        window = glfwCreateWindow(kFramebufferSize, kFramebufferSize, "render_smoke_tests", nullptr, nullptr);
        require(window != nullptr, "Failed to create hidden GLFW window");
        glfwMakeContextCurrent(window);

        require(GLContext::init(), "Failed to initialize GLAD");
        GLContext::enable_default_render_settings();

        int passed = 0;
        int failed = 0;
        auto run = [&passed, &failed](const char* name, void (*test)()) {
            if (runTest(name, test)) {
                passed++;
            } else {
                failed++;
            }
        };

        run("static_tinted_draw", testStaticTintedDraw);
        run("textured_draw", testTexturedDraw);
        run("light_block_intensity_changes_pixels", testLightBlockIntensityChangesPixels);
        run("point_light_color_affects_pixels", testPointLightColorAffectsPixels);
        run("skinned_draw", testSkinnedDraw);

        if (window) {
            glfwDestroyWindow(window);
        }
        glfwTerminate();

        std::cout << "Tests passed: " << passed << std::endl;
        std::cout << "Tests failed: " << failed << std::endl;
        return failed == 0 ? 0 : 1;
    } catch (const std::exception& e) {
        std::cerr << "[FAIL] render_smoke_tests - " << e.what() << std::endl;
        if (window) {
            glfwDestroyWindow(window);
        }
        glfwTerminate();
        return 1;
    }
}
