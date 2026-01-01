cmake_minimum_required(VERSION 3.10)
project(OpenGLEngine VERSION 0.1.0)

# ==========================================================
# =================== C++ CONFIGURATION ====================
# ==========================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENGINE_BUILD_SHARED "Build Engine as a shared library instead of static" OFF)
option(ENGINE_BUILD_EXAMPLES "Build example apps (src/Game)" ON)
option(ENGINE_ENABLE_TESTS "Enable building tests (tests/)" ON)

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
set(GLAD_SOURCES ${CMAKE_SOURCE_DIR}/vendor/glad/src/glad.c)
add_library(glad STATIC ${GLAD_SOURCES})
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glad/include)

# ---------- GLM ----------
include_directories(${CMAKE_SOURCE_DIR}/vendor/glm)

# ---------- stb ----------
include_directories(${CMAKE_SOURCE_DIR}/vendor/stb)

# ---------- tiny_obj_loader ----------
include_directories(${CMAKE_SOURCE_DIR}/vendor)

# ---------- ImGui ----------
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/vendor/imgui)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${CMAKE_SOURCE_DIR}/vendor/glfw/include
)

# Link GLFW + OpenGL (needed by backends)
target_link_libraries(imgui PUBLIC
    glfw
    ${OPENGL_gl_LIBRARY}
)



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
    ${CMAKE_SOURCE_DIR}/src/engine/animations/*.cpp
)

# ==========================================================
# =============== ENGINE LIBRARY CREATION ==================
# ==========================================================
if (ENGINE_BUILD_SHARED)
    add_library(Engine SHARED ${ENGINE_SOURCES})
else()
    add_library(Engine STATIC ${ENGINE_SOURCES})
endif()

target_include_directories(Engine
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src                     # internal headers
        ${CMAKE_SOURCE_DIR}/src/include             # public engine.hpp
        ${CMAKE_SOURCE_DIR}/vendor/glfw/include
        ${CMAKE_SOURCE_DIR}/vendor/glad/include
        ${CMAKE_SOURCE_DIR}/vendor/json/single_include
        ${CMAKE_SOURCE_DIR}/vendor/glm
        ${CMAKE_SOURCE_DIR}/vendor/stb
        ${CMAKE_SOURCE_DIR}/vendor/tinygltf
        ${CMAKE_SOURCE_DIR}/vendor                  # for tiny_obj_loader.h and tinygltf
)

target_link_libraries(Engine
    PUBLIC
        glfw
        glad
        imgui
        imgui
        ${OPENGL_gl_LIBRARY}
)

target_compile_definitions(Engine PUBLIC
    _CRT_SECURE_NO_WARNINGS
    SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders"
    ASSET_DIR="${CMAKE_SOURCE_DIR}/assets"
)

# ==========================================================
# ============== COMPILER WARNINGS & PROPERTIES ============
# ==========================================================
if (MSVC)
    target_compile_options(Engine PRIVATE /W4)
else()
    target_compile_options(Engine PRIVATE -Wall -Wextra -pedantic)
endif()

set_target_properties(Engine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# ==========================================================
# ============== APPLICATION EXECUTABLE SETUP ==============
# ==========================================================
if (ENGINE_BUILD_EXAMPLES)
    add_executable(OpenGLEngine
        src/Game/main.cpp
        src/Game/Entities/Player.cpp
        src/Game/Entities/Enemy.cpp
        src/Game/Entities/Crusader.cpp
    )

    target_link_libraries(OpenGLEngine PRIVATE Engine)

    target_compile_definitions(OpenGLEngine PRIVATE
        _CRT_SECURE_NO_WARNINGS
        SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders"
        ASSET_DIR="${CMAKE_SOURCE_DIR}/assets"
    )

    set_target_properties(OpenGLEngine PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

# ==========================================================
# ================== UNIT TESTS (OPTIONAL) =================
# ==========================================================
if (ENGINE_ENABLE_TESTS)
    enable_testing()

    # ----- Animation tests -----
    add_executable(animation_tests
        ${CMAKE_SOURCE_DIR}/tests/animation_tests.cpp
    )
    target_link_libraries(animation_tests PRIVATE Engine)
    add_test(NAME animation_tests COMMAND animation_tests)

    # ----- Asset tests -----
    add_executable(asset_tests
        ${CMAKE_SOURCE_DIR}/tests/asset_tests.cpp
    )
    target_link_libraries(asset_tests PRIVATE Engine)
    add_test(NAME asset_tests COMMAND asset_tests)

    # ----- ECS tests -----
    add_executable(ecs_tests
        ${CMAKE_SOURCE_DIR}/tests/ecs_tests.cpp
    )
    target_link_libraries(ecs_tests PRIVATE Engine)
    add_test(NAME ecs_tests COMMAND ecs_tests)

    # ----- Animation demo (has its own main) -----
    add_executable(animation_demo
        ${CMAKE_SOURCE_DIR}/tests/animation_demo.cpp
    )
    target_link_libraries(animation_demo PRIVATE Engine glfw glad imgui)
    target_compile_definitions(animation_demo PRIVATE
        _CRT_SECURE_NO_WARNINGS
        SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders"
        ASSET_DIR="${CMAKE_SOURCE_DIR}/assets"
        SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    )
    add_test(NAME animation_demo COMMAND animation_demo)
endif()
