#include "MeshGroup.h"

// 쉐이더 파일 이름 접두사를 인자로 받도록 수정하기
void MeshGroup::Initialize(ComPtr<ID3D11Device>& device) {
    // constant data 초기화
    XMMATRIX tempMatrix = XMMatrixIdentity();
    XMStoreFloat4x4(&m_vertexConstantData.model, tempMatrix);
    XMStoreFloat4x4(&m_vertexConstantData.invTranspose, tempMatrix);
    XMStoreFloat4x4(&m_vertexConstantData.view, tempMatrix);
    XMStoreFloat4x4(&m_vertexConstantData.projection, tempMatrix);

    // constant buffer 생성
    D3D11Utils::CreateConstantBuffer(device, m_vertexConstantData, m_vertexConstantBuffer);
    //D3D11Utils::CreateConstantBuffer(device, m_basicPixelConstantData, m_pixelConstantBuffer);

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
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 6, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    // 버텍스 쉐이더와 픽셀 쉐이더 생성
    // 각 함수의 두번째 인자 수정해야 함
    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"VertexShader.hlsl", inputElements, m_vertexShader, m_inputLayout);
    D3D11Utils::CreatePixelShader(device, L"PixelShader.hlsl", m_pixelShader);
}

void MeshGroup::AddMesh(ComPtr<ID3D11Device>& device, const std::vector<MeshData>& meshes) {
    // 그룹에 메쉬 추가
    for (const auto& meshData : meshes) {
        auto newMesh = std::make_shared<Mesh>();

        // 버텍스 버퍼와 인덱스 버퍼 생성
        D3D11Utils::CreateVertexBuffer(device, meshData.vertices, newMesh->m_vertexBuffer);
        D3D11Utils::CreateIndexBuffer(device, meshData.indices, newMesh->m_indexBuffer);

        // 인덱스 개수 지정
        newMesh->m_indexCount = static_cast<UINT>(meshData.indices.size());

        // 텍스처 파일 존재 시 텍스처 생성
        if (!meshData.textureFilename.empty()) {
            D3D11Utils::CreateTexture(device, meshData.textureFilename, newMesh->m_texture, newMesh->m_shaderResourceView);
        }

        m_meshes.push_back(newMesh);
    }
}

void MeshGroup::Render(ComPtr<ID3D11DeviceContext>& context) {
    // VS, PS 설정
    context->VSSetShader(m_vertexShader.Get(), 0, 0);
    context->PSSetShader(m_pixelShader.Get(), 0, 0);

    // constant buffer 설정
    context->VSSetConstantBuffers(0, 1, m_vertexConstantBuffer.GetAddressOf());
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
        context->PSSetShaderResources(0, 1, mesh->m_shaderResourceView.GetAddressOf());

        context->DrawIndexed(mesh->m_indexCount, 0, 0);
    }
}

void MeshGroup::UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context) {
    // 버퍼 업데이트
    D3D11Utils::UpdateBuffer(device, context, m_vertexConstantData, m_vertexConstantBuffer);
    //D3D11Utils::UpdateBuffer(device, context, m_pixelConstantData, m_pixelConstantBuffer);
}