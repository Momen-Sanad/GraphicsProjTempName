#include <iostream>
#include "core/Application.hpp"
#include "core/ShaderLoader.hpp"

// Example user behaviour similar to Unity’s MonoBehaviour
class MyGame : public MonoBehaviour {
private:
    GLuint shaderProgram = 0;
    GLuint vao = 0;

public:
    void Start() override {
        std::cout << "MyGame::Start() — one-time initialization\n";

        // Load shaders (from the shaders/ directory)
        GLuint vert = LoadVert("triangle.vert");
        GLuint frag = LoadFrag("triangle.frag");
        shaderProgram = LinkProgram(vert, frag);

        // Create a dummy VAO to satisfy core profile requirements
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    void Update(float dt) override {
        // Clear the screen
        glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the shader
        glUseProgram(shaderProgram);

        // Bind VAO and draw
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void OnExit() override {
        std::cout << "MyGame::OnExit() — cleanup before exit\n";
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(shaderProgram);
    }
};

// Entry point — same concept as Unity’s main scene setup
int main() {
    Application app(800, 600, "OpenGLEngine — Unity Style");

    // Attach our custom behaviour
    app.Run(new MyGame());

    return 0;
}
