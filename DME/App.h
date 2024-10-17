#pragma once

#include <windows.h>
#include <wrl.h>

#include <d3d11.h>
#include <d3dcompiler.h>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "MeshGroup.h"

using Microsoft::WRL::ComPtr;

class App {
public:
    App();
    ~App();

    bool Initialize();

    int Run();

    LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
    bool InitWindow();
    bool InitDirect3D();
    bool InitGUI();

    void UpdateMVP();
    void UpdateGUI();

    void Render();

    int m_screenWidth;
    int m_screenHeight;
    HWND m_hWnd = nullptr;

    ComPtr<IDXGISwapChain> m_swapChain;
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_context;
    ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    ComPtr<ID3D11RasterizerState> m_rasterizerState;
    ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    ComPtr<ID3D11DepthStencilState> m_depthStencilState;

    MeshGroup m_OriginalMeshGroup;
    MeshGroup m_ShellMeshGroup;

    // MVP За·Д
    XMFLOAT3 m_scale = { 1.0f, 1.0f, 1.0f };
    XMFLOAT3 m_rotation = { 0.0f, 0.0f, 0.0f };
    XMFLOAT3 m_translation = { 0.0f, 0.0f, 0.0f };
    float m_fovY = 70.0f;
    float m_nearZ = 0.01f;
    float m_farZ = 100.0f;
};