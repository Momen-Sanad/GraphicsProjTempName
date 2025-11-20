#include <iostream>
#include <engine/core/Application.hpp>
#include <engine/gl/ShaderManager.hpp>
#include <engine/systems/RenderSystem.hpp>

// Your custom MonoBehaviour behavior
class MyGame : public MonoBehaviour {
private:
    GLuint shaderProgram;
    GLuint vao;

public:
    void Start() override {
        std::cout << "MyGame::Start() - one-time initialization" << std::endl;

        ShaderManager shaderManager;
        GLuint vert = shaderManager.LoadShader("triangle.vert", GL_VERTEX_SHADER);
        GLuint frag = shaderManager.LoadShader("triangle.frag", GL_FRAGMENT_SHADER);
        shaderProgram = shaderManager.LinkProgram(vert, frag);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    void Update(float dt) override {
        glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void OnExit() override {
        std::cout << "MyGame::OnExit() - cleanup" << std::endl;
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(shaderProgram);
    }
};

int main() {
    Application app(800, 600, "OpenGL Engine");

    // Run the game with our custom MonoBehaviour behavior
    app.Run(new MyGame());

    return 0;
}
