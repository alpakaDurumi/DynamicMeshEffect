#pragma once

#include "DirectXMath.h"

using DirectX::XMFLOAT3;

enum class MaterialPreset {
    Gold,
    Silver,
};

struct Material {
    XMFLOAT3 ambient = { 0.1f, 0.1f, 0.1f };
    float shininess = 10.0f;
    XMFLOAT3 diffuse = { 0.7f, 0.7f, 0.7f };
    float padding1;
    XMFLOAT3 specular = { 0.5f, 0.5f, 0.5f };
    float padding2;

    static Material GetMaterialPreset(MaterialPreset preset);
};