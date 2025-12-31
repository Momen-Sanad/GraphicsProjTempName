#pragma once

class GpuMesh;
class Material;

struct MeshRenderer {
    GpuMesh* mesh = nullptr;
    Material* material = nullptr;
};
