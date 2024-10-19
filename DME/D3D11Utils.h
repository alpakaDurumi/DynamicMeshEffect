#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iostream>

#include <windows.h>
#include <wrl.h>

#include <d3d11.h>
#include <d3dcompiler.h>

#include "Vertex.h"

using Microsoft::WRL::ComPtr;

class D3D11Utils {
public:
    static void SetViewport(ComPtr<ID3D11DeviceContext>& context, int screenWidth, int screenHeight);
    static void CreateRenderTargetView(ComPtr<ID3D11Device>& device, ComPtr<IDXGISwapChain>& swapChain, ComPtr<ID3D11RenderTargetView>& renderTargetView);
    static void CreateDepthBuffer(ComPtr<ID3D11Device>& device, int screenWidth, int screenHeight, ComPtr<ID3D11DepthStencilView>& depthStencilView);

    static void CreateVertexBuffer(ComPtr<ID3D11Device>& device, const std::vector<Vertex>& vertices, ComPtr<ID3D11Buffer>& vertexBuffer);
    static void CreateIndexBuffer(ComPtr<ID3D11Device>& device, const std::vector<UINT>& indices, ComPtr<ID3D11Buffer>& indexBuffer);

    static void CreateVertexShaderAndInputLayout(
        ComPtr<ID3D11Device>& device,
        const std::wstring& filename,
        const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
        ComPtr<ID3D11VertexShader>& vertexShader,
        ComPtr<ID3D11InputLayout>& inputLayout);

    static void CreatePixelShader(
        ComPtr<ID3D11Device>& device,
        const std::wstring& filename,
        ComPtr<ID3D11PixelShader>& pixelShader);

    template <typename T_CONSTANT>
    static void CreateConstantBuffer(ComPtr<ID3D11Device>& device,
        const T_CONSTANT& constantBufferData,
        ComPtr<ID3D11Buffer>& constantBuffer) {

        D3D11_BUFFER_DESC cbDesc;
        ZeroMemory(&cbDesc, sizeof(D3D11_BUFFER_DESC));
        cbDesc.ByteWidth = sizeof(T_CONSTANT);
        cbDesc.Usage = D3D11_USAGE_DYNAMIC;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbDesc.MiscFlags = 0;
        cbDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = &constantBufferData;
        initData.SysMemPitch = 0;
        initData.SysMemSlicePitch = 0;

        // 버퍼 생성
        HRESULT hr = device->CreateBuffer(
            &cbDesc,
            &initData,
            constantBuffer.GetAddressOf());

        // 검사
        if (FAILED(hr)) {
            std::cout << "CreateConstantBuffer() failed()." << std::endl;
        }
    }

    template <typename T_DATA>
    static void UpdateBuffer(
        ComPtr<ID3D11Device>& device,
        ComPtr<ID3D11DeviceContext>& context,
        const T_DATA& bufferData,
        ComPtr<ID3D11Buffer>& buffer) {

        if (!buffer.Get())
            std::cout << "Buffer was not initialized." << std::endl;

        D3D11_MAPPED_SUBRESOURCE ms;
        HRESULT hr = context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
        if (FAILED(hr)) {
            std::cout << "Buffer map failed." << std::endl;
            return;
        }

        // 메모리 복사
        memcpy(ms.pData, &bufferData, sizeof(bufferData));

        context->Unmap(buffer.Get(), NULL);
    }

    static void CreateTexture(
        ComPtr<ID3D11Device>& device,
        const std::string filename,
        ComPtr<ID3D11Texture2D>& texture,
        ComPtr<ID3D11ShaderResourceView>& textureResourceView);

    static void CreateCubemapTexture(
        ComPtr<ID3D11Device>& device,
        const wchar_t* filename,
        ComPtr<ID3D11ShaderResourceView>& textureResourceView);
};
