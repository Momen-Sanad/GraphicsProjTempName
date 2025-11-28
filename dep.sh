echo "Dependencies downloaded successfully!"
#!/bin/bash

VENDOR_DIR="vendor"
mkdir -p $VENDOR_DIR

echo "======================================"
echo "Downloading dependencies..."
echo "======================================"

# GLFW
if [ ! -d "$VENDOR_DIR/glfw" ]; then
    echo "📦 Cloning GLFW..."
    git clone --depth 1 --branch 3.3.8 https://github.com/glfw/glfw.git $VENDOR_DIR/glfw
else
    echo "✓ GLFW already exists"
fi

# GLM
if [ ! -d "$VENDOR_DIR/glm" ]; then
    echo "📦 Cloning GLM..."
    git clone --depth 1 --branch 0.9.9.8 https://github.com/g-truc/glm.git $VENDOR_DIR/glm
else
    echo "✓ GLM already exists"
fi

# fmt
if [ ! -d "$VENDOR_DIR/fmt" ]; then
    echo "📦 Cloning fmt..."
    git clone --depth 1 --branch 10.1.1 https://github.com/fmtlib/fmt.git $VENDOR_DIR/fmt
else
    echo "✓ fmt already exists"
fi

# spdlog
if [ ! -d "$VENDOR_DIR/spdlog" ]; then
    echo "📦 Cloning spdlog..."
    git clone --depth 1 --branch v1.11.0 https://github.com/gabime/spdlog.git $VENDOR_DIR/spdlog
else
    echo "✓ spdlog already exists"
fi

# nlohmann/json
if [ ! -d "$VENDOR_DIR/json" ]; then
    echo "📦 Cloning nlohmann/json..."
    git clone --depth 1 --branch v3.11.2 https://github.com/nlohmann/json.git $VENDOR_DIR/json
else
    echo "✓ nlohmann/json already exists"
fi

# GLAD
if [ ! -d "$VENDOR_DIR/glad" ]; then
    echo "📦 Cloning GLAD..."
    git clone --depth 1 https://github.com/Dav1dde/glad.git $VENDOR_DIR/glad_temp

    # GLAD needs to be generated, so we'll use a pre-built version
    # Alternative: Download pre-generated GLAD files
    echo "⚙️  Setting up GLAD..."
    mkdir -p $VENDOR_DIR/glad/include/glad
    mkdir -p $VENDOR_DIR/glad/include/KHR
    mkdir -p $VENDOR_DIR/glad/src

    # Use glad generator (Python required)
    if command -v python3 &> /dev/null; then
        cd $VENDOR_DIR/glad_temp
        python3 -m glad --generator=c --spec=gl --profile=core --out-path=../ --extensions=GL_ARB_bindless_texture
        cd ../..
        rm -rf $VENDOR_DIR/glad_temp
    else
        echo "⚠️  Python3 not found. Downloading pre-generated GLAD files..."
        # Download pre-generated GLAD (OpenGL 4.6 Core)
        curl -L "https://glad.dav1d.de/generated/tmpfivnqbveglad/glad.zip" -o $VENDOR_DIR/glad.zip
        unzip -q $VENDOR_DIR/glad.zip -d $VENDOR_DIR/glad_extracted
        mv $VENDOR_DIR/glad_extracted/include/* $VENDOR_DIR/glad/include/
        mv $VENDOR_DIR/glad_extracted/src/* $VENDOR_DIR/glad/src/
        rm -rf $VENDOR_DIR/glad_temp $VENDOR_DIR/glad.zip $VENDOR_DIR/glad_extracted
    fi
else
    echo "✓ GLAD already exists"
fi

# Assimp
if [ ! -d "$VENDOR_DIR/assimp" ]; then
    echo "📦 Cloning Assimp..."
    git clone --depth 1 --branch v5.2.5 https://github.com/assimp/assimp.git $VENDOR_DIR/assimp
else
    echo "✓ Assimp already exists"
fi

# ReactPhysics3D
if [ ! -d "$VENDOR_DIR/reactphysics3d" ]; then
    echo "📦 Cloning ReactPhysics3D..."
    git clone --depth 1 --branch v0.9.0 https://github.com/DanielChappuis/reactphysics3d.git $VENDOR_DIR/reactphysics3d
else
    echo "✓ ReactPhysics3D already exists"
fi

# OpenAL
if [ ! -d "$VENDOR_DIR/OpenAL" ]; then
    echo "📦 Cloning OpenAL-Soft..."
    git clone --depth 1 --branch 1.23.0 https://github.com/kcat/openal-soft.git $VENDOR_DIR/OpenAL
else
    echo "✓ OpenAL already exists"
fi

# STB
if [ ! -d "$VENDOR_DIR/stb" ]; then
    echo "📦 Cloning STB..."
    git clone --depth 1 https://github.com/nothings/stb.git $VENDOR_DIR/stb
else
    echo "✓ STB already exists"
fi

# TinyObjLoader
if [ ! -f "$VENDOR_DIR/tiny_obj_loader.h" ]; then
    echo "📦 Downloading TinyObjLoader..."
    mkdir -p $VENDOR_DIR/tinyobjloader_temp
    git clone --depth 1 --branch v2.0.0rc10 https://github.com/tinyobjloader/tinyobjloader.git $VENDOR_DIR/tinyobjloader_temp
    cp $VENDOR_DIR/tinyobjloader_temp/tiny_obj_loader.h $VENDOR_DIR/
    rm -rf $VENDOR_DIR/tinyobjloader_temp
else
    echo "✓ TinyObjLoader already exists"
fi

echo ""
echo "======================================"
echo "✅ All dependencies downloaded!"
echo "======================================"
echo ""
echo "Dependency Summary:"
echo "  - GLFW (Windowing)"
echo "  - GLAD (OpenGL Loader)"
echo "  - GLM (Math Library)"
echo "  - fmt (Formatting)"
echo "  - spdlog (Logging)"
echo "  - nlohmann/json (JSON Parser)"
echo "  - Assimp (Asset Importer)"
echo "  - ReactPhysics3D (Physics Engine)"
echo "  - OpenAL-Soft (Audio)"
echo "  - STB (Image Loading)"
echo "  - TinyObjLoader (OBJ Loading)"
echo ""
echo "You can now run: cmake -B build && cmake --build build"
