cmake_minimum_required(VERSION 3.10)
project(OpenGLEngine VERSION 0.1.0)

# ==========================================================
# =================== C++ CONFIGURATION ====================
# ==========================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENGINE_BUILD_SHARED "Build Engine as a shared library instead of static" OFF)
option(ENGINE_BUILD_EXAMPLES "Build example apps (src/app)" ON)
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
# glad is typically just a single C file with headers; we compile it manually.
set(GLAD_SOURCES ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c)
add_library(glad STATIC ${GLAD_SOURCES})
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glad/include)

# ---------- GLM ----------
# GLM is header-only, so no target needed, just add include dir.
include_directories(${CMAKE_SOURCE_DIR}/vendor/glm)

# ---------- stb ----------
# stb is also header-only.
include_directories(${CMAKE_SOURCE_DIR}/vendor/stb)

# ---------- ImGui ----------
set(IMGUI_SOURCES
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_opengl3.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    ${CMAKE_SOURCE_DIR}/vendor/imgui
    ${CMAKE_SOURCE_DIR}/vendor/imgui/backends
    ${CMAKE_SOURCE_DIR}/vendor/glfw/include
    ${CMAKE_SOURCE_DIR}/vendor/glad/include
)
target_link_libraries(imgui PUBLIC glfw glad)

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
        ${CMAKE_SOURCE_DIR}/vendor/glm
        ${CMAKE_SOURCE_DIR}/vendor/stb
        ${CMAKE_SOURCE_DIR}/vendor                  # for tiny_obj_loader.h and tinygltf
)

target_link_libraries(Engine
    PUBLIC
        glfw
        glad
        imgui
        ${OPENGL_gl_LIBRARY}
)

target_compile_definitions(Engine PRIVATE
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
        src/app/main.cpp
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

    # Optional: copy shaders/assets into build directory
    # add_custom_target(copy_assets ALL
    #     COMMAND ${CMAKE_COMMAND} -E copy_directory
    #     ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
    # )
    # add_dependencies(OpenGLEngine copy_assets)
endif()

# ==========================================================
# ================== UNIT TESTS (OPTIONAL) =================
# ==========================================================
if (ENGINE_ENABLE_TESTS)
    enable_testing()

    # Animation tests executable
    add_executable(animation_tests ${CMAKE_SOURCE_DIR}/tests/animation_tests.cpp)
    target_link_libraries(animation_tests PRIVATE Engine)
    target_compile_definitions(animation_tests PRIVATE
        _CRT_SECURE_NO_WARNINGS
    )
    add_test(NAME animation_tests COMMAND animation_tests)

    # Generic engine tests (all test files)
    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/tests/*.cpp)
    add_executable(EngineTests ${TEST_SOURCES})
    target_link_libraries(EngineTests PRIVATE Engine)
    add_test(NAME EngineTests COMMAND EngineTests)
endif()

add_executable(animation_demo tests/animation_demo.cpp)
target_link_libraries(animation_demo Engine glfw glad imgui)
target_compile_definitions(animation_demo PRIVATE
    _CRT_SECURE_NO_WARNINGS
    SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders"
    ASSET_DIR="${CMAKE_SOURCE_DIR}/assets"
    SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

# ==========================================================
# ================= Extra helpful CMake hints ==============
# ==========================================================
# - If the engine gets large, consider splitting src/engine into its own CMake subdirectory
#   (add_subdirectory(src/engine)), with a small src/engine/CMakeLists.txt that lists implementation files.
# - Consider exporting a CMake config file so other projects can use find_package(OpenGLEngine).
# - If you want to support editor-only code, put it under src/editor and do NOT include it into ENGINE_SOURCES.
#
# End of CMakeLists.txt
