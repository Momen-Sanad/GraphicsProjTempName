#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

// ----------------------------------------------------
// Destructor: Cleans up the shader program and its resources
// ----------------------------------------------------
Shader::~Shader() {
    destroy();  // Calls the destroy function to delete the shader program
}

// ----------------------------------------------------
// Move constructor: Transfers ownership of resources from another shader object
// ----------------------------------------------------
Shader::Shader(Shader&& other) noexcept {
    *this = std::move(other);  // Uses move assignment operator to transfer resources
}

// ----------------------------------------------------
// Move assignment: Releases current resources and transfers ownership from another shader
// ----------------------------------------------------
Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        destroy();  // Cleans up any existing resources

        // Transfers the program and uniform cache
        program = other.program;
        uniformCache = std::move(other.uniformCache);

        // Nullify the source object to avoid double deletion
        other.program = 0;
    }
    return *this;
}

// ----------------------------------------------------
// File reading helper: Reads the contents of a shader file
// ----------------------------------------------------
std::string Shader::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return "";  // Returns an empty string if file cannot be opened
    }
    
    std::stringstream ss;
    ss << file.rdbuf();  // Read the entire file content into a stringstream
    return ss.str();     // Return the file contents as a string
}

// ----------------------------------------------------
// Shader compiler: Compiles a shader from source code
// ----------------------------------------------------
GLuint Shader::compileShader(const std::string& source, GLenum type) {
    GLuint shader = glCreateShader(type);  // Creates the shader object (either vertex or fragment)

    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);  // Pass the shader source code to OpenGL
    glCompileShader(shader);  // Compile the shader

    // Check compile status
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(logLength, ' ');
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());  // Retrieve the error log
        std::cerr << "Shader compilation failed:\n" << log << std::endl;
        glDeleteShader(shader);  // Clean up the shader if compilation fails
        return 0;  // Return 0 if compilation failed
    }

    return shader;  // Return the compiled shader ID
}

// ----------------------------------------------------
// Create shader from source code: Compiles and links vertex and fragment shaders
// ----------------------------------------------------
bool Shader::createFromSources(const std::string& vsSource,
                               const std::string& fsSource)
{
    destroy();  // Clean up existing resources before creating new ones

    // Compile the vertex and fragment shaders
    GLuint vs = compileShader(vsSource, GL_VERTEX_SHADER);
    if (!vs) return false;

    GLuint fs = compileShader(fsSource, GL_FRAGMENT_SHADER);
    if (!fs) {
        glDeleteShader(vs);  // Clean up if fragment shader compilation fails
        return false;
    }

    // Create the shader program and attach the compiled shaders
    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);  // Link the shaders into a program

    glDeleteShader(vs);  // Delete individual shaders after linking
    glDeleteShader(fs);

    // Check if the program was successfully linked
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(logLength, ' ');
        glGetProgramInfoLog(program, logLength, nullptr, log.data());  // Retrieve the program link error log
        std::cerr << "Shader link failed:\n" << log << std::endl;
        glDeleteProgram(program);  // Clean up if linking failed
        program = 0;
        return false;
    }

    return true;  // Return true if the program was successfully created and linked
}

// ----------------------------------------------------
// Create shader from file paths: Reads shader files and creates a program
// ----------------------------------------------------
bool Shader::createFromFiles(const std::string& vsPath,
                             const std::string& fsPath)
{
    std::string vs = readFile(vsPath);  // Read the vertex shader source file
    std::string fs = readFile(fsPath);  // Read the fragment shader source file

    if (vs.empty() || fs.empty())  // Check if reading the files was successful
        return false;

    return createFromSources(vs, fs);  // Use the source strings to create the shader program
}

// ----------------------------------------------------
// Cleanup: Deletes the shader program and clears the uniform cache
// ----------------------------------------------------
void Shader::destroy() {
    if (program) {
        glDeleteProgram(program);  // Deletes the shader program from OpenGL
        program = 0;  // Set program ID to 0 to indicate it's been deleted
    }
    uniformCache.clear();  // Clear the cached uniform locations
}

// ----------------------------------------------------
// Use program: Activates the shader program for rendering
// ----------------------------------------------------
void Shader::use() const {
    glUseProgram(program);  // Use the program in OpenGL's current rendering context
}

// ----------------------------------------------------
// Cached uniform locations: Retrieves or caches the location of a uniform variable
// ----------------------------------------------------
GLint Shader::getUniformLocation(const std::string& name) const {
    // Check if the uniform location is already cached
    auto it = uniformCache.find(name);
    if (it != uniformCache.end())
        return it->second;  // Return the cached location if found

    // Otherwise, retrieve the location from OpenGL
    GLint loc = glGetUniformLocation(program, name.c_str());
    uniformCache[name] = loc;  // Cache the uniform location for future use
    return loc;
}
