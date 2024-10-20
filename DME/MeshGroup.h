#pragma once

#include <memory>

#include "D3D11Utils.h"
#include "Mesh.h"
#include "Material.h"
#include "Light.h"

using namespace DirectX;

struct VertexConstantData {
    XMFLOAT4X4 model;
    XMFLOAT4X4 invTranspose;
    XMFLOAT4X4 view;
    XMFLOAT4X4 projection;
};

struct PixelConstantData {
    XMFLOAT3 viewWorld;
    bool useTexture;
    Material material;
    Light light;
    int lightType;
    XMFLOAT3 padding;
};

class MeshGroup {
public:
    void Initialize(ComPtr<ID3D11Device>& device);
    void AddMesh(ComPtr<ID3D11Device>& device, const std::vector<MeshData>& meshes);
    void Render(ComPtr<ID3D11DeviceContext>& context);
    virtual void UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context);
    void ClearMeshes();

    VertexConstantData m_vertexConstantData;
    PixelConstantData m_pixelConstantData;

    // 환경 매핑
    ComPtr<ID3D11ShaderResourceView> m_diffuseResView;
    ComPtr<ID3D11ShaderResourceView> m_specularResView;
protected:
    std::vector<std::shared_ptr<Mesh>> m_meshes;

    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11InputLayout> m_inputLayout;

    ComPtr<ID3D11SamplerState> m_samplerState;
    ComPtr<ID3D11Buffer> m_vertexConstantBuffer;
    ComPtr<ID3D11Buffer> m_pixelConstantBuffer;
};
