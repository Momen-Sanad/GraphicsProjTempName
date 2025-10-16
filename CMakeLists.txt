cmake_minimum_required(VERSION 3.10) # Define the minimum cmake version that can run this file.
project(OpenGLEngine VERSION 0.1.0) # Define a name & a version for our project. This is unrelated to the executable that we will define later.

# Tell cmake that we want to use C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# These options tell CMake not to build any of the extra stuff that comes with GLFW.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)        # Don't build Documentation
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)       # Don't build Tests
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)    # Don't build Examples
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)           # Don't build Installation Information
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)     # Add variables to use High Performance Graphics Card if available

# This tells cmake to include GLFW's cmake file into our cmake project.
# By doing so, the "glfw" library target will be defined, and we will be able to link it to our executable.
add_subdirectory(vendor/glfw EXCLUDE_FROM_ALL)

# Tell CMake that we need OpenGL. On Linux this links the OpenGL library automatically.
find_package(OpenGL REQUIRED)

# This command automatically collects all .cpp files from the src/ folders so we don’t need to list them manually.
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    src/core/*.cpp
    src/systems/*.cpp
    src/components/*.cpp
    src/assets/*.cpp
    src/platform/*.cpp
    src/app/*.cpp
)

# Safer if we later add folders that shouldn’t be compiled (e.g. src/tests or src/experimental).
# & clearer for viewing
# file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS src/*/*.cpp)


# This creates a new executable called "OpenGLEngine" (e.g., on Windows, it will be called "OpenGLEngine.exe")
# The executable is created by compiling the following c/c++ files and linking some libraries (which we will define later)
add_executable(OpenGLEngine
    ${ENGINE_SOURCES}
    vendor/glad/src/gl.c
)

# This tells the compiler where to find the header files for GLFW, glad, stb, and glm.
target_include_directories(OpenGLEngine PRIVATE 
   vendor/glfw/include
   vendor/glad/include
   vendor/stb
   vendor/glm
   src
)

# This tells the linker to link the GLFW and OpenGL libraries to our executable.
target_link_libraries(OpenGLEngine PRIVATE glfw ${OPENGL_gl_LIBRARY})

# Compilers on Windows don't like the fopen function, so we disable the C-runtime security warnings.
# This is like adding "#define _CRT_SECURE_NO_WARNINGS" at the start of each file.
target_compile_definitions(OpenGLEngine PRIVATE _CRT_SECURE_NO_WARNINGS)

# Enable useful compiler warnings to catch errors early.
if (MSVC)
    target_compile_options(OpenGLEngine PRIVATE /W4)
else()
    target_compile_options(OpenGLEngine PRIVATE -Wall -Wextra -pedantic)
endif()

# Set where the compiled executable will be placed (e.g., ../build)
set_target_properties(OpenGLEngine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# This organizes files inside IDEs like Visual Studio or CLion for easier navigation.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${ENGINE_SOURCES})
