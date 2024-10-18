#include "D3D11Utils.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void D3D11Utils::CreateVertexBuffer(ComPtr<ID3D11Device>& device, const std::vector<Vertex>& vertices, ComPtr<ID3D11Buffer>& vertexBuffer) {
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = sizeof(Vertex);

    D3D11_SUBRESOURCE_DATA subDesc;
    ZeroMemory(&subDesc, sizeof(D3D11_SUBRESOURCE_DATA));
    subDesc.pSysMem = vertices.data();
    subDesc.SysMemPitch = 0;
    subDesc.SysMemSlicePitch = 0;

    if (FAILED(device->CreateBuffer(&bufferDesc, &subDesc, vertexBuffer.GetAddressOf()))) {
        std::cout << "CreateBuffer() failed." << std::endl;
    }
}

void D3D11Utils::CreateIndexBuffer(ComPtr<ID3D11Device>& device, const std::vector<UINT>& indices, ComPtr<ID3D11Buffer>& indexBuffer) {
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = sizeof(UINT);

    D3D11_SUBRESOURCE_DATA subDesc;
    ZeroMemory(&subDesc, sizeof(D3D11_SUBRESOURCE_DATA));
    subDesc.pSysMem = indices.data();
    subDesc.SysMemPitch = 0;
    subDesc.SysMemSlicePitch = 0;

    if (FAILED(device->CreateBuffer(&bufferDesc, &subDesc, indexBuffer.GetAddressOf()))) {
        std::cout << "CreateBuffer() failed." << std::endl;
    }
}

void D3D11Utils::CreateVertexShaderAndInputLayout(
    ComPtr<ID3D11Device>& device,
    const std::wstring& filename,
    const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
    ComPtr<ID3D11VertexShader>& vertexShader,
    ComPtr<ID3D11InputLayout>& inputLayout) {

    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    // 디버그 모드 시 플래그 설정
    UINT compileFlags = 0;
#if defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    // 쉐이더 컴파일
    HRESULT hr = D3DCompileFromFile(
        filename.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "vs_5_0",
        compileFlags,
        0,
        &shaderBlob,
        &errorBlob);

    // 검사
    if (FAILED(hr)) {
        // 파일이 없을 경우
        if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0)
            std::cout << "Shader file not found." << std::endl;
        // 에러 메시지가 있으면 출력
        if (errorBlob)
            std::cout << "Shader compile error : " << static_cast<char*>(errorBlob->GetBufferPointer()) << std::endl;
    }

    // 버텍스 쉐이더 생성
    device->CreateVertexShader(
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        nullptr,
        vertexShader.GetAddressOf());

    // input layout 생성
    device->CreateInputLayout(
        inputElements.data(),
        static_cast<UINT>(inputElements.size()),
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        inputLayout.GetAddressOf());
}

void D3D11Utils::CreatePixelShader(
    ComPtr<ID3D11Device>& device,
    const std::wstring& filename,
    ComPtr<ID3D11PixelShader>& pixelShader) {

    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    // 디버그 모드 시 플래그 설정
    UINT compileFlags = 0;
#if defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    // 쉐이더 컴파일
    HRESULT hr = D3DCompileFromFile(
        filename.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "ps_5_0",
        compileFlags,
        0,
        &shaderBlob,
        &errorBlob);

    // 검사
    if (FAILED(hr)) {
        // 파일이 없을 경우
        if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0)
            std::cout << "Shader file not found." << std::endl;
        // 에러 메시지가 있으면 출력
        if (errorBlob)
            std::cout << "Shader compile error : " << static_cast<char*>(errorBlob->GetBufferPointer()) << std::endl;
    }

    // 픽셀 쉐이더 생성
    device->CreatePixelShader(
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        nullptr,
        pixelShader.GetAddressOf());
}

void D3D11Utils::CreateTexture(
    ComPtr<ID3D11Device>& device,
    const std::string filename,
    ComPtr<ID3D11Texture2D>& texture,
    ComPtr<ID3D11ShaderResourceView>& shaderResourceView) {

    int width, height, channels;

    unsigned char* img = stbi_load(filename.c_str(), &width, &height, &channels, 0);

    // 텍스처 파일 로드 실패 시
    if (!img) {
        std::cout << "Texture file load failed." << std::endl;
        return;
    }

    // 4채널로 만들어서 복사
    std::vector<BYTE> image;
    image.resize(width * height * 4);
    for (size_t i = 0; i < width * height; i++) {
        for (size_t c = 0; c < 3; c++) {
            image[4 * i + c] = img[i * channels + c];
        }
        image[4 * i + 3] = 255;
    }

    D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(D3D11_TEXTURE2D_DESC));
    txtDesc.Width = width;
    txtDesc.Height = height;
    txtDesc.MipLevels = 1;
    txtDesc.ArraySize = 1;
    txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    txtDesc.SampleDesc.Count = 1;
    txtDesc.SampleDesc.Quality = 0;
    txtDesc.Usage = D3D11_USAGE_IMMUTABLE;
    txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    txtDesc.CPUAccessFlags = 0;
    txtDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = image.data();
    initData.SysMemPitch = txtDesc.Width * sizeof(BYTE) * 4;
    initData.SysMemSlicePitch = 0;

    // 텍스처와 SRV 생성
    device->CreateTexture2D(&txtDesc, &initData, texture.GetAddressOf());
    device->CreateShaderResourceView(texture.Get(), nullptr, shaderResourceView.GetAddressOf());

    // img 메모리 해제
    stbi_image_free(img);
}