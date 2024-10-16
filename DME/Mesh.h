#pragma once

#include <windows.h>
#include <wrl.h>
#include <d3d11.h>

#include "GeometryGenerator.h"

using Microsoft::WRL::ComPtr;

class Mesh {
public:
    ComPtr<ID3D11Buffer> m_vertexBuffer;
    ComPtr<ID3D11Buffer> m_indexBuffer;
};