cmake_minimum_required(VERSION 3.10)
project(OpenGLEngine VERSION 0.1.0)

# ==========================================================
# =================== C++ CONFIGURATION ====================
# ==========================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENGINE_BUILD_SHARED "Build Engine as a shared library instead of static" OFF)
option(ENGINE_BUILD_EXAMPLES "Build example apps (src/app)" ON)
option(ENGINE_ENABLE_TESTS "Enable building tests (tests/)" OFF)

# ==========================================================
# ================== GLFW CONFIGURATION ====================
# ==========================================================
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)

# ==========================================================
# =================== THIRD-PARTY LIBS =====================
# ==========================================================

# ---------- GLFW ----------
add_subdirectory(vendor/glfw EXCLUDE_FROM_ALL)

# ---------- GLAD ----------
set(GLAD_SOURCES ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c)
add_library(glad STATIC ${GLAD_SOURCES})
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glad/include)

# ==========================================================
# =================== OPENGL DETECTION =====================
# ==========================================================
find_package(OpenGL REQUIRED)

# ==========================================================
# =============== ENGINE SOURCE COLLECTION =================
# ==========================================================
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/engine/core/*.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/ecs/*.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/components/*.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/assets/*.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/gl/*.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/platform/*.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/scripting/*.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/systems/*.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/utils/*.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/scene/*.cpp
)

# ==========================================================
# =============== ENGINE LIBRARY CREATION ==================
# ==========================================================
if (ENGINE_BUILD_SHARED)
    add_library(Engine SHARED ${ENGINE_SOURCES})
else()
    add_library(Engine STATIC ${ENGINE_SOURCES})
endif()

# Add include dirs *after* Engine exists
target_include_directories(Engine PUBLIC
    ${CMAKE_SOURCE_DIR}/src                     # internal headers
    ${CMAKE_SOURCE_DIR}/src/include             # public engine.hpp
    ${CMAKE_SOURCE_DIR}/vendor/glfw/include
    ${CMAKE_SOURCE_DIR}/vendor/glad/include
    ${CMAKE_SOURCE_DIR}/vendor/glm              # <<< GLM fixed here
    ${CMAKE_SOURCE_DIR}/vendor/stb              # <<< stb
)

# Link dependencies
target_link_libraries(Engine PUBLIC
    glfw
    glad
    ${OPENGL_gl_LIBRARY}
)

# Preprocessor definitions
target_compile_definitions(Engine PRIVATE
    _CRT_SECURE_NO_WARNINGS
    SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders"
    ASSET_DIR="${CMAKE_SOURCE_DIR}/assets"
)

# Compiler warnings
if (MSVC)
    target_compile_options(Engine PRIVATE /W4)
else()
    target_compile_options(Engine PRIVATE -Wall -Wextra -pedantic)
endif()

# Output directories
set_target_properties(Engine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# ==========================================================
# ============== APPLICATION EXECUTABLE SETUP ==============
# ==========================================================
if (ENGINE_BUILD_EXAMPLES)
    add_executable(OpenGLEngine src/app/main.cpp)

    target_link_libraries(OpenGLEngine PRIVATE Engine)

    set_target_properties(OpenGLEngine PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

# ==========================================================
# ================== UNIT TESTS (OPTIONAL) =================
# ==========================================================
if (ENGINE_ENABLE_TESTS)
    enable_testing()

    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/tests/*.cpp)
    add_executable(EngineTests ${TEST_SOURCES})
    target_link_libraries(EngineTests PRIVATE Engine)
    add_test(NAME EngineTests COMMAND EngineTests)
endif()