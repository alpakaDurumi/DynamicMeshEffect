#pragma once

#include "MeshGroup.h"

class CubeMapping :public MeshGroup {
public:
    void Initialize(ComPtr<ID3D11Device>& device, const wchar_t* diffuseFilename, const wchar_t* specularFilename);
    void Render(ComPtr<ID3D11DeviceContext>& context);
    void UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context);

    struct VertexConstantData {
        XMFLOAT4X4 viewProj;
    };

    CubeMapping::VertexConstantData m_vertexConstantData;
private:
    std::shared_ptr<Mesh> m_cubeMesh;

    ComPtr<ID3D11ShaderResourceView> m_diffuseResView;
    ComPtr<ID3D11ShaderResourceView> m_specularResView;
};
