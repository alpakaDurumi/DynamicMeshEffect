#include "App.h"
#include "GeometryGenerator.h"

// ���� ����
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ��� �Լ��� ���������� ȣ���ϱ� ���� ���� ������
App* g_app = nullptr;

App::App()
    : m_screenWidth(1280), m_screenHeight(720) {
    g_app = this;
}

App::~App() {
    g_app = nullptr;

    // ImGui
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    DestroyWindow(m_hWnd);
}

bool App::Initialize() {
    if (!InitWindow()) return false;
    if (!InitDirect3D()) return false;
    if (!InitGUI()) return false;

    m_OriginalMeshGroup.Initialize(m_device);

    //auto box = GeometryGenerator::CreateBox();
    //box.textureFilename = "wall.jpg";
    //m_OriginalMeshGroup.AddMesh(m_device, { box });

    // from https://f3d.app//doc/GALLERY.html
    auto zelda = GeometryGenerator::ReadFromFile("C:/Users/duram/Downloads/zelda/", "zeldaPosed001.fbx");
    m_OriginalMeshGroup.AddMesh(m_device, { zelda });

    return true;
}

int App::Run() {
    // ������ �޽��� ó�� ����
    MSG msg = {};
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            // ImGui frame ����
            ImGui_ImplWin32_NewFrame();
            ImGui_ImplDX11_NewFrame();
            ImGui::NewFrame();
            
            // ImGui�� ���� �� ������Ʈ
            UpdateGUI();

            // constant data ������Ʈ
            UpdateVertexConstantData();
            UpdatePixelConstantData();

            // ���� ������Ʈ
            m_OriginalMeshGroup.UpdateConstantBuffers(m_device, m_context);

            // ������
            Render();

            // ImGui frame ����
            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            // swap chain ����
            m_swapChain->Present(1, 0);
        }
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return g_app->MsgProc(hWnd, msg, wParam, lParam);
}

LRESULT App::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool App::InitWindow() {
    // DPI �ν� ����
    SetProcessDPIAware();

    // ������ Ŭ���� ����
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = L"DMEWindowClass";
    wc.hIconSm = nullptr;

    // ������ Ŭ���� ���
    if (!RegisterClassEx(&wc)) {
        MessageBox(nullptr, L"Window Registration Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // ������ ��Ÿ��
    DWORD dwStyle = WS_OVERLAPPEDWINDOW;
    DWORD dwExStyle = WS_EX_APPWINDOW;      // �۾� ǥ���ٿ� ������ ǥ��

    // ������ ũ�� ����
    // ���� �� -> ������ �Ʒ� ����
    RECT rect = { 0, 0, m_screenWidth, m_screenHeight };
    AdjustWindowRectEx(&rect, dwStyle, false, dwExStyle);

    // ������ ����
    m_hWnd = CreateWindowEx(
        dwExStyle,
        wc.lpszClassName,
        L"Dynamic Mesh Effect",
        dwStyle,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        NULL, NULL, wc.hInstance, NULL);

    if (!m_hWnd) {
        MessageBox(nullptr, L"Window Creation Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    ShowWindow(m_hWnd, SW_SHOWDEFAULT);
    UpdateWindow(m_hWnd);

    return true;
}

bool App::InitDirect3D() {
    // swap chain desc
    DXGI_SWAP_CHAIN_DESC scDesc;
    ZeroMemory(&scDesc, sizeof(scDesc));
    scDesc.BufferDesc.Width = m_screenWidth;
    scDesc.BufferDesc.Height = m_screenHeight;
    scDesc.BufferDesc.RefreshRate.Numerator = 60;
    scDesc.BufferDesc.RefreshRate.Denominator = 1;
    scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.SampleDesc.Count = 1;     // not use MSAA
    scDesc.SampleDesc.Quality = 0;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.BufferCount = 2;
    scDesc.OutputWindow = m_hWnd;
    scDesc.Windowed = TRUE;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // feature level
    D3D_FEATURE_LEVEL featureLevels[2] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0
    };
    D3D_FEATURE_LEVEL featureLevel;

    // ����� ��� �� �÷��� ����
    UINT createDeviceFlags = 0;
#if defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // device �� swap chain ����
    if (FAILED(D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &scDesc,
        m_swapChain.GetAddressOf(),
        m_device.GetAddressOf(),
        &featureLevel,
        m_context.GetAddressOf()))) {
        MessageBox(nullptr, L"Device and Swap Chain Creation Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // RTV ����
    ComPtr<ID3D11Texture2D> backBuffer;
    m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    if (FAILED(m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf()))) {
        MessageBox(nullptr, L"Render Target View Creation Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // ����Ʈ ����
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(m_screenWidth);
    viewport.Height = static_cast<float>(m_screenHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_context->RSSetViewports(1, &viewport);
    
    // rasterizer state ����
    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
    rastDesc.FrontCounterClockwise = FALSE;
    rastDesc.DepthClipEnable = TRUE;
    if (FAILED(m_device->CreateRasterizerState(&rastDesc, m_rasterizerState.GetAddressOf()))) {
        MessageBox(nullptr, L"Rasterizer State Creation Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }
    m_context->RSSetState(m_rasterizerState.Get());

    // depth stencil buffer desc
    D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
    ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
    depthStencilBufferDesc.Width = m_screenWidth;
    depthStencilBufferDesc.Height = m_screenHeight;
    depthStencilBufferDesc.MipLevels = 1;
    depthStencilBufferDesc.ArraySize = 1;
    depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilBufferDesc.SampleDesc.Count = 1;
    depthStencilBufferDesc.SampleDesc.Quality = 0;
    depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilBufferDesc.CPUAccessFlags = 0;
    depthStencilBufferDesc.MiscFlags = 0;

    // depth stencil buffer ����
    ComPtr<ID3D11Texture2D> depthStencilBuffer;
    if (FAILED(m_device->CreateTexture2D(&depthStencilBufferDesc, nullptr, depthStencilBuffer.GetAddressOf()))) {
        MessageBox(nullptr, L"Depth Stencil Buffer Creation Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // depth stencil view ����
    if (FAILED(m_device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, m_depthStencilView.GetAddressOf()))) {
        MessageBox(nullptr, L"Depth Stencil View Creation Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // depth stencil state ����
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    depthStencilDesc.StencilEnable = FALSE;
    if (FAILED(m_device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.GetAddressOf()))) {
        MessageBox(nullptr, L"Depth Stencil State Creation Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

bool App::InitGUI() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(static_cast<float>(m_screenWidth), static_cast<float>(m_screenHeight));
    ImGui::StyleColorsDark();

    if (!ImGui_ImplWin32_Init(m_hWnd)) {
        MessageBox(nullptr, L"ImGui Win32 Initialization Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    if (!ImGui_ImplDX11_Init(m_device.Get(), m_context.Get())) {
        MessageBox(nullptr, L"ImGui DX11 Initialization Failed!", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

void App::UpdateVertexConstantData() {
    // model
    XMMATRIX modelMatrix = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
        XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z) *
        XMMatrixTranslation(m_translation.x, m_translation.y, m_translation.z);
    XMStoreFloat4x4(&m_OriginalMeshGroup.m_vertexConstantData.model, XMMatrixTranspose(modelMatrix));

    // invTranspose
    XMMATRIX invTransposeMatrix = modelMatrix * XMMatrixTranslation(0.0f, 0.0f, 0.0f);
    invTransposeMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, modelMatrix));
    XMStoreFloat4x4(&m_OriginalMeshGroup.m_vertexConstantData.invTranspose, XMMatrixTranspose(invTransposeMatrix));

    // view
    XMMATRIX viewMatrix = XMMatrixLookAtLH({ 0.0f, 0.0f, -2.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
    XMStoreFloat4x4(&m_OriginalMeshGroup.m_vertexConstantData.view, XMMatrixTranspose(viewMatrix));

    // projection
    XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(m_fovY, static_cast<float>(m_screenWidth) / m_screenHeight, m_nearZ, m_farZ);
    XMStoreFloat4x4(&m_OriginalMeshGroup.m_vertexConstantData.projection, XMMatrixTranspose(projectionMatrix));
}

void App::UpdatePixelConstantData() {
    // direction ����ȭ
    XMVECTOR dir = XMVector3Normalize(XMLoadFloat3(&m_light[m_lightType].direction));
    XMStoreFloat3(&m_light[m_lightType].direction, dir);

    m_OriginalMeshGroup.m_pixelConstantData.light = m_light[m_lightType];
    m_OriginalMeshGroup.m_pixelConstantData.lightType = m_lightType;
}

void App::UpdateGUI() {
    // �� ��ȯ
    ImGui::SliderFloat3("Scale", &m_scale.x, 0.1f, 2.0f);
    ImGui::SliderFloat3("Rotation", &m_rotation.x, -3.14f, 3.14f);
    ImGui::SliderFloat3("Translation", &m_translation.x, -1.0f, 1.0f);

    // �ؽ�ó ���
    ImGui::Checkbox("Use Texture", &m_OriginalMeshGroup.m_pixelConstantData.useTexture);

    // ���� Ÿ��
    if (ImGui::RadioButton("Directional Light", m_lightType == 0)) {
        m_lightType = 0;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Point Light", m_lightType == 1)) {
        m_lightType = 1;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Spot Light", m_lightType == 2)) {
        m_lightType = 2;
    }

    // ���� �Ӽ�
    ImGui::SliderFloat3("Light Position", &m_light[m_lightType].position.x, -5.0f, 5.0f);
    ImGui::SliderFloat3("Light Direction", &m_light[m_lightType].direction.x, -5.0f, 5.0f);
    ImGui::SliderFloat("Light fallOffStart", &m_light[m_lightType].fallOffStart, 0.0f, 5.0f);
    ImGui::SliderFloat("Light fallOffEnd", &m_light[m_lightType].fallOffEnd, 0.0f, 10.0f);
    ImGui::SliderFloat("Light spotPower", &m_light[m_lightType].spotPower, 1.0f, 512.0f);
}

void App::Render() {
    // RTV, depth stencil view Ŭ����
    float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // ������ ���
    m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
    m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // RTV, depth stencil state ����
    m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
    m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

    // MeshGroup ����
    m_OriginalMeshGroup.Render(m_context);
}