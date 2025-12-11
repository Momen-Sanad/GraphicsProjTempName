//gl stuff
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <string>

//imgui stuff
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

//custom stuff
#include "../engine/platform/Window.hpp"
#include "../engine/gl/GLContext.hpp"
#include "../engine/ecs/World.hpp"
#include "../engine/ecs/CameraController.hpp"
#include "../engine/ecs/ShaderManager.hpp"
#include "../engine/assets/MaterialManager.hpp"
#include "../engine/components/MeshRenderer.hpp"
#include "../engine/gl/Mesh.hpp"
#include "../engine/utils/Im_GUI_Inspector.hpp"
#include "../engine/assets/MeshLoader.hpp"
#include "../engine/assets/TexturedMaterial.hpp"


#include "../engine/assets/TextureLoader.hpp"
// Window size
#define WINDOW_W 1280
#define WINDOW_H 720
#ifdef ASSET_DIR
#define MODELS_DIR ASSET_DIR "/models"
#define TEXTURES_DIR ASSET_DIR "/textures"
#else
#define MODELS_DIR "../../assets/models"
#define TEXTURES_DIR "../../assets/textures"
#endif
#ifndef SHADER_DIR
#define SHADER_DIR "../../shaders"
#endif

/* 
 * #TODO
 * do something about all this gl stuff (also apply anistrpoic filtering 16x ) DONE
 * probably in GLContext.cpp /.hpp
 * do the frame scheduler
 * need pipelineState.hpp probably for the frame scheduler and coordinator
 * turn the superloop into unity's Update()
 * optional: logger
 * rana should do react3dphysics    DONE
 * soliman should do textures and obj loading DONE
 * need to do scenes and scene manager DONE
 * decide what to do with these files:
 * system manager, system.hpp, component, asset manager,
 * renderSystem, TransformSystem <- we already have "TransformComponent"
 * 
 */

int main() {

    // ---------------------------
    // Window + GL Context Setup
    // ---------------------------
    Window window(WINDOW_W, WINDOW_H, "Hello World");
    if (!window.get_handle()) return 1;

    if (!GLContext::init()) return 1;
    GLContext::enable_default_render_settings();

    // ---------------------------
    // ImGui Initialization
    // ---------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // Init backends
    ImGui_ImplGlfw_InitForOpenGL(window.get_handle(), true);
    ImGui_ImplOpenGL3_Init("#version 330");  // matches GL context

    // ---------------------------
    // Load Shader
    // ---------------------------
    ShaderManager shaderManager;

    auto mainShader = shaderManager.loadShader("main",
        std::string(SHADER_DIR) + "/blackToWhite.vert",
        std::string(SHADER_DIR) + "/blackToWhite.frag"
    );
    auto houseShader = shaderManager.loadShader("textured",
        std::string(SHADER_DIR) + "/textured.vert",
        std::string(SHADER_DIR) + "/textured.frag"
    );

    auto houseMixedShader = shaderManager.loadShader("blended",
        std::string(SHADER_DIR) + "/blended.vert",
        std::string(SHADER_DIR) + "/blended.frag"
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

    yellow.setShader(mainShader);
    yellow.tint = glm::vec4(1.0f, 1.0f, 0.3f, 1.0f);

    green.setShader(mainShader);
    green.tint = glm::vec4(0.4f, 1.0f, 0.2f, 1.0f);

    // ---------------------------
    // Create Textures
    // ---------------------------

    // Texture Loading
    std::string MoonTexturePath = std::string(TEXTURES_DIR) + "/moon.jpg";
    auto MoonTexture = TextureLoader::load(MoonTexturePath);
    std::cout << "Attempting to load texture: " << MoonTexturePath << std::endl;

    std::string HouseText = std::string(TEXTURES_DIR) + "/house/house.jpeg";
    Texture* HouseTexture = TextureLoader::load(HouseText);
    std::cout << "Attempting to load texture: " << HouseText << std::endl;

    std::string GlassText = std::string(TEXTURES_DIR) + "/house/glass.png";
    Texture* GlassTexture = TextureLoader::load(GlassText);
    std::cout << "Attempting to load texture: " << GlassText << std::endl;

    std::string SkyText = std::string(TEXTURES_DIR) + "/sky.jpg";
    Texture* SkyTexture = TextureLoader::load(SkyText);
    if (!SkyTexture) {
        std::cerr << "Warning: failed to load sky texture: " << SkyText << " (sky will be blank)\n";
    }

    // Material for house
    TexturedMaterial houseMaterial(houseShader, HouseTexture);
	TexturedMaterial houseMixedMaterial(houseMixedShader, HouseTexture);
    houseMixedMaterial.addTextureLayer(MoonTexture, BlendMode::Lerp, 0.4f);
    
    TexturedMaterial skyMaterial(houseShader, SkyTexture);
    skyMaterial.setShader(houseShader);

    //house glass material
    TexturedMaterial GlassMaterial(houseMixedShader, GlassTexture);



    // create a sampler for sky texturing (for now till it's actually made in the file)
    GLuint skySampler = 0;
    glGenSamplers(1, &skySampler);
    glSamplerParameteri(skySampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(skySampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(skySampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(skySampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    
    //loading mesh from obj
    std::string meshPath = std::string(MODELS_DIR) + "/house/house.obj";
    std::cout << "Attempting to load mesh: " << meshPath << std::endl;
    Mesh* loadedMesh = MeshLoader::load(meshPath.c_str());

    // creating primitive mesh
    Mesh cubeMesh = Mesh::create_cuboid(glm::vec3(0.0f), glm::vec3(1.0f));  // Centered cube with size 1.0f

    Mesh glass_mesh = Mesh::create_plane(glm::vec3(0.0f), glm::vec3(1.0f));

    Mesh skySphere = Mesh::create_sphere();

    // Create a MeshRenderer to upload and render the cube
    MeshRenderer cube, house, glass, skyRenderer;              
    
    // Upload the mesh data to the GPU
    cube.upload(cubeMesh);
	house.upload(*loadedMesh);
	glass.upload(glass_mesh);
    skyRenderer.upload(skySphere);

    // ---------------------------
    // World + Camera Setup
    // ---------------------------
    World world;

    world.get_camera().position  = glm::vec3(10.f, 5.f, 10.f);
    world.get_camera().direction = glm::normalize(glm::vec3(-1.f, 0.f, -1.f));
    world.get_camera().up        = glm::vec3(0.f, 1.f, 0.f);
    world.get_camera().fov       = glm::radians(60.0f);
    world.get_camera().near      = 0.1f;
    world.get_camera().far       = 100.0f;

    // ---------------------------
    // Scene graph
    // ---------------------------
    Entity* root = world.createEntityWithParams(nullptr);

    // Water plane
    Entity* water = world.createEntityWithParams(
        root, {0.f, 0.f, 0.f}, glm::quat(), {10.f, 1.f, 10.f}, &cube, &blue
    );

    // Island (empty entity, parent of sand & tree)
    Entity* island = world.createEntityWithParams(root);

    {
        //debugging only
        glm::quat island_rotation = island->getRotation();
        std::cout << "Island rotation: " << island_rotation.x << ", " << island_rotation.y << ", " << island_rotation.z << ", " << island_rotation.w << std::endl;

    }

    // Sand on the island
    Entity* sand = world.createEntityWithParams(island, {0.f, 0.5f, 0.f}, glm::quat(), {2.f, 1.f, 2.f}, &cube, &yellow);
    Entity* testhouse = world.createEntityWithParams(root, {10.f, 1.f, 1.f}, glm::quat(), {1.f, 1.f, 1.f}, &house, &houseMixedMaterial);
    
    {
        // Window 1
        Entity* window1 = world.createEntityWithParams(
            testhouse,
            // local position relative to testhouse
            { 2.0f, 1.5f, 0.0f },
            // rotation using angleAxis
            glm::angleAxis(glm::half_pi<float>() * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f)), 
            // scale
            { 2.0f, 1.0f, 1.0f },
            &glass,
            &GlassMaterial
        );
    }

    {
        // Window 2
        Entity* window2 = world.createEntityWithParams(
            testhouse,
            { -0.2f, 2.0f, -2.75f }, // local
            glm::angleAxis(glm::half_pi<float>() * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)),
            { 1.0f, 1.0f, 1.5f },
            &glass,
            &GlassMaterial
        );
    }

    {
        // Window 3
        Entity* window3 = world.createEntityWithParams(
            testhouse,
            { -3.55f, 2.0f, 0.55f }, // local
            glm::angleAxis(-glm::half_pi<float>() * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f)),
            { 1.5f, 1.0f, 1.0f },
            &glass,
            &GlassMaterial
        );
    }


    // Tree base
    Entity* tree = world.createEntityWithParams(island);

    // Tree trunk
    Entity* tree_trunk = world.createEntityWithParams(tree, {0.f, 2.5f, 0.f}, glm::quat(), {0.5f, 5.f, 0.5f}, &cube, &brown);

    // Tree leaves

    glm::vec3 leafOffsets[4] = {
        { 0.f, 0.f, 0.5f },   // leaf 0
        { 0.f, 0.f, 0.75f },  // leaf 1
        { 0.f, 0.f, 2.5f },   // leaf 2
        { 0.f, 0.f, -2.5f }   // leaf 3
    };

    for (int i = 0; i < 4; i++) {
        glm::quat rotY = glm::angleAxis(1.5f * i + 0.7f, glm::vec3(0.f, 1.f, 0.f));
        glm::quat rotZ = glm::angleAxis(0.7f, glm::vec3(0.f, 0.f, 1.f));

        world.createEntityWithParams(
            tree,
            {leafOffsets[i].x, 6.0f + leafOffsets[i].y, leafOffsets[i].z},           
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


    // FPS Tracking
    float last_time = static_cast<float>(glfwGetTime());
    float last_fps_time = last_time;
    int frameCount = 0;

    // ---------------------------
    // Main Loop
    // ---------------------------
    while (!glfwWindowShouldClose(window.get_handle())) {
        window.poll_events();

        float time = static_cast<float>(glfwGetTime());
        float delta_time = time - last_time;
        last_time = time;                 // update immediately after delta is computed
        frameCount++;


        glm::vec3 island_position = island->getPosition();
        glm::quat island_rotation = island->getRotation();

        // Check if 1 second has passed (based on previous `last_time`)
        if (time - last_fps_time >= 1.0f) {
            // Print island's position and rotation every frame
            std::cout << "Island position: " << island_position.x << ", " << island_position.y << ", " << island_position.z << std::endl;
            std::cout << "Island rotation: " << island_rotation.x << ", " << island_rotation.y << ", " << island_rotation.z << ", " << island_rotation.w << std::endl;

            // Print world matrix every frame in a more readable format
            glm::mat4 M = island->getWorldMatrix();
            std::cout << "Island world matrix: \n";
            for (int i = 0; i < 4; i++) {
                std::cout << "| " << M[i][0] << " " << M[i][1] << " " << M[i][2] << " " << M[i][3] << " |\n";
            }

            // Print FPS
            printf("FPS: %d\n", frameCount);
            frameCount = 0;  // Reset frame count after FPS print
            // Reset last_time after printing FPS
            last_fps_time = time;
        }

        // Animate water scale and rotate island
        water->setScale({10.f, 1.0f + 0.1f * glm::sin(time), 10.f});
        float rot_speed = glm::radians(30.f); // 30 deg/sec

        glm::quat delta_rot = glm::angleAxis(rot_speed * delta_time, glm::vec3(0.f,1.f,0));
        island->setRotation(delta_rot * island_rotation);


        // Update controller and rendering
        controller.update(delta_time);

        // Rendering setup
        int w, h;
        window.get_framebuffer_size(w, h);
        glViewport(0, 0, w, h);
        glClearColor(0.90f, 0.95f, 1.0f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int width, height;
        glfwGetFramebufferSize(window.get_handle(), &width, &height);

        glm::mat4 VP = world.get_camera().get_view_projection_matrix(
            glm::vec2(width, height)
        );
        

        // ---------------------------
        // Draw sky (before other objects)
        // ---------------------------
        if (skySphere.get_vertex_count() > 0 && SkyTexture) {

            // Prepare transform: follow camera and scale big enough
            glm::mat4 M = glm::translate(glm::mat4(1.0f), world.get_camera().position)
                        * glm::scale(glm::mat4(1.0f), glm::vec3(500.0f)); // adjust scale if needed

            glm::mat4 MVP = VP * M;
            // z/w trick: push sky to far plane
            MVP[0][2] = MVP[0][3];
            MVP[1][2] = MVP[1][3];
            MVP[2][2] = MVP[2][3];
            MVP[3][2] = MVP[3][3];

            // Use the shader program first (must be done before any glUniform calls)
            glUseProgram(houseShader->getProgram());

            // Let the material prepare itself (binds texture/sampler/uniforms if implemented)
            skyMaterial.setShader(houseShader);
            skyMaterial.setup();

            // If your TexturedMaterial doesn't set the sampler uniform name "u_texture",
            // set it explicitly here (shader expects "u_texture")
            glActiveTexture(GL_TEXTURE0);
            
            // Tell the shader that the sampler 'u_texture' is on texture unit 0
            glUniform1i(houseShader->getUniformLocation("u_texture"), 0);

            // Upload MVP (now that program is active)
            glUniformMatrix4fv(houseShader->getUniformLocation("MVP"), 1, GL_FALSE, &MVP[0][0]);

            // Render state for sky: don't write depth, draw inside of sphere
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);          // view inner surface of sphere
            glDepthFunc(GL_LEQUAL);       // allow sky drawn at far plane
            glDepthMask(GL_FALSE);        // don't write depth

            skyRenderer.draw();           // draw the sphere mesh

            // restore state
            glDepthMask(GL_TRUE);
            glCullFace(GL_BACK);
            glDepthFunc(GL_LESS);
        }




        // Render all entities
        for (Entity* root : world.getEntityManager().getRoots()) {
            world.getEntityManager().renderEntityRecursive(root, VP);
        }

        // ==========================
        // ImGui New Frame
        // ==========================
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --------------------------
        // Example UI Window
        // --------------------------
        ImGui::Begin("Hello ImGui");
        ImGui::Text("This is working!");
        
        ImGuiHelpers::ShowTransformInspector("Selected", island);
        
        ImGui::End();

        // ==========================
        // Render ImGui
        // ==========================
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        // Swap buffers for the next frame
        window.swap_buffers();
    }



    // Cleanup
    cube.destroy();
    // mainShader.destroy();

    // ImGui cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}