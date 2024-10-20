#include "ShellMeshGroup.h"

void ShellMeshGroup::Initialize(ComPtr<ID3D11Device>& device) {
    // constant data 초기화
    XMMATRIX tempMatrix = XMMatrixIdentity();
    XMStoreFloat4x4(&m_vertexConstantData.model, tempMatrix);
    XMStoreFloat4x4(&m_vertexConstantData.invTranspose, tempMatrix);
    XMStoreFloat4x4(&m_vertexConstantData.view, tempMatrix);
    XMStoreFloat4x4(&m_vertexConstantData.projection, tempMatrix);

    // constant buffer 생성
    D3D11Utils::CreateConstantBuffer(device, m_vertexConstantData, m_vertexConstantBuffer);
    D3D11Utils::CreateConstantBuffer(device, m_shellVertexConstantData, m_shellVertexConstantBuffer);
    D3D11Utils::CreateConstantBuffer(device, m_pixelConstantData, m_pixelConstantBuffer);

    // sampler 생성
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf()); 

    // input elements 설정
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 6, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 8, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    // 버텍스 쉐이더와 픽셀 쉐이더 생성
    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"ShellVertexShader.hlsl", inputElements, m_vertexShader, m_inputLayout);
    D3D11Utils::CreatePixelShader(device, L"BasicPixelShader.hlsl", m_pixelShader);
}

void ShellMeshGroup::Render(ComPtr<ID3D11DeviceContext>& context) {
    // VS, PS 설정
    context->VSSetShader(m_vertexShader.Get(), 0, 0);
    context->PSSetShader(m_pixelShader.Get(), 0, 0);

    // constant buffer 설정
    ID3D11Buffer* vertexConstantBuffers[] = {
        m_vertexConstantBuffer.Get(),
        m_shellVertexConstantBuffer.Get()
    };
    context->VSSetConstantBuffers(0, 2, vertexConstantBuffers);
    context->PSSetConstantBuffers(0, 1, m_pixelConstantBuffer.GetAddressOf());

    // sampler 설정
    context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

    // input layout 설정
    context->IASetInputLayout(m_inputLayout.Get());

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    for (const auto& mesh : m_meshes) {
        context->IASetVertexBuffers(0, 1, mesh->m_vertexBuffer.GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(mesh->m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // SRV 설정
        ID3D11ShaderResourceView* resViews[3] = {
            mesh->m_shaderResourceView.Get(),
            m_diffuseResView.Get(),
            m_specularResView.Get() };
        context->PSSetShaderResources(0, 3, resViews);

        context->DrawIndexed(mesh->m_indexCount, 0, 0);
    }
}

void ShellMeshGroup::UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context) {
    MeshGroup::UpdateConstantBuffers(device, context);

    // 추가로 업데이트
    D3D11Utils::UpdateBuffer(device, context, m_shellVertexConstantData, m_shellVertexConstantBuffer);
}