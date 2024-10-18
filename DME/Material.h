#pragma once

#include "DirectXMath.h"

using DirectX::XMFLOAT3;

struct Material {
    XMFLOAT3 ambient = { 0.1f, 0.1f, 0.1f };
    float shininess = 1.0f;
    XMFLOAT3 diffuse = { 0.5f, 0.5f, 0.5f };
    float padding1;
    XMFLOAT3 specular = { 1.0f, 1.0f, 1.0f };
    float padding2;
};

