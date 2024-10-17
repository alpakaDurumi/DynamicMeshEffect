#pragma once

#include <memory>

#include "D3D11Utils.h"
#include "Mesh.h"

using namespace DirectX;

struct VertexConstantData {
    XMFLOAT4X4 model;
    XMFLOAT4X4 invTranspose;
    XMFLOAT4X4 view;
    XMFLOAT4X4 projection;
};

class MeshGroup {
public:
    void Initialize(ComPtr<ID3D11Device>& device);
    void AddMesh(ComPtr<ID3D11Device>& device, const std::vector<MeshData>& meshes);
    void Render(ComPtr<ID3D11DeviceContext>& context);
    void UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
        const XMFLOAT3& scale, const XMFLOAT3& rotation, const XMFLOAT3& translation,
        float fovY, float aspect, float nearZ, float farZ);
private:
    std::vector<std::shared_ptr<Mesh>> m_meshes;

    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11InputLayout> m_inputLayout;

    ComPtr<ID3D11SamplerState> m_samplerState;
    ComPtr<ID3D11Buffer> m_vertexConstantBuffer;
    ComPtr<ID3D11Buffer> m_pixelConstantBuffer;

    VertexConstantData m_vertexConstantData;
    //PixelConstantData m_pixelConstantData;
};
