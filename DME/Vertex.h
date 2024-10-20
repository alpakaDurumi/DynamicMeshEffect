#pragma once

#include <DirectXMath.h>

struct Vertex {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 texCoord;
    DirectX::XMFLOAT3 faceNormal;

    Vertex()
        : position(), normal(), texCoord(), faceNormal() {}
    Vertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT2 texCoord)
        : position(position), normal(normal), texCoord(texCoord), faceNormal() {}
    Vertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty)
        : position(px, py, pz), normal(nx, ny, nz), texCoord(tx, ty), faceNormal() {}
};

struct ShellVertex :public Vertex {
    uint32_t isOutside;     // true(1)라면 바깥쪽, false(0)라면 안쪽
};