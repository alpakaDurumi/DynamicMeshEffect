#include "Material.h"

Material Material::GetMaterialPreset(MaterialPreset preset) {
    Material mat;
    switch (preset) {
    case MaterialPreset::Gold:
        mat = { { 0.24725f, 0.1995f, 0.0745f }, 51.2f,
                { 0.75164f, 0.60648f, 0.22648f }, 0.0f,
                { 0.628281f, 0.555802f, 0.366065f }, 0.0f };
        break;
    case MaterialPreset::Silver:
        mat = { { 0.19225f, 0.19225f, 0.19225f }, 50.0f,
                { 0.50754f, 0.50754f, 0.50754f }, 0.0f,
                { 0.508273f, 0.508273f, 0.508273f }, 0.0f };
        break;
    default:
        mat = { { 0.1f, 0.1f, 0.1f }, 10.0f,
                { 0.7f, 0.7f, 0.7f }, 0.0f,
                { 0.5f, 0.5f, 0.5f }, 0.0f };
        break;
    }
    return mat;
}