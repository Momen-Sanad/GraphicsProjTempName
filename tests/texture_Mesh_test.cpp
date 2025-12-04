#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "engine/assets/TextureLoader.hpp"
#include "engine/gl/Mesh.hpp"
#include "engine/assets/MeshLoader.hpp"

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLuint setupShader();
void setupMeshBuffers(const Mesh& mesh, GLuint& VAO, GLuint& VBO, GLuint& EBO);
void renderMesh(GLuint VAO, GLsizei indexCount);
void checkGLError(const std::string& context);

// Struct to hold OpenGL buffers for each mesh
struct MeshBuffers {
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    GLsizei indexCount = 0;
};

// Camera settings
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraSpeed = 0.05f;
float fov = 45.0f;

// Mouse control
bool firstMouse = true;
float lastX = 400.0f, lastY = 300.0f;
float yaw = -90.0f, pitch = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Constrain pitch to avoid screen flip
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 90.0f) fov = 90.0f;
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(1200, 800, "Texture and Mesh Loader Test", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLAD
    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Get OpenGL version
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "OpenGL " << version << std::endl;

    // Setup basic OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);  // Blue background

    // Setup shader
    GLuint shaderProgram = setupShader();

    std::cout << "\n=== Texture Loader Test ===\n" << std::endl;

    std::string texturePath = std::string(TEST_ASSET_DIR) + "/moon.jpg";
    std::cout << "Attempting to load texture: " << texturePath << std::endl;

    // Test 1: Texture Loading
    Texture* texture = TextureLoader::load(texturePath);
    if (texture)
    {
        std::cout << "Texture loaded successfully!" << std::endl;
        std::cout << "  Size: " << texture->get_width() << "x" << texture->get_height() << std::endl;
        std::cout << "  Loaded textures count: " << TextureLoader::get_loaded_count() << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture. Using fallback..." << std::endl;
        // Create a simple fallback texture (checkerboard)
        unsigned char fallbackData[16 * 16 * 3];
        for (int y = 0; y < 16; y++) {
            for (int x = 0; x < 16; x++) {
                unsigned char color = ((x + y) % 2) * 255;
                fallbackData[(y * 16 + x) * 3 + 0] = color;
                fallbackData[(y * 16 + x) * 3 + 1] = color;
                fallbackData[(y * 16 + x) * 3 + 2] = color;
            }
        }
        texture = new Texture();
        texture->create(16, 16, fallbackData, GL_RGB);
    }

    // After loading the moon texture, load house texture too
    std::cout << "\n=== Loading House Texture ===\n" << std::endl;
    std::string houseTexturePath = std::string(TEST_ASSET_DIR) + "/house/house.jpeg";
    std::cout << "Attempting to load house texture: " << houseTexturePath << std::endl;

    Texture* house_texture = TextureLoader::load(houseTexturePath);
    if (house_texture)
    {
        std::cout << "House texture loaded successfully!" << std::endl;
        std::cout << "  Size: " << house_texture->get_width() << "x" << house_texture->get_height() << std::endl;
    }
    else
    {
        std::cout << "Failed to load house texture. Will use moon texture as fallback." << std::endl;
        house_texture = texture; // Use moon texture as fallback
    }

    std::cout << "\n=== Mesh Class Test ===\n" << std::endl;

    // Create primitive meshes
    std::cout << "Creating primitive meshes..." << std::endl;

    // Create meshes
    Mesh cube = Mesh::create_cuboid(glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f));
    Mesh plane = Mesh::create_plane(glm::vec3(0.0f, -2.0f, 0.0f), glm::vec2(4.0f), glm::vec2(2.0f));
    Mesh sphere = Mesh::create_sphere(glm::ivec2(32, 16), glm::vec3(3.0f, 2.0f, 0.0f), 1.0f);
    Mesh cylinder = Mesh::create_cylinder(32, glm::vec3(-3.0f, -1.0f, 0.0f), 2.0f, 0.5f);

    std::cout << "Cube created: " << cube.get_vert_count() << " vertices" << std::endl;
    std::cout << "Plane created: " << plane.get_vert_count() << " vertices" << std::endl;
    std::cout << "Sphere created: " << sphere.get_vert_count() << " vertices" << std::endl;
    std::cout << "Cylinder created: " << cylinder.get_vert_count() << " vertices" << std::endl;

    std::cout << "\n=== Mesh Loader Test ===\n" << std::endl;

    // Test 3: Mesh Loading from File
    std::string meshPath = std::string(TEST_ASSET_DIR) + "/house/house.obj";
    std::cout << "Attempting to load mesh: " << meshPath << std::endl;

    Mesh* loadedMesh = MeshLoader::load(meshPath.c_str());
    if (loadedMesh)
    {
        std::cout << "Mesh loaded successfully!" << std::endl;
        std::cout << "  Vertex count: " << loadedMesh->get_vert_count() << std::endl;
        std::cout << "  Loaded meshes count: " << MeshLoader::get_loaded_count() << std::endl;
    }
    else
    {
        std::cout << "Failed to load mesh. Using fallback cube..." << std::endl;
        loadedMesh = new Mesh();
        *loadedMesh = cube;
    }

    // Create OpenGL buffers for each mesh
    std::unordered_map<std::string, MeshBuffers> meshBuffers;

    setupMeshBuffers(cube, meshBuffers["cube"].VAO, meshBuffers["cube"].VBO, meshBuffers["cube"].EBO);
    meshBuffers["cube"].indexCount = cube.get_vert_count();

    setupMeshBuffers(plane, meshBuffers["plane"].VAO, meshBuffers["plane"].VBO, meshBuffers["plane"].EBO);
    meshBuffers["plane"].indexCount = plane.get_vert_count();

    setupMeshBuffers(sphere, meshBuffers["sphere"].VAO, meshBuffers["sphere"].VBO, meshBuffers["sphere"].EBO);
    meshBuffers["sphere"].indexCount = sphere.get_vert_count();

    setupMeshBuffers(cylinder, meshBuffers["cylinder"].VAO, meshBuffers["cylinder"].VBO, meshBuffers["cylinder"].EBO);
    meshBuffers["cylinder"].indexCount = cylinder.get_vert_count();

    setupMeshBuffers(*loadedMesh, meshBuffers["house"].VAO, meshBuffers["house"].VBO, meshBuffers["house"].EBO);
    meshBuffers["house"].indexCount = loadedMesh->get_vert_count();

    // Main render loop
    std::cout << "\n=== Starting Render Loop ===\n" << std::endl;
    std::cout << "Press ESC to exit" << std::endl;
    std::cout << "Use WASD to move, mouse to look around" << std::endl;

    float lastTime = glfwGetTime();
    int frameCount = 0;
    float rotation = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        frameCount++;
        if (currentTime - lastTime >= 1.0f) {
            std::cout << "FPS: " << frameCount << std::endl;
            frameCount = 0;
            lastTime = currentTime;
        }


        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            static bool wireframe = false;
            wireframe = !wireframe;
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        }
        // Input
        processInput(window);

        // Clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use shader
        glUseProgram(shaderProgram);

        // Set view and projection matrices
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(fov), 1200.0f / 800.0f, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Bind texture
        if (texture)
        {
            texture->bind(0);
            glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        }

        // Update rotation
        rotation += 0.01f;

        // Render Cube
        glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 2.0f, 0.0f));
        cubeModel = glm::rotate(cubeModel, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(cubeModel));
        renderMesh(meshBuffers["cube"].VAO, meshBuffers["cube"].indexCount);

        // Render Plane
        glm::mat4 planeModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(planeModel));
        renderMesh(meshBuffers["plane"].VAO, meshBuffers["plane"].indexCount);

        // Render Sphere
        glm::mat4 sphereModel = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 2.0f, 0.0f));
        sphereModel = glm::rotate(sphereModel, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(sphereModel));
        renderMesh(meshBuffers["sphere"].VAO, meshBuffers["sphere"].indexCount);

        // Render Cylinder
        glm::mat4 cylinderModel = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -1.0f, 0.0f));
        cylinderModel = glm::rotate(cylinderModel, rotation, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(cylinderModel));
        renderMesh(meshBuffers["cylinder"].VAO, meshBuffers["cylinder"].indexCount);

        // Bind house texture
        if (house_texture)
        {
            house_texture->bind(0);
            glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        }

        // Render Loaded House
        glm::mat4 houseModel = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, -1.0f, 0.0f));
        houseModel = glm::rotate(houseModel, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        houseModel = glm::scale(houseModel, glm::vec3(0.1f));  // Scale down the house
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(houseModel));
        renderMesh(meshBuffers["house"].VAO, meshBuffers["house"].indexCount);

        // Check for OpenGL errors
        checkGLError("Render loop");

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    std::cout << "\n=== Cleanup ===\n" << std::endl;

    // Cleanup OpenGL buffers
    for (auto& pair : meshBuffers) {
        glDeleteVertexArrays(1, &pair.second.VAO);
        glDeleteBuffers(1, &pair.second.VBO);
        glDeleteBuffers(1, &pair.second.EBO);
    }

    // Cleanup textures and meshes
    TextureLoader::unload_all();
    std::cout << "Textures unloaded. Remaining: " << TextureLoader::get_loaded_count() << std::endl;

    MeshLoader::unload_all();
    std::cout << "Meshes unloaded. Remaining: " << MeshLoader::get_loaded_count() << std::endl;

    // Delete shader
    glDeleteProgram(shaderProgram);

    // Terminate GLFW
    glfwTerminate();

    std::cout << "\nTest completed successfully!" << std::endl;
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

GLuint setupShader()
{
    // Vertex shader
    const char* vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec4 aColor;
        layout(location = 2) in vec2 aTexCoord;
        
        out vec4 vertexColor;
        out vec2 texCoord;
        out vec3 fragPos;  // Pass position to fragment shader
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        void main()
        {
            vec4 worldPos = model * vec4(aPos, 1.0);
            gl_Position = projection * view * worldPos;
            vertexColor = aColor;
            texCoord = aTexCoord;
            fragPos = worldPos.xyz;  // Store world position
        }
    )";

    // Fragment shader - FIXED: Removed gl_Position reference
    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec4 vertexColor;
        in vec2 texCoord;
        in vec3 fragPos;
        out vec4 FragColor;
        
        uniform sampler2D texture1;
        uniform vec3 lightPos = vec3(10.0, 10.0, 10.0);
        uniform vec3 viewPos = vec3(0.0, 0.0, 10.0);
        
        void main()
        {
            vec4 texColor = texture(texture1, texCoord);
            
            // Simple directional lighting
            vec3 lightDir = normalize(lightPos - fragPos);
            vec3 viewDir = normalize(viewPos - fragPos);
            vec3 reflectDir = reflect(-lightDir, vec3(0.0, 1.0, 0.0)); // Approximate normal
            
            float ambient = 0.3;
            float diffuse = max(dot(vec3(0.0, 1.0, 0.0), lightDir), 0.0);
            float specular = pow(max(dot(viewDir, reflectDir), 0.0), 32.0) * 0.5;
            
            float light = ambient + diffuse + specular;
            light = min(light, 1.0);
            
            // Mix texture with vertex color
            vec4 baseColor = mix(texColor, vertexColor, 0.3);
            FragColor = baseColor * vec4(vec3(light), 1.0);
            
            // Add gamma correction
            FragColor.rgb = pow(FragColor.rgb, vec3(1.0/2.2));
        }
    )";

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check vertex shader
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
        return 0;
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check fragment shader
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
        return 0;
    }

    // Link shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check linking
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
        return 0;
    }

    // Cleanup shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::cout << "Shader setup complete" << std::endl;
    return shaderProgram;
}

void setupMeshBuffers(const Mesh& mesh, GLuint& VAO, GLuint& VBO, GLuint& EBO)
{
    std::vector<Vertex> vertices = mesh.get_vertices();
    std::vector<uint16_t> indices = mesh.get_indices();

    if (vertices.empty() || indices.empty()) {
        std::cerr << "Warning: Mesh has no vertices or indices!" << std::endl;
        return;
    }

    // Generate buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);

    // Set vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));

    // Unbind VAO
    glBindVertexArray(0);

    std::cout << "Created buffers for mesh: " << vertices.size() << " vertices, "
        << indices.size() << " indices" << std::endl;
}

void renderMesh(GLuint VAO, GLsizei indexCount)
{
    if (VAO == 0 || indexCount == 0) return;

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

void checkGLError(const std::string& context)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cerr << "OpenGL Error in " << context << ": " << error << std::endl;
    }
}