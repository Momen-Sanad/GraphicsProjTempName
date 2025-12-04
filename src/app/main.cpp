#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "../engine/platform/Window.hpp"
#include "../engine/gl/GLContext.hpp"
#include "../engine/ecs/World.hpp"
#include "../engine/ecs/CameraController.hpp"
#include "../engine/ecs/ShaderManager.hpp"
#include "../engine/assets/MaterialManager.hpp"
#include "../engine/components/MeshRenderer.hpp"

// Window size
#define WINDOW_W 1280
#define WINDOW_H 720

int main() {

    // ---------------------------
    // Window + GL Context Setup
    // ---------------------------
    Window window(WINDOW_W, WINDOW_H, "Hello World");
    if (!window.get_handle()) return 1;

    if (!GLContext::init()) return 1;
    GLContext::enable_default_render_settings();

    // ---------------------------
    // Load Shader
    // ---------------------------
    ShaderManager shaderManager;
    auto mainShader = shaderManager.loadShader("main",
        "../shaders/main.vert",
        "../shaders/main.frag"
    );
    if (!mainShader) {
        fprintf(stderr, "Failed to load main shader\n");
        return 1;
    }

    // ---------------------------
    // Create materials
    // ---------------------------
    TintedMaterial blue, brown, green, yellow;

    blue.setShader(mainShader);
    blue.tint  = glm::vec4(0.2f, 0.4f, 1.0f, 1.0f);

    brown.setShader(mainShader);
    brown.tint = glm::vec4(0.5f, 0.2f, 0.1f, 1.0f);

    green.setShader(mainShader);
    green.tint = glm::vec4(0.4f, 1.0f, 0.2f, 1.0f);

    yellow.setShader(mainShader);
    yellow.tint = glm::vec4(1.0f, 1.0f, 0.3f, 1.0f);


    // ---------------------------
    // Mesh Setup (cube vertex/index data)
    // ---------------------------
    std::vector<Vertex> vertices = {
        // Front
        {{ 0.5f, 1.0f, 0.5f}, {255,255,255,255}},
        {{-0.5f, 1.0f, 0.5f}, {255,255,255,255}},
        {{-0.5f, 0.0f, 0.5f}, {0,0,0,255}},
        {{ 0.5f, 0.0f, 0.5f}, {0,0,0,255}},
        // Back
        {{-0.5f, 1.0f, -0.5f}, {255,255,255,255}},
        {{ 0.5f, 1.0f, -0.5f}, {255,255,255,255}},
        {{ 0.5f, 0.0f, -0.5f}, {0,0,0,255}},
        {{-0.5f, 0.0f, -0.5f}, {0,0,0,255}},
        // Top
        {{-0.5f, 1.0f, 0.5f}, {255,255,255,255}},
        {{ 0.5f, 1.0f, 0.5f}, {255,255,255,255}},
        {{ 0.5f, 1.0f, -0.5f}, {255,255,255,255}},
        {{-0.5f, 1.0f, -0.5f}, {255,255,255,255}},
        // Bottom
        {{ 0.5f, 0.0f, 0.5f}, {0,0,0,255}},
        {{-0.5f, 0.0f, 0.5f}, {0,0,0,255}},
        {{-0.5f, 0.0f, -0.5f}, {0,0,0,255}},
        {{ 0.5f, 0.0f, -0.5f}, {0,0,0,255}},
        // Right
        {{ 0.5f, 0.0f, -0.5f}, {0,0,0,255}},
        {{ 0.5f, 1.0f, -0.5f}, {255,255,255,255}},
        {{ 0.5f, 1.0f, 0.5f}, {255,255,255,255}},
        {{ 0.5f, 0.0f, 0.5f}, {0,0,0,255}},
        // Left
        {{-0.5f, 0.0f, 0.5f}, {0,0,0,255}},
        {{-0.5f, 1.0f, 0.5f}, {255,255,255,255}},
        {{-0.5f, 1.0f, -0.5f}, {255,255,255,255}},
        {{-0.5f, 0.0f, -0.5f}, {0,0,0,255}}
    };

    std::vector<uint16_t> indices = {
        0,1,2, 2,3,0,
        4,5,6, 6,7,4,
        8,9,10, 10,11,8,
        12,13,14, 14,15,12,
        16,17,18, 18,19,16,
        20,21,22, 22,23,20
    };
    
    /*
     * make an abstraction to satisfy the following:
     * give cube coordinates to spawn at (x, y, z)
     * give cube scale
     * give cube rotation
     * then the engine figures out the indices and vertices on its own
     * instead of physically writing them out
     * make the user able to create X amount of standard objects
     * like cylinder, cube, sphere, etc..
     * then the user can modify scale,rotation, coordinates
     * don't allow the user to create custom objects like before
     * 
     * */
    
    MeshData cubeData(vertices, indices);
    MeshRenderer cube;
    cube.upload(cubeData);

    // ---------------------------
    // World + Camera Setup
    // ---------------------------
    World world;

    // configure world camera directly (Camera has public members in your build)
    world.get_camera().position  = glm::vec3(10.f, 5.f, 10.f);
    world.get_camera().direction = glm::normalize(glm::vec3(-1.f, 0.f, -1.f));
    world.get_camera().up        = glm::vec3(0.f, 1.f, 0.f);
    world.get_camera().fov       = glm::radians(60.0f);
    world.get_camera().near      = 0.1f;
    world.get_camera().far       = 100.0f;

    // ---------------------------
    //       Scene graph
    // ---------------------------
    Entity* root = world.createEntityWithParams(nullptr);

    // Water plane
    Entity* water = world.createEntityWithParams(
        root,                    // parent
        {0.f, 0.f, 0.f},         // position
        glm::quat(),             // rotation
        {10.f, 1.f, 10.f},       // scale
        &cube,                   // mesh
        &blue                    // material
    );

    // Island (empty entity, parent of sand & tree)
    Entity* island = world.createEntityWithParams(root);

    // Sand on the island
    Entity* sand = world.createEntityWithParams(
        island,
        {0.f, 0.5f, 0.f},
        glm::quat(),
        {2.f, 1.f, 2.f},
        &cube,
        &yellow
    );

    // Tree base
    Entity* tree = world.createEntityWithParams(island);

    // Tree trunk
    Entity* tree_trunk = world.createEntityWithParams(
        tree,
        {0.f, 0.f, 0.f},
        glm::quat(),
        {0.5f, 5.f, 0.5f},
        &cube,
        &brown
    );

    // Tree leaves
    for (int i = 0; i < 4; i++) {
        glm::quat rotY = glm::angleAxis(1.5f * i + 0.7f, glm::vec3(0.f, 1.f, 0.f));
        glm::quat rotZ = glm::angleAxis(0.7f, glm::vec3(0.f, 0.f, 1.f));

        world.createEntityWithParams(
            tree,
            {0.f, 5.f, 0.f},
            rotY * rotZ,
            {0.5f, 2.f, 0.5f},
            &cube,
            &green
        );
    }

    // ---------------------------
    // Camera controller
    // ---------------------------
    CameraController controller;
    controller.setup(window.get_handle(), &world.get_camera());


    float last_time = 0.0f; // store previous frame time
    // printf("island has %zu children\n", island->getChildren().size());
    // for (Entity* c : island->getChildren()) {
    //     printf("child %p parent=%p\n", (void*)c, (void*)c->getParent());
    // }
    
    // ---------------------------
    // Main Loop
    // ---------------------------
    while (!glfwWindowShouldClose(window.get_handle())) {
        window.poll_events();

        float time = static_cast<float>(glfwGetTime());
        float delta_time = time - last_time;  // calculate frame delta
        last_time = time;
        

        // animate water scale (use setter) and rotate island using getters/setters:
        water->setScale({10.f, 1.0f + 0.1f * glm::sin(time), 10.f});
        float rot_speed = glm::radians(30.f); // 30 deg/sec
        water->setRotation(glm::angleAxis(rot_speed * delta_time, glm::vec3(0.f,1.f,0)) * water->getRotation());

        // for (Entity* leaf : treeLeaves) {
        //     leaf->setRotation(glm::angleAxis(time*1.5f, glm::vec3(0.f,1.f,0)) * leaf->getRotation());
        // }
        
        controller.update(delta_time);
        
        int w, h;
        window.get_framebuffer_size(w, h);
        
        glViewport(0, 0, w, h);
        glClearColor(0.90f, 0.95f, 1.0f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 VP = world.get_camera().get_view_projection_matrix(glm::vec2((float)w, (float)h));

        for (Entity* root : world.getEntityManager().getRoots()) {
            world.getEntityManager().renderEntityRecursive(root, VP);
        }

        window.swap_buffers();
    }

    // Cleanup
    cube.destroy();
    // mainShader.destroy();

    return 0;
}