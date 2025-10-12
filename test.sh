# create folders
mkdir -p src/{core,systems,components,assets,platform,app}
mkdir -p shaders assets/{models,textures,scenes}
mkdir -p examples third_party/vendor/{glad,glfw}

# add .gitkeep to keep empty dirs in git
for d in src/core src/systems src/components src/assets src/platform src/app shaders assets/models assets/textures assets/scenes examples third_party/vendor/glad third_party/vendor/glfw; do
  touch "$d/.gitkeep"
done

# create top-level files
touch CMakeLists.txt README.md .gitignore

# init git repo (optional)
git init

# populate .gitignore (adjust as needed)
cat > .gitignore <<'EOF'
/build/
/bin/
/*.exe
/*.dll
/*.so
*.obj
*.o
.vscode/
.idea/
*.DS_Store
EOF

# minimal CMakeLists.txt starter (edit to fit your project)
cat > CMakeLists.txt <<'EOF'
cmake_minimum_required(VERSION 3.10)
project(OpenGLEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY \${CMAKE_BINARY_DIR}/bin)

# add your subprojects or source files here, for example:
# add_executable(engine src/app/main.cpp)
# target_link_libraries(engine PRIVATE glfw glad glm)

EOF

# minimal README placeholder
cat > README.md <<'EOF'
# Custom OpenGL Engine & 3D Game

Two-phase project: (1) OpenGL engine (ECS, asset deserialization, forward renderer) (2) 3D game using the engine.
See full README for details.
EOF

# tiny main.cpp stub so build can be tested quickly
cat > src/app/main.cpp <<'EOF'
#include <iostream>

int main() {
    std::cout << "OpenGL Engine stub\\n";
    return 0;
}
EOF

# stage initial files and make a first commit (optional)
git add .
git commit -m "Initial project skeleton: src/, shaders/, assets/, examples/, third_party/vendor/"
