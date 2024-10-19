#include "CubeMapping.h"

void CubeMapping::Initialize(ComPtr<ID3D11Device>& device, const wchar_t* skyboxFilename, const wchar_t* diffuseFilename, const wchar_t* specularFilename) {
    // .dds 파일 읽어들여서 초기화
    D3D11Utils::CreateCubemapTexture(device, skyboxFilename, m_skyboxResView);
    D3D11Utils::CreateCubemapTexture(device, diffuseFilename, m_diffuseResView);
    D3D11Utils::CreateCubemapTexture(device, specularFilename, m_specularResView);

    // 메쉬 객체 생성
    m_cubeMesh = std::make_shared<Mesh>();

    D3D11Utils::CreateConstantBuffer(device, m_vertexConstantData, m_vertexConstantBuffer);

    MeshData cubeMeshData = GeometryGenerator::CreateBox(20.0f);
    //MeshData cubeMeshData = GeometryGenerator::MakeSphere(10.0f, 100, 100);
    std::reverse(cubeMeshData.indices.begin(), cubeMeshData.indices.end());

    D3D11Utils::CreateVertexBuffer(device, cubeMeshData.vertices, m_cubeMesh->m_vertexBuffer);
    D3D11Utils::CreateIndexBuffer(device, cubeMeshData.indices, m_cubeMesh->m_indexBuffer);

    m_cubeMesh->m_indexCount = UINT(cubeMeshData.indices.size());

    // input elements 설정
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 6, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    // 버텍스 쉐이더와 픽셀 쉐이더 생성
    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"CubeMappingVertexShader.hlsl", inputElements, m_vertexShader, m_inputLayout);
    D3D11Utils::CreatePixelShader(device, L"CubeMappingPixelShader.hlsl", m_pixelShader);

    // Texture sampler 만들기
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the Sample State
    device->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf());
}

void CubeMapping::Render(ComPtr<ID3D11DeviceContext>& context) {
    // VS, PS 설정
    context->VSSetShader(m_vertexShader.Get(), 0, 0);
    context->PSSetShader(m_pixelShader.Get(), 0, 0);

    // constant buffer 설정
    context->VSSetConstantBuffers(0, 1, m_vertexConstantBuffer.GetAddressOf());
    //context->PSSetConstantBuffers(0, 1, m_pixelConstantBuffer.GetAddressOf());

    // sampler 설정
    context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

    // input layout 설정
    context->IASetInputLayout(m_inputLayout.Get());

    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    context->IASetVertexBuffers(0, 1, m_cubeMesh->m_vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(m_cubeMesh->m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // SRV 설정
    context->PSSetShaderResources(0, 1, m_skyboxResView.GetAddressOf());

    context->DrawIndexed(m_cubeMesh->m_indexCount, 0, 0);
}

void CubeMapping::UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context) {
    // 버퍼 업데이트
    D3D11Utils::UpdateBuffer(device, context, m_vertexConstantData, m_vertexConstantBuffer);
    //D3D11Utils::UpdateBuffer(device, context, m_pixelConstantData, m_pixelConstantBuffer);
}