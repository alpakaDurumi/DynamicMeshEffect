#pragma once

#include <DirectXMath.h>

using namespace DirectX;

struct Vertex {
    XMFLOAT3 position;
    XMFLOAT3 normal;
    XMFLOAT2 texCoord;

    Vertex()
        : position(), normal(), texCoord() {}
    Vertex(const XMFLOAT3& position, const XMFLOAT3& normal, const XMFLOAT2 texCoord)
        : position(position), normal(normal), texCoord(texCoord) {}
    Vertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty)
        : position(px, py, pz), normal(nx, ny, nz), texCoord(tx, ty) {}
};