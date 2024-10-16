#pragma once

#include <vector>
#include <DirectXMath.h>
#include <windows.h>

#include "Vertex.h"

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;
};

class GeometryGenerator {
public:
    static MeshData CreateBox(const float scale = 1.0f);
};