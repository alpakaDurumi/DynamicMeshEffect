#pragma once

#include <DirectXMath.h>

struct Vertex {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 texCoord;

    Vertex()
        : position(), normal(), texCoord() {}
    Vertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT2 texCoord)
        : position(position), normal(normal), texCoord(texCoord) {}
    Vertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty)
        : position(px, py, pz), normal(nx, ny, nz), texCoord(tx, ty) {}
};