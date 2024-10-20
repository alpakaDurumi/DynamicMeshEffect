#pragma once

#include "MeshGroup.h"

struct ShellVertexConstantData {
    XMFLOAT3 mousePos;
    float radius = 0.5f;
};

class ShellMeshGroup :public MeshGroup {
public:
    void Initialize(ComPtr<ID3D11Device>& device);
    void Render(ComPtr<ID3D11DeviceContext>& context);
    void UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context) override;

    ShellVertexConstantData m_shellVertexConstantData;
private:
    ComPtr<ID3D11Buffer> m_shellVertexConstantBuffer;
};