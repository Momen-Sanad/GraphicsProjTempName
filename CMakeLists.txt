cmake_minimum_required(VERSION 3.10) 
project(OpenGLEngine VERSION 0.1.0) 
# Define a name & a version for our project. 
# This is unrelated to the executable that we will define later.

# Tell CMake that we want to use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==========================================================
# =============== GLFW CONFIGURATION OPTIONS ===============
# ==========================================================
# These options tell CMake not to build any of the extra stuff that comes with GLFW.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)        # Don't build Documentation
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)       # Don't build Tests
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)    # Don't build Examples
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)           # Don't build Installation Information
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)     # Use High Performance GPU if available

# This tells CMake to include GLFW’s CMake file into our project.
# By doing so, the "glfw" target will be defined, and we can link it later.
add_subdirectory(vendor/glfw EXCLUDE_FROM_ALL)

# ==========================================================
# ===================== OPENGL DETECTION ====================
# ==========================================================
# This will automatically link the system's OpenGL library (e.g. libGL.so on Linux).
find_package(OpenGL REQUIRED)

# ==========================================================
# ================== SOURCE FILE COLLECTION =================
# ==========================================================
# This command automatically collects all .cpp files from inside src/
# so we don’t need to list them manually every time we add a new file.
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    src/core/*.cpp
    src/systems/*.cpp
    src/components/*.cpp
    src/assets/*.cpp
    src/graphics/*.cpp
    src/platform/*.cpp
    # Note: do NOT include src/app/*.cpp here — app/main.cpp is compiled into the final executable only.
)

# Tip:
# You could also use a simpler pattern:
# file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS src/*/*.cpp)
# This version is cleaner but might include unwanted folders (e.g. src/tests).

# ==========================================================
# =================== ENGINE LIBRARY CREATION ===================
# ==========================================================
# This creates a static library called "Engine" which contains all engine code.
# The executable will later link against this library.
add_library(Engine STATIC
    ${ENGINE_SOURCES}
    vendor/glad/src/gl.c
)

# ==========================================================
# ================== INCLUDE DIRECTORIES ====================
# ==========================================================
# This tells the compiler where to find the header files for GLFW, glad, stb, and glm.
# We set these as PUBLIC because consumers of the Engine library (like the OpenGLEngine executable)
# need these include paths when compiling their translation units.
target_include_directories(Engine
    PUBLIC
       ${CMAKE_SOURCE_DIR}/src        # so #include <Application.hpp> works
       ${CMAKE_SOURCE_DIR}/vendor/glfw/include
       ${CMAKE_SOURCE_DIR}/vendor/glad/include
       ${CMAKE_SOURCE_DIR}/vendor/stb
       ${CMAKE_SOURCE_DIR}/vendor/glm
)

# ==========================================================
# ===================== LINK LIBRARIES ======================
# ==========================================================
# Link libraries needed by the engine (GLFW and system OpenGL).
# Mark PUBLIC so anything that links Engine sees the link requirement if necessary.
target_link_libraries(Engine
    PUBLIC
       glfw
       ${OPENGL_gl_LIBRARY}
)

# ==========================================================
# ================== COMPILER DEFINITIONS ===================
# ==========================================================
# Compilers on Windows don't like the fopen function, so we disable those warnings.
# Equivalent to "#define _CRT_SECURE_NO_WARNINGS" at the top of each file.
target_compile_definitions(Engine PRIVATE _CRT_SECURE_NO_WARNINGS)

# This defines a compile-time variable (macro) SHADER_DIR so the engine can find shaders at runtime.
# It allows the engine to know where shaders are located, no matter where the program runs.
# Use it in code as: std::string(SHADER_DIR) + "/main.vert"
target_compile_definitions(Engine PRIVATE SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders")

# ==========================================================
# ============== ENABLE USEFUL COMPILER WARNINGS ============
# ==========================================================
# These warnings help catch subtle issues early during compilation.
if (MSVC)
    target_compile_options(Engine PRIVATE /W4)
else()
    target_compile_options(Engine PRIVATE -Wall -Wextra -pedantic)
endif()

# ==========================================================
# ================== OUTPUT DIRECTORY SETUP =================
# ==========================================================
# This tells CMake where to place the final compiled executable.
# Example: if you're building in "build/", the output will be "build/OpenGLEngine"
set_target_properties(Engine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# ==========================================================
# ===================== IDE ORGANIZATION ====================
# ==========================================================
# This organizes files in IDEs (like CLion or Visual Studio) for easier navigation.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${ENGINE_SOURCES})

# ----------------------------------------------------------
# Create the final executable which only compiles the user app (main.cpp)
# ----------------------------------------------------------
# The executable is small; it links to the Engine library which has all the engine code.
add_executable(OpenGLEngine
    src/app/main.cpp
)

# Link the Engine library into the executable.
target_link_libraries(OpenGLEngine PRIVATE Engine)

# This tells CMake where to place the final compiled executable.
# Example: if you're building in "build/", the output will be "build/OpenGLEngine"
set_target_properties(OpenGLEngine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)