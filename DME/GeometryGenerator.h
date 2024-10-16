#pragma once

#include <vector>
#include <DirectXMath.h>
#include <windows.h>

using namespace DirectX;

struct Vertex {
    XMFLOAT3 position;
    XMFLOAT3 normal;

    Vertex()
        : position(), normal() {}
    Vertex(const XMFLOAT3& position, const XMFLOAT3& normal)
        : position(position), normal(normal) {}
    Vertex(float px, float py, float pz, float nx, float ny, float nz)
        : position(px, py, pz), normal(nx, ny, nz) {}
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;
};

class GeometryGenerator {
public:
    static MeshData CreateBox(const float scale = 1.0f);
};