#pragma once

#include <string>

// forward-declare Mesh to avoid circular includes
namespace engine { namespace gl { class Mesh; } }
using MeshPtr = engine::gl::Mesh*;

namespace engine {
namespace assets {

class MeshLoader {
public:
    // Load an OBJ file and upload its buffers to the GPU,
    // then assign the created VAO / element_count into the provided Mesh instance.
    // Returns true on success.
    static bool load_obj(MeshPtr mesh, const std::string& file_path);
};

} // namespace assets
} // namespace engine
