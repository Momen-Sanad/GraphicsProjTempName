#include "engine/assets/TintedMaterial.hpp"
#include "engine/ecs/World.hpp"
#include "engine/gl/GLContext.hpp"
#include "engine/gl/Mesh.hpp"
#include "engine/gl/Shader.hpp"
#include "engine/systems/RenderSystem.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

namespace {
const char* kVertexShader = R"GLSL(
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

const char* kFragmentShader = R"GLSL(
#version 330 core
in vec4 v_color;
uniform vec4 tint;
out vec4 frag_color;
void main() {
    frag_color = v_color * tint;
}
)GLSL";

void require(bool condition, const char* message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}
} // namespace

int main()
{
    try {
        require(glfwInit() == GLFW_TRUE, "Failed to initialize GLFW");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        GLFWwindow* window = glfwCreateWindow(64, 64, "render_smoke_tests", nullptr, nullptr);
        require(window != nullptr, "Failed to create hidden GLFW window");
        glfwMakeContextCurrent(window);

        require(GLContext::init(), "Failed to initialize GLAD");
        GLContext::enable_default_render_settings();

        auto shader = std::make_shared<Shader>();
        require(shader->createFromSources(kVertexShader, kFragmentShader), "Failed to create smoke shader");

        std::vector<Vertex> vertices = {
            {glm::vec3(-0.5f, -0.5f, 0.0f), WHITE, glm::vec2(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
            {glm::vec3(0.5f, -0.5f, 0.0f), WHITE, glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
            {glm::vec3(0.0f, 0.5f, 0.0f), WHITE, glm::vec2(0.5f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)}
        };
        std::vector<MeshIndex> indices = {0, 1, 2};

        World world;
        Mesh mesh(vertices, indices);
        auto renderer = world.assets().createMeshRenderer("smoke-triangle", mesh);
        auto material = std::make_shared<TintedMaterial>();
        material->setShader(shader);
        material->tint = glm::vec4(1.0f);
        world.assets().registerMaterial("smoke-tint", material);
        world.createRenderable("smoke", renderer, material);

        RenderContext context;
        context.viewProj = glm::mat4(1.0f);
        context.view = glm::mat4(1.0f);
        context.projection = glm::mat4(1.0f);

        glViewport(0, 0, 64, 64);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        world.renderer().render(world, context);
        glFinish();

        GLenum err = glGetError();
        world.clear();
        world.assets().clear();
        glfwDestroyWindow(window);
        glfwTerminate();

        require(err == GL_NO_ERROR, "OpenGL smoke draw produced an error");
        std::cout << "[PASS] render_smoke_tests" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "[FAIL] render_smoke_tests - " << e.what() << std::endl;
        glfwTerminate();
        return 1;
    }
}
